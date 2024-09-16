import web;
import web_coro;
import utils;
import nlohmann_json;
import webxx;

#include <coroutine>
#include <chrono>

namespace files {
    constexpr char src_main[] = { // secrets in this file will be optimized away :D
        #embed "src/main.cpp"
    };
    constexpr char webxx_license[] = {
        #embed "build/_deps/webxx-src/LICENSE.md"
    };
    constexpr char json_license[] = {
        #embed "build/_deps/json-src/LICENSE.MIT"
    };

    namespace views {
        constexpr std::string_view webxx_license{::files::webxx_license, sizeof(::files::webxx_license)};
        constexpr std::string_view json_license{::files::json_license, sizeof(::files::json_license)};
    }
    namespace arrays {
        constexpr auto src_main = std::to_array(::files::src_main);
    }
}

constexpr auto src_main_sanitised_array = [](){
    constexpr auto s0 = files::arrays::src_main;
    constexpr auto s1 = utils::replace_sub_str<s0, std::to_array("&"), std::to_array("&amp;")>();
    constexpr auto s2 = utils::replace_sub_str<s1, std::to_array("<"), std::to_array("&lt;")>();
    constexpr auto s3 = utils::replace_sub_str<s2, std::to_array(">"), std::to_array("&gt;")>();
    constexpr auto s4 = utils::replace_sub_str<s3, std::to_array("cyndi"), std::to_array("redacted")>();
    constexpr auto s5 = utils::replace_sub_str<s4, std::to_array("Cyndi"), std::to_array("redacted")>();
    return s5;
}();
constexpr std::string_view src_main_sanitised{src_main_sanitised_array.data(), src_main_sanitised_array.size()};

auto page(bool cyndi) {
    using namespace Webxx;

    struct Window : component<Window> {
        Window(std::string_view id, std::string_view title, fragment&& content) : component<Window>{
            dv { {_id{id}, _class{"window"}},
                dv { {_id{std::string{id}+"__titlebar"}, _class{"titlebar"}},
                    h3 {title},
                    button { {_class{"minimize"}},"_"},
                    button { {_class{"maximize"}},"□"},
                    button { {_class{"close"}},"×"},
                },
                hr{},
                dv { {_class{"content"}},
                    std::move(content)
                }
            }
        } {}
    };

    return fragment{
        h1{"Hello from JCM!"},
        button{{_id{"test"}}, "Click me!"},
        h2{{_id{"counter"}}, "Coroutine counter = 0"},
        Window{"source_code", "Source Code", fragment{
            a{{_href{"https://git.jcm.re/jcm/website"}, _target{"_blank"}, _style{"display: flex; align-items: center;"}}, img{{_src{"https://git.jcm.re/assets/img/logo.svg"}}}, b{"GitHug"}},
            details{
                summary{a{{_href{"https://git.jcm.re/jcm/website/src/branch/main/src/main.cpp"}, _target{"_blank"}}, "src/main.cpp"}},
                pre{src_main_sanitised},
            },
        }},
        Window{"licenses", "Licenses", fragment{
            ul{ {_class{"licenses"}},
                li{details{
                    summary{a{{_href{"https://github.com/rthrfrd/webxx"}, _target{"_blank"}}, code{"webxx"}}},
                    pre{files::views::webxx_license},
                }},
                li{details{
                    summary{a{{_href{"https://github.com/nlohmann/json"}, _target{"_blank"}}, code{"nlohmann::json"}}},
                    pre{files::views::json_license},
                }},
            },
        }},
        Window{"build_info", "Build Info", fragment{
            std::format(
                "Built on {} at {} with {} version {}.{}.{}.",
                __DATE__, __TIME__, utils::cxx_compiler_name,
                utils::cxx_compiler_version_major, utils::cxx_compiler_version_minor, utils::cxx_compiler_version_patch
            )
        }},
        maybe(cyndi, [](){
            return Window{"cyndi", "Cyndi", fragment{
                h1{"I love you 🩷"},
                dv{{_id{"cyndi_content"}},
                    input{{_id{"cyndi_password"}, _type{"password"}, _placeholder{"Password"}}},
                    button{{_id{"cyndi_submit"}}, "💌"},
                },
            }};
        }),
        details {
            summary{"A JS Event"},
            pre{{_id{"event_test"}}},
        },
        details {
            summary{"Some fetched data"},
            pre{{_id{"fetch_test"}}},
        },
    };
}
void move_window(std::string_view id, int x, int y) {
    web::set_style_property(id, "left", "{}px", x);
    web::set_style_property(id, "top", "{}px", y);
}

