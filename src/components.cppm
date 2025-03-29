export module components;

import std;
import web;
import web_coro;
import webxx;
import nlohmann_json;

export template<class... Args>
struct ActionHandlerList {
    using function_signature = void(Args...);
    using function_type = std::function<function_signature>;
    using ext_function_signature = void(ActionHandlerList<Args...>&, Args...);
    using ext_function_type = std::function<ext_function_signature>;

    std::list<ext_function_type> handlers;
    decltype(handlers)::iterator current_handler;

    void* operator+=(function_type&& handler) {
        handlers.push_back([handler = std::move(handler)](ActionHandlerList<Args...>&, Args... args) {
            handler(args...);
        });
        return &handlers.back();
    }
    void* operator+=(ext_function_type&& handler) {
        handlers.push_back(std::move(handler));
        return &handlers.back();
    }

    auto add_unique(function_type&& handler) {
        return std::unique_ptr<void, std::function<void(void*)>>{*this += std::move(handler), [this](void* handler) {
            remove(handler);
        }};
    }
    auto add_unique(ext_function_type&& handler) {
        return std::unique_ptr<void, std::function<void(void*)>>{*this += std::move(handler), [this](void* handler) {
            remove(handler);
        }};
    }

    void operator()(Args... args) {
        for(current_handler = handlers.begin(); current_handler != handlers.end(); ++current_handler) {
            (*current_handler)(*this, args...);
        }
    }
    operator bool() const {
        return !handlers.empty();
    }
    bool empty() const {
        return handlers.empty();
    }
    void clear() {
        handlers.clear();
    }
    void remove_current() {
        if(current_handler != handlers.end()) {
            current_handler = handlers.erase(current_handler++);
        }
    }
    void remove(void* handler) {
        auto it = std::find_if(handlers.begin(), handlers.end(), [handler](const auto& h) {
            return &h == handler;
        });
        if(it != handlers.end()) {
            if(it == current_handler) {
                remove_current();
            } else {
                handlers.erase(it);
            }
        }
    }
};

export class Window {
    public:
        constexpr Window(std::string&& id, std::string&& title, std::add_pointer_t<Webxx::fragment()> content) :
            id(id), m_title(title), m_content(content) {}
        const std::string id;

        auto render_window(int x = 0, int y = 0) {
            using namespace Webxx;
            return dv { {_id{id}, _class{"window"}, _style{std::format("left:{}px;top:{}px", x, y)}},
                dv { {_id{std::string{id}+"__titlebar"}, _class{"titlebar"}},
                    h3 {m_title},
                    button { {_id{std::string{id}+"__minimize"}, _class{"minimize"}},"_"},
                    button { {_id{std::string{id}+"__maximize"}, _class{"maximize"}},"□"},
                    button { {_id{std::string{id}+"__close"}, _class{"close"}},"×"},
                },
                hr{},
                dv { {_class{"content"}},
                    m_content()
                }
            };
        }

        void setup(int initial_x, int initial_y) {
            web::add_event_listener(id+"__titlebar", "mousedown", [this](std::string_view j) {
                nlohmann::json json = nlohmann::json::parse(j);
                if(json["button"] != 0)
                    return;
                int offsetLeft = web::get_property_int(id, "offsetLeft").value_or(0);
                int offsetTop = web::get_property_int(id, "offsetTop").value_or(0);
                grab_start_x = static_cast<int>(json["clientX"]) - offsetLeft;
                grab_start_y = static_cast<int>(json["clientY"]) - offsetTop;
                grabbed_window = this;
                web::add_class(id, "grabbed");
            }, false, true);
            web::add_event_listener(id, "mousedown", [this](std::string_view) {
                bring_to_front();
            });
            web::add_event_listener(id+"__minimize", "click", [this](std::string_view) {
                minimize();
            });
            web::add_event_listener(id+"__maximize", "click", [this](std::string_view) {
                toggle_maximize();
            });
            web::add_event_listener(id+"__close", "click", [this](std::string_view) {
                close();
            });
            move(initial_x, initial_y);
        }
        void bring_to_front() {
            web::set_style_property(id, "zIndex", "{}", highest_z_index++);
            if(on_focus) { on_focus(); }
        }
        void minimize() {
            web::add_class(id, "minimized");
            if(on_minimize) { on_minimize(true); }
        }
        void toggle_minimize() {
            web::toggle_class(id, "minimized");
            if(on_minimize) { on_minimize(web::has_class(id, "minimized").value()); }
        }
        void maximize() {
            web::add_class(id, "maximized");
            if(on_maximize) { on_maximize(true); }
        }
        void toggle_maximize() {
            web::toggle_class(id, "maximized");
            if(on_maximize) { on_maximize(web::has_class(id, "maximized").value()); }
        }
        void restore() {
            web::remove_class(id, "minimized");
            web::remove_class(id, "maximized");
            if(on_restore) { on_restore(); }
        }
        void move(int x, int y) {
            web::set_style_property(id, "left", "{}px", x);
            web::set_style_property(id, "top", "{}px", y);
            m_x = x;
            m_y = y;
            if(on_move) { on_move(m_x, m_y); }
        }
        void close() {
            web::remove_element(id);
            m_open = false;
            if(on_close) { on_close(); }
        }

        [[nodiscard("This is a coroutine, you must either co_await or submit it.")]]
        auto open() {
            return open(m_x, m_y);
        }

        [[nodiscard("This is a coroutine, you must either co_await or submit it.")]]
        auto open(int initial_x, int initial_y) -> web::coro::coroutine<void> {
            std::string html = Webxx::render(render_window(initial_x, initial_y));
            web::add_element_html("main", html);
            co_await web::coro::next_tick();
            setup(initial_x, initial_y);
            m_open = true;
            if(on_open) { on_open(); }
            co_return;
        }

        bool is_open() const {
            return m_open;
        }
        int get_x() const {
            return m_x;
        }
        int get_y() const {
            return m_y;
        }

        ActionHandlerList<> on_open;
        ActionHandlerList<> on_close;
        ActionHandlerList<> on_focus;
        ActionHandlerList<bool> on_minimize;
        ActionHandlerList<bool> on_maximize;
        ActionHandlerList<> on_restore;
        ActionHandlerList<int, int> on_move;

        static void setup() {
            web::add_event_listener("__document__", "mouseup", global_mouseup);
            web::add_event_listener("__document__", "mousemove", global_mousemove);
        }

    private:
        std::string m_title;
        std::add_pointer_t<Webxx::fragment()> m_content;
        bool m_open = false;
        int m_x = 0;
        int m_y = 0;

        static inline int highest_z_index = 1;
        static inline int grab_start_x = 0;
        static inline int grab_start_y = 0;
        static inline struct Window* grabbed_window = nullptr;
        static void global_mouseup(std::string_view) {
            if(!grabbed_window)
                return;
            web::remove_class(grabbed_window->id, "grabbed");
            grabbed_window = nullptr;
        }
        static void global_mousemove(std::string_view j) {
            if(!grabbed_window)
                return;
            nlohmann::json json = nlohmann::json::parse(j);
            int y = static_cast<int>(json["clientY"]) - grab_start_y;
            int x = static_cast<int>(json["clientX"]) - grab_start_x;
            grabbed_window->move(x, y);
        }
};
