import web;
import web_coro;

#include <coroutine>
#include <chrono>

#include <webxx.h>
#include <nlohmann/json.hpp>

constexpr char this_file[] = {
#embed __FILE__
};
constexpr std::string_view this_file_view{this_file, sizeof(this_file)};

constexpr char webxx_license[] = {
#embed "../build/_deps/webxx-src/LICENSE.md"
};
constexpr std::string_view webxx_license_view{webxx_license, sizeof(webxx_license)};

constexpr char json_license[] = {
#embed "../build/_deps/json-src/LICENSE.MIT"
};
constexpr std::string_view json_license_view{json_license, sizeof(json_license)};

#ifdef __clang__
constexpr auto cxx_compiler_name = "clang++";
constexpr auto cxx_compiler_version_major = __clang_major__;
constexpr auto cxx_compiler_version_minor = __clang_minor__;
constexpr auto cxx_compiler_version_patch = __clang_patchlevel__;
#else
constexpr auto cxx_compiler_name = "g++";
constexpr auto cxx_compiler_version_major = __GNUC__;
constexpr auto cxx_compiler_version_minor = __GNUC_MINOR__;
constexpr auto cxx_compiler_version_patch = __GNUC_PATCHLEVEL__;
#endif

struct WindowData {
    std::string id;
    int x;
    int y;
};

auto page() {
    using namespace Webxx;

    struct Window : component<Window> {
        Window(std::string_view id, std::string_view title) : component<Window>{
            dv { {_id{id}, _class{"window"}},
                dv { {_class{"titlebar"}},
                    h3 {title},
                    button { {_class{"minimize"}},"_"},
                    button { {_class{"maximize"}},"□"},
                    button { {_class{"close"}},"×"},
                },
                hr{},
                dv { {_class{"content"}},
                    "Hello World!",
                }
            }
        } {}
    };

    return fragment{
        h1{"Hello from JCM!"},
        button{{_id{"test"}}, "Click me!"},
        h2{{_id{"counter"}}, "Coroutine counter = 0"},
        Window{"source_code", "Source Code"},
        Window{"licenses", "Licenses"},
        details {
            summary{"A JS Event"},
            pre{{_id{"event_test"}}},
        },
        details{
            summary{a{{_href{"https://git.jcm.re/jcm/website"}, _target{"_blank"}}, "Source Code"}},
            pre{this_file_view},
        },
        details{{_class{"licenses"}},
            summary{"Licenses"},
            ul{
                li{details{
                    summary{a{{_href{"https://github.com/rthrfrd/webxx"}, _target{"_blank"}}, code{"webxx"}}},
                    pre{webxx_license_view},
                }},
                li{details{
                    summary{a{{_href{"https://github.com/nlohmann/json"}, _target{"_blank"}}, code{"nlohmann::json"}}},
                    pre{json_license_view},
                }},
            },
        },
        p{
            std::format(
                "Built on {} at {} with {} version {}.{}.{}.",
                __DATE__, __TIME__, cxx_compiler_name,
                cxx_compiler_version_major, cxx_compiler_version_minor, cxx_compiler_version_patch
            )
        },
    };
}
void move_window(std::string_view id, int x, int y) {
    web::set_property(id, "style", "left: {}px; top: {}px;", x, y);
}


int grab_start_x = 0;
int grab_start_y = 0;
std::string grabbed_window = "";

[[clang::export_name("main")]]
int main() {
    web::log("Hello World!");
    web::set_html("main", Webxx::render(page()));
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

    for(const auto& window : {"source_code", "licenses"}){
        web::add_event_listener(window, "mousedown", [w = std::string{window}](std::string_view j) {
            nlohmann::json json = nlohmann::json::parse(j);
            int offsetLeft = web::get_property_int(w, "offsetLeft");
            int offsetTop = web::get_property_int(w, "offsetTop");
            grab_start_x = static_cast<int>(json["clientX"]) - offsetLeft;
            grab_start_y = static_cast<int>(json["clientY"]) - offsetTop;
            grabbed_window = w;
        });
    }

    web::add_event_listener("main", "mouseup", [](std::string_view) {
        grabbed_window = "";
    });
    web::add_event_listener("main", "mousemove", [](std::string_view j) {
        if(grabbed_window.empty())
            return;
        nlohmann::json json = nlohmann::json::parse(j);
        int y = static_cast<int>(json["clientY"]) - grab_start_y;
        int x = static_cast<int>(json["clientX"]) - grab_start_x;
        move_window(grabbed_window, x, y);
    });

    move_window("source_code", 300, 100);
    move_window("licenses", 700, 300);

    using namespace web::coro;
    submit([]()->coroutine<void> {
        co_await event{"test", "click"};
        for(int i=0; i<100; i++) {
            web::set_html("counter", "Coroutine counter = {}", i);
            co_await timeout(std::chrono::milliseconds(100));
        }
        co_return;
    }());

    return 0;
}
