import std;
import webpp;
import webxx;
import components;
import windows;

namespace Webxx {
    constexpr static char onClickAttr[] = "onclick";
    using _onClick = attr<onClickAttr>;

    constexpr static char dataWindowIdAttr[] = "data-window-id";
    using _dataWindowId = attr<dataWindowIdAttr>;
}

static std::array all_windows = {
    &windows::about_me,
    &windows::projects,
    &windows::blog,
    &windows::calendar,
    //&windows::recommendations,
    &windows::source_code,
    &windows::licenses,
    &windows::build_info,
    //&windows::c_interpreter,
    &windows::cyndi,
    &windows::feedback,
};

auto render_dock() {
    using js_handle = std::decay_t<decltype(std::declval<webpp::event>().handle())>;
    static webpp::callback_data on_click{[](js_handle handle, std::string_view){
        webpp::event event{handle};

        auto id = event["target"]["dataset"]["windowId"].as<std::string>();
        if(!id || id->empty()) {
            return;
        }
        webpp::log("Clicked on window: {}", *id);

        Window* w = nullptr;
        for(auto& win : all_windows) {
            if(win->id == *id) {
                w = win;
                break;
            }
        }
        if(!w) {
            return;
        }

        w->toggle_minimize();
        w->bring_to_front();
    }, false};

    using namespace Webxx;
    return fragment{
        dv{{_id{"dock"}},
            each(all_windows, [](Window* w) {
                if(!w->is_open()) {
                    return fragment{};
                }

                return fragment{button{{
                        _dataWindowId{w->id},
                        _onClick{std::format("handleEvent(this, event, {});", reinterpret_cast<std::uintptr_t>(&on_click))},
                        _style{std::format("background-image: url('{}');", w->get_icon())}
                    },
                    dv{{_class{"tooltip"}}, w->get_title()}
                }};
            })
        }
    };
}

auto page() {
    using namespace Webxx;
    return fragment{
        dv{{_id{"dock_container"}}, render_dock()},
        h1{"Hello from JCM!"},
        dv{{_id{"close_message"}}},
    };
}

static std::default_random_engine gen{std::random_device{}()};

auto ganyu() -> webpp::coroutine<void> {
    using namespace Webxx;
    static std::uniform_real_distribution<float> chance_dist{};
    constexpr float ganyu_chance = 0.1f;
    constexpr auto ganyu_duration = std::chrono::seconds{60};

    while(true) {
        co_await webpp::coro::timeout(std::chrono::seconds{1});
        if(chance_dist(gen) > ganyu_chance) {
            continue;
        }

        std::vector<Window*> open_windows;
        for(auto& w : all_windows) {
            if(w->is_open()) {
                open_windows.push_back(w);
            }
        }
        if(open_windows.empty()) {
            continue;
        }
        bool is_one_maximized = std::ranges::any_of(all_windows, [](Window* w) {
            return w->get_state() == Window::window_state::maximized;
        });
        if(is_one_maximized) {
            continue;
        }

        std::uniform_int_distribution<int> window_dist(0, open_windows.size()-1);
        auto target_window = open_windows[window_dist(gen)];
        auto target_element = *webpp::get_element_by_id(target_window->id);

        const int height = 50;
        const int offset_x = 0;
        const int offset_y = -height;
        int z_index = target_element.style()["zIndex"].as<int>().value_or(0);

        auto ganyu_elem = a{
            {
                _id{"ganyu"},
                _href{"https://git.jcm.re/jcm/"},
                _target{"_blank"},
                _style{std::format("left: {}px; top: {}px; z-index: {};",
                    target_window->get_x()+offset_x, target_window->get_y()+offset_y, z_index)}
            },
            img{{_src{"ganyu.png"}, _alt{"Ganyu"}, _height{std::to_string(height)}}}
        };
        auto ganyu_element = *webpp::create_element_from_html(Webxx::render(ganyu_elem));
        webpp::get_element_by_id("main")->append_child(ganyu_element);

        auto ref1 = target_window->on_move.add_unique([&ganyu_element, offset_x, offset_y](int x, int y) {
            auto style = ganyu_element.style();
            style["left"] = std::format("{}px", x+offset_x);
            style["top"] = std::format("{}px", y+offset_y);
        });
        auto ref2 = target_window->on_focus.add_unique([&ganyu_element, &target_element]() {
            int z_index = target_element.style()["zIndex"].as<int>().value_or(0);
            ganyu_element.style()["zIndex"] = z_index;
        });
        auto ref3 = target_window->on_maximize.add_unique([&ganyu_element](bool maximized) {
            ganyu_element.style()["display"] = maximized ? "none" : "unset";
        });
        bool window_closed = false;
        auto ref4 = target_window->on_close.add_unique([&window_closed](){
            window_closed = true;
        });

        constexpr auto tick = std::chrono::milliseconds{100};
        for(unsigned int i=0; i<ganyu_duration/tick && !window_closed; i++) {
            co_await webpp::coro::timeout(std::chrono::milliseconds{100});
        }

        ganyu_element.remove();
    }

    co_return;
}

