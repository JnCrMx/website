export module components;

import std;
import webpp;
import webxx;

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
        enum class window_state {
            normal,
            minimized,
            maximized,
            closed
        };

        constexpr Window(std::string&& id, std::string&& title, std::string&& icon, std::add_pointer_t<Webxx::fragment()> content) :
            id(id), m_title(title), m_icon(icon), m_content(content) {}
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
            webpp::get_element_by_id(id+"__titlebar")->add_event_listener("mousedown", [this](webpp::event event) {
                if(event["button"].as<int>() != 0)
                    return;
                event.prevent_default();

                auto window = *webpp::get_element_by_id(id);
                int offsetLeft = *window["offsetLeft"].as<int>();
                int offsetTop = *window["offsetTop"].as<int>();
                grab_start_x = *event["clientX"].as<int>() - offsetLeft;
                grab_start_y = *event["clientY"].as<int>() - offsetTop;
                grabbed_window = this;
                window.add_class("grabbed");
            });
            webpp::get_element_by_id(id)->add_event_listener("mousedown", [this](webpp::event event) {
                bring_to_front();
            });
            webpp::get_element_by_id(id+"__minimize")->add_event_listener("click", [this](webpp::event) {
                minimize();
            });
            webpp::get_element_by_id(id+"__maximize")->add_event_listener("click", [this](webpp::event) {
                toggle_maximize();
            });
            webpp::get_element_by_id(id+"__close")->add_event_listener("click", [this](webpp::event) {
                close();
            });
            move(initial_x, initial_y);
        }
        void bring_to_front() {
            (*webpp::get_element_by_id(id)).style()["zIndex"] = highest_z_index++;
            if(on_focus) { on_focus(); }
        }
        void minimize() {
            webpp::get_element_by_id(id)->add_class("minimized");
            is_minimized = true;

            if(on_minimize) { on_minimize(true); }
            if(on_state_change) { on_state_change(); }
        }
        void toggle_minimize() {
            auto e = *webpp::get_element_by_id(id);
            e.toggle_class("minimized");
            is_minimized = !is_minimized;

            if(on_minimize) { on_minimize(is_minimized); }
            if(on_state_change) { on_state_change(); }
        }
        void maximize() {
            webpp::get_element_by_id(id)->add_class("maximized");
            is_maximized = true;

            if(on_maximize) { on_maximize(true); }
            if(on_state_change) { on_state_change(); }
        }
        void toggle_maximize() {
            auto e = *webpp::get_element_by_id(id);
            e.toggle_class("maximized");
            is_maximized = !is_maximized;

            if(on_maximize) { on_maximize(is_maximized); }
            if(on_state_change) { on_state_change(); }
        }
        void restore() {
            auto e = *webpp::get_element_by_id(id);
            e.remove_class("maximized");
            e.remove_class("minimized");
            is_maximized = false;
            is_minimized = false;

            if(on_restore) { on_restore(); }
            if(on_state_change) { on_state_change(); }
        }
        void move(int x, int y) {
            auto html_element = webpp::document.document_element();
            auto e = *webpp::get_element_by_id(id);

            x = std::clamp(x, html_element.client_left()+1, html_element.client_width() - e.offset_width()-1);
            y = std::clamp(y, html_element.client_top()+1, html_element.client_height() - e.offset_height()-1);

            e.style()["left"] = std::format("{}px", x);
            e.style()["top"] = std::format("{}px", y);
            m_x = x;
            m_y = y;
            if(on_move) { on_move(m_x, m_y); }
        }
        void close() {
            webpp::get_element_by_id(id)->remove();
            m_open = false;
            if(on_close) { on_close(); }
            if(on_state_change) { on_state_change(); }
        }

        void open() {
            return open(m_x, m_y);
        }

        void open(int initial_x, int initial_y) {
            std::string html = Webxx::render(render_window(initial_x, initial_y));
            auto el = webpp::create_element_from_html(html);
            webpp::get_element_by_id("main")->append_child(*el);

            setup(initial_x, initial_y);
            m_open = true;
            if(on_open) { on_open(); }
            if(on_state_change) { on_state_change(); }
        }

        const std::string& get_title() const {
            return m_title;
        }
        const std::string& get_icon() const {
            return m_icon;
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
        window_state get_state() const {
            if(!m_open)
                return window_state::closed;
            if(is_minimized)
                return window_state::minimized;
            if(is_maximized)
                return window_state::maximized;
            return window_state::normal;
        }

        ActionHandlerList<> on_open;
        ActionHandlerList<> on_close;
        ActionHandlerList<> on_focus;
        ActionHandlerList<bool> on_minimize;
        ActionHandlerList<bool> on_maximize;
        ActionHandlerList<> on_restore;
        ActionHandlerList<int, int> on_move;
        ActionHandlerList<> on_state_change;

        static void setup() {
            webpp::document.add_event_listener("mouseup", global_mouseup);
            webpp::document.add_event_listener("mousemove", global_mousemove);
        }

    private:
        std::string m_title;
        std::string m_icon;
        std::add_pointer_t<Webxx::fragment()> m_content;
        bool m_open = false;
        int m_x = 0;
        int m_y = 0;

        bool is_minimized = false;
        bool is_maximized = false;

        static inline int highest_z_index = 1;
        static inline int grab_start_x = 0;
        static inline int grab_start_y = 0;
        static inline struct Window* grabbed_window = nullptr;
        static void global_mouseup(webpp::event) {
            if(!grabbed_window)
                return;
            webpp::get_element_by_id(grabbed_window->id)->remove_class("grabbed");
            grabbed_window = nullptr;
        }
        static void global_mousemove(webpp::event e) {
            if(!grabbed_window)
                return;
            int y = *e["clientY"].as<int>() - grab_start_y;
            int x = *e["clientX"].as<int>() - grab_start_x;
            grabbed_window->move(x, y);
        }
};
