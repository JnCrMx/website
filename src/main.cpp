import web;
import web_coro;
import utils;
import nlohmann_json;
import webxx;

#include <array>
#include <coroutine> // IWYU pragma: keep
#include <format>
#include <string>
#include <string_view>

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

    constexpr char git_log[] = {
        #embed ".git/logs/HEAD"
    };

    namespace arrays {
        constexpr auto src_main = std::to_array(::files::src_main);

        constexpr auto git_log = std::to_array(::files::git_log);
        constexpr auto git_commit_hash = [](){
            constexpr std::string_view sv{::files::git_log, sizeof(::files::git_log)};
            constexpr auto pos1 = sv.find_last_of('\n');
            constexpr auto pos2 = sv.find_last_of('\n', pos1-1);
            constexpr auto pos3 = pos2 == std::string_view::npos ? 0 : pos2; // the file might have only one line
            constexpr auto pos4 = sv.find(' ', pos3);

            return utils::substr<git_log, pos4+1, 40>();
        }();
    }
    namespace views {
        constexpr std::string_view webxx_license{::files::webxx_license, sizeof(::files::webxx_license)};
        constexpr std::string_view json_license{::files::json_license, sizeof(::files::json_license)};

        constexpr std::string_view git_commit_hash{::files::arrays::git_commit_hash.data(), ::files::arrays::git_commit_hash.size()};
        constexpr std::string_view git_short_commit_hash = git_commit_hash.substr(0, 7);
    }
}

constexpr auto src_main_sanitised_array = [](){
    constexpr auto s0 = files::arrays::src_main;
    constexpr auto s1 = utils::replace_sub_str<s0, std::to_array("&"), std::to_array("&amp;")>();
    constexpr auto s2 = utils::replace_sub_str<s1, std::to_array("<"), std::to_array("&lt;")>();
    constexpr auto s3 = utils::replace_sub_str<s2, std::to_array(">"), std::to_array("&gt;")>();
    constexpr auto s4 = utils::replace_sub_str<s3, std::to_array("cyndi"), std::to_array("secret")>();
    constexpr auto s5 = utils::replace_sub_str<s4, std::to_array("Cyndi"), std::to_array("secret")>();
    return s5;
}();
constexpr std::string_view src_main_sanitised{src_main_sanitised_array.data(), src_main_sanitised_array.size()};

namespace windows {
    using namespace Webxx;
    struct Window : component<Window> {
        constexpr Window(std::string_view id, std::string_view title, fragment&& content) : component<Window>{
            dv { {_id{id}, _class{"window"}},
                dv { {_id{std::string{id}+"__titlebar"}, _class{"titlebar"}},
                    h3 {title},
                    button { {_id{std::string{id}+"__minimize"}, _class{"minimize"}},"_"},
                    button { {_id{std::string{id}+"__maximize"}, _class{"maximize"}},"□"},
                    button { {_id{std::string{id}+"__close"}, _class{"close"}},"×"},
                },
                hr{},
                dv { {_class{"content"}},
                    std::move(content)
                }
            }
        } {}
    };