int highest_z_index = 1;
int grab_start_x = 0;
int grab_start_y = 0;
std::string grabbed_window = "";

void setup_window(std::string id, int initial_x, int initial_y) {
    web::add_event_listener(id+"__titlebar", "mousedown", [id](std::string_view j) {
        nlohmann::json json = nlohmann::json::parse(j);
        if(json["button"] != 0)
            return;
        int offsetLeft = web::get_property_int(id, "offsetLeft");
        int offsetTop = web::get_property_int(id, "offsetTop");
        grab_start_x = static_cast<int>(json["clientX"]) - offsetLeft;
        grab_start_y = static_cast<int>(json["clientY"]) - offsetTop;
        grabbed_window = id;
        web::set_property(id, "classList", "window grabbed");
    }, false, true);
    web::add_event_listener(id, "mousedown", [id](std::string_view) {
        web::set_style_property(id, "zIndex", "{}", highest_z_index++);
    });
    move_window(id, initial_x, initial_y);
}

[[clang::export_name("main")]]
int main() {
    web::log("Hello World!");

    std::string hash = web::eval("location.hash");
    bool cyndi = hash == "#cyndi";

    web::set_html("main", Webxx::render(page(cyndi)));
    web::add_event_listener("test", "click", [i=0](std::string_view json) mutable {
        web::set_html("test", "You clicked me {} times!", ++i);
        nlohmann::json j = nlohmann::json::parse(json);
        web::set_html("event_test", j.dump(4));
        if(i == 10) {
            web::set_timeout(std::chrono::milliseconds(1000), [](std::string_view){
                web::eval("alert('You clicked me 10 times!')");
            });
        }
    });

    setup_window("source_code", 300, 200);
    setup_window("licenses", 700, 300);
    setup_window("build_info", 20, 400);
    if(cyndi) {
        setup_window("cyndi", 600, 100);
    }

    web::add_event_listener("__document__", "mouseup", [](std::string_view) {
        if(grabbed_window.empty())
            return;
        web::set_property(grabbed_window, "classList", "window");
        grabbed_window = "";
    });
    web::add_event_listener("__document__", "mousemove", [](std::string_view j) {
        if(grabbed_window.empty())
            return;
        nlohmann::json json = nlohmann::json::parse(j);
        int y = static_cast<int>(json["clientY"]) - grab_start_y;
        int x = static_cast<int>(json["clientX"]) - grab_start_x;
        move_window(grabbed_window, x, y);
    });

    using namespace web::coro;
    submit([]()->coroutine<void> {
        co_await event{"test", "click"};
        std::string res = co_await fetch("https://files.jcm.re/status");
        web::set_html("fetch_test", "Response: {}", res);
        for(int i=0; i<100; i++) {
            web::set_html("counter", "Coroutine counter = {}", i);
            co_await timeout(std::chrono::milliseconds(100));
        }
        co_return;
    }());

    if(cyndi) {
        submit([]()->coroutine<void> {
            co_await event{"cyndi_submit", "click"};
            std::string password = web::get_property("cyndi_password", "value");
            std::string res = co_await fetch("https://files.jcm.re/cyndi/"+password);
            web::set_html("cyndi_content", res);
            co_return;
        }());
    }

    return 0;
}