[[clang::export_name("main")]]
int my_main() {
    webpp::log("Hello World!");
    webpp::get_element_by_id("main")->inner_html(Webxx::render(page()));

    static std::string hash = webpp::eval("window.location.hash")["result"].as<std::string>().value_or("");
    static bool cyndi = hash == "#cyndi";

    Window::setup();
    webpp::coro::submit([]() -> webpp::coroutine<void> {
        co_await webpp::coro::next_tick();

        //windows::recommendations.open(120, 400);
        windows::blog.open(400, 450);
        windows::calendar.open(600, 150);
        windows::about_me.open(75, 50);
        windows::projects.open(800, 100);
        windows::source_code.open(900, 500);
        windows::licenses.open(100, 450);
        windows::build_info.open(900, 850);
        //windows::c_interpreter.open(400, 100);
        windows::feedback.open(100, 650);

        if(cyndi) {
            windows::cyndi.open(500, 250);
        } else if(hash.size() > 1) {
            auto fullscreen_window_id = std::string_view{hash}.substr(1); // remove '#' from the beginning
            for(auto& w : all_windows) {
                if(w->id == fullscreen_window_id) {
                    w->maximize();
                    w->bring_to_front();
                    break;
                }
            }
        }

        webpp::get_element_by_id("dock_container")->inner_html(Webxx::render(render_dock()));

        co_return;
    }());

    auto close_handler = []() {
        webpp::get_element_by_id("dock_container")->inner_html(Webxx::render(render_dock()));

        bool all_closed = true;
        for(auto& w : all_windows) {
            if(w->is_open()) {
                all_closed = false;
                break;
            }
        }

        if(all_closed) {
            using namespace Webxx;
            webpp::get_element_by_id("close_message")->inner_html(render(fragment{
                h2{"You closed all windows!"},
                button{{_id{"reopen_button"}}, "Reopen them all~!"}
            }));
            using namespace webpp::coro;
            submit([]() -> coroutine<void> {
                co_await next_tick();

                auto reopen_button = *webpp::get_element_by_id("reopen_button");
                co_await reopen_button.event("click");

                constexpr std::array messages = {
                    "Are you sure? ;)",
                    "Why did you close them all before then?",
                    "Baka baka b-baaakaaa!",
                };
                static std::uniform_int_distribution<int> dist{0, messages.size()-1};
                int last = -1;
                for(int i=0; i<5-1; i++) {
                    int r;
                    do {
                        r = dist(gen);
                    } while(r == last);
                    last = r;

                    reopen_button.inner_text(messages[r]);

                    co_await next_tick();
                    co_await reopen_button.event("click");
                }

                webpp::get_element_by_id("close_message")->inner_html("");
                for(auto* window : all_windows) {
                    if(window == &windows::cyndi && !cyndi) {
                        continue;
                    }
                    window->open();
                }

                webpp::get_element_by_id("dock_container")->inner_html(Webxx::render(render_dock()));
                co_return;
            }());
        }
    };
    auto state_change_handler = []() {
        bool is_one_maximized = std::ranges::any_of(all_windows, [](Window* w) {
            return w->get_state() == Window::window_state::maximized;
        });
        if(is_one_maximized) {
            webpp::get_element_by_id("dock_container")->style()["display"] = "none";
        } else {
            webpp::get_element_by_id("dock_container")->style()["display"] = "unset";
        }
    };
    for(auto& w : all_windows) {
        w->on_close += close_handler;
        w->on_state_change += state_change_handler;
    }

    webpp::coro::submit(ganyu());

    return 0;
}