    const Window about_me{"about_me", "About Me",
        fragment{
            h1{"JCM"},
            p{"I'm a software developer and computer engineering student."},
            dv{{_class{"socials"}},
                a{{_href{"https://github.com/JnCrMx/"}, _target{"_blank"}},
                    img{{_src{"https://github.githubassets.com/assets/GitHub-Mark-ea2971cee799.png"}, _title{"GitHub"}}}},
                a{{_href{"https://git.jcm.re/jcm/"}, _target{"_blank"}},
                    img{{_src{"https://git.jcm.re/assets/img/logo.svg"}, _title{"Forgejo instance"}}}},
                a{{_href{"https://blog.jcm.re/"}, _target{"_blank"}},
                    img{{_src{"https://files.jcm.re/blog_icon.png"}, _title{"Blog"}}}},
                a{{_href{"https://keys.openpgp.org/vks/v1/by-fingerprint/7B839F0DA64069DD7832BB802F536DE3E9EC3AD8"}, _target{"_blank"}},
                    img{{_src{"https://www.openpgp.org/images/apple-touch-icon.png"}, _title{"OpenPGP"}}}},
            }
        }
    };
    const Window projects{"projects", "Projects",
        fragment{
            p{
                "I have plenty of public projects!<br>",
                "Some of them are hosted on ", a{{_href{"https://github.com/JnCrMx/"}, _target{"_blank"}}, "GitHub"},
                " and some on my own self-hosted ", a{{_href{"https://git.jcm.re/jcm/"}, _target{"_blank"}}, "Forgejo instance"},
                ".",
            },
            p{"Some of them are:"},
            ul{
                li{p{
                    a{{_href{"https://github.com/JnCrMx/discord-game-sdk4j"}, _target{"_blank"}}, h4{"discord-game-sdk4j"}},
                    "Java bindings for Discord's Game SDK.<br>"
                    "Now without using the native library and instead replicating the functionality entirely in pure Java."
                }},
                li{p{
                    a{{_href{"https://github.com/JnCrMx/cpp-toy-os"}, _target{"_blank"}}, h4{"cpp-toy-os"}},
                    "An operating system kernel for ARM written in modern C++, using coroutines.<br>"
                    "It is mostly a playground to try out how well certain modern features work in a freestanding environment (the answer is: pretty well)."
                }},
                li{p{
                    a{{_href{"https://github.com/JnCrMx/VulkanBot"}, _target{"_blank"}}, h4{"VulkanBot"}},
                    "A Discord bot that can render models using custom Vulkan shaders and send the result as an image or video."
                }},
                li{p{
                    a{{_href{"https://github.com/JnCrMx/cheeky-imp"}, _target{"_blank"}}, h4{"cheeky-imp"}},
                    "A framework for hooking into Vulkan-based games for extracting and replacing shaders, texture and models.<br>"
                    "It makes use of a self-made rule-based language for adding custom behaviour and has multiple plugins, such as:",
                    ul{
                        li{a{{_href{"https://github.com/JnCrMx/cheeky-companion"}, _target{"_blank"}}, "cheeky-companion"}, " for rendering and controlling additional models"},
                        li{a{{_href{"https://git.jcm.re/cheeky-imp/cheeky-imgui"}, _target{"_blank"}}, "cheeky-imgui"}, " for easily adding an ImGui overlay"},
                        li{a{{_href{"https://git.jcm.re/cheeky-imp/cheeky-input-x11"}, _target{"_blank"}}, "cheeky-input-x11"}, " for capturing input from X11 based applications"},
                        li{a{{_href{"https://git.jcm.re/cheeky-imp/cheeky-datahook"}, _target{"_blank"}}, "cheeky-datahook"}, " for extracting data from shaders"},
                        li{a{{_href{"https://git.jcm.re/cheeky-imp/cheeky-dbus"}, _target{"_blank"}}, "cheeky-dbus"}, " for interacting with D-Bus services"},
                    }
                }},
                li{p{
                    a{{_href{"https://github.com/JnCrMx/gpgfs"}, _target{"_blank"}}, h4{"gpgfs"}},
                    "A FUSE filesystem that decrypts files on the access using GnuPG.<br>"
                    "I mainly use it for keeping credential files encrypted at rest."
                }},
                li{p{
                    a{{_href{"https://github.com/JnCrMx/chocobotpp"}, _target{"_blank"}}, h4{"chocobotpp"}},
                    "A general purpose Discord bot written in C++, containing fun commands and mini-games."
                }},
                li{p{
                    a{{_href{"https://github.com/JnCrMx/cpp-snippets"}, _target{"_blank"}}, h4{"cpp-snippets"}},
                    "A collection of useful C++ snippets."
                }},
                li{p{
                    a{{_href{"https://github.com/JnCrMx/dreamrender"}, _target{"_blank"}}, h4{"dreamrender"}},
                    "A simple Vulkan-based modern C++ render library used in some of my projects."
                }},
            },
        }
    };
    const Window source_code{"source_code", "Source Code",
        fragment{
            p{
                "This website is mostly written in C++ 23 using WASM and the ", code{"webxx"}, " library.<br>",
                "It is compiled with ", code{"clang++"}, " and ", code{"lld"}, " (version 20) and built with ", code{"CMake"}, ".<br>",
                "To provide a (more or less complete) standard library, ", code{"libc++-wasm32"} , " is used.<br>",
                "It is using C++ named modules and coroutines just for fun and to test how well these modern features work already ", i{"(quite well!)"}, ".<br>",
                "Interaction with the DOM and other browser APIs is done with self-made bindings (the ", code{"web"}, " and ", code{"web_coro"}, " modules).<br>",
            },
            p{"You can find the source code of the main file (in which I am typing this text right now) here:"},
            details{
                summary{a{{_href{"https://git.jcm.re/jcm/website/src/branch/main/src/main.cpp"}, _target{"_blank"}}, "src/main.cpp"}},
                pre{src_main_sanitised},
            },
        }
    };
    const Window licenses{"licenses", "Licenses",
        fragment{
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
        }
    };
    const Window build_info{"build_info", "Build Info",
        fragment{
            std::format(
                "Built from commit {} on {} at {} with {} version {}.{}.{}.",
                files::views::git_short_commit_hash, __DATE__, __TIME__, utils::cxx_compiler_name,
                utils::cxx_compiler_version_major, utils::cxx_compiler_version_minor, utils::cxx_compiler_version_patch
            )
        }
    };
    const Window cyndi{"cyndi", "Cyndi",
        fragment{
            h1{"I love you 🩷"},
            dv{{_id{"secret_content"}},
                input{{_id{"secret_password"}, _type{"password"}, _placeholder{"Password"}}},
                button{{_id{"secret_submit"}}, "💌"},
            },
        }
    };
}

auto page(bool cyndi) {
    using namespace Webxx;
    return fragment{
        h1{"Hello from JCM!"},
        windows::about_me,
        windows::projects,
        windows::source_code,
        windows::licenses,
        windows::build_info,
        maybe(cyndi, [](){return windows::cyndi;}),
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
        web::add_class(id, "grabbed");
    }, false, true);
    web::add_event_listener(id, "mousedown", [id](std::string_view) {
        web::set_style_property(id, "zIndex", "{}", highest_z_index++);
    });
    web::add_event_listener(id+"__minimize", "click", [id](std::string_view) {
        web::add_class(id, "minimized");
    });
    web::add_event_listener(id+"__maximize", "click", [id](std::string_view) {
        web::toggle_class(id, "maximized");
    });
    web::add_event_listener(id+"__close", "click", [id](std::string_view) {
        web::remove_element(id);
    });
    move_window(id, initial_x, initial_y);
}

[[clang::export_name("main")]]
int main() {
    web::log("Hello World!");

    std::string hash = web::eval("location.hash");
    bool cyndi = hash == "#cyndi";

    web::set_html("main", Webxx::render(page(cyndi)));

    setup_window("about_me", 75, 50);
    setup_window("projects", 800, 100);
    setup_window("source_code", 700, 500);
    setup_window("licenses", 100, 550);
    setup_window("build_info", 50, 800);
    if(cyndi) {
        setup_window("cyndi", 400, 300);
    }

    web::add_event_listener("__document__", "mouseup", [](std::string_view) {
        if(grabbed_window.empty())
            return;
        web::remove_class(grabbed_window, "grabbed");
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
    if(cyndi) {
        submit([]()->coroutine<void> {
            co_await event{"secret_submit", "click"};
            std::string password = web::get_property("secret_password", "value");
            std::string res = co_await fetch("https://files.jcm.re/website_secret/"+password);
            web::set_html("secret_content", res);
            co_return;
        }());
    }

    return 0;
}
