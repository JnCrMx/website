import std;
import webpp;
import webxx;
import tinyxml2;

import utils;
import components;
import c_interpreter;

namespace Webxx {
    constexpr static char onClickAttr[] = "onclick";
    using _onClick = attr<onClickAttr>;

    constexpr static char dataWindowIdAttr[] = "data-window-id";
    using _dataWindowId = attr<dataWindowIdAttr>;
}

namespace files {
    constexpr char src_main[] = { // secrets in this file will be optimized away :D
        #embed "src/main.cpp"
    };
    constexpr char webxx_license[] = {
        #embed "build/_deps/webxx-src/LICENSE.md"
    };
    constexpr char tinyxml2_license[] = {
        #embed "build/_deps/tinyxml2-src/LICENSE.txt"
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
        constexpr std::string_view tinyxml2_license{::files::tinyxml2_license, sizeof(::files::tinyxml2_license)};

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

    Window about_me{"about_me", "About Me", "images/about_me.png", [](){
        return fragment{
            h1{"JCM"},
            p{"I'm a software developer and computer engineering student."},
            p{{_class{"socials"}},
                a{{_href{"https://github.com/JnCrMx/"}, _target{"_blank"}},
                    img{{_src{"https://github.githubassets.com/assets/GitHub-Mark-ea2971cee799.png"}, _title{"GitHub"}}}},
                a{{_href{"https://git.jcm.re/jcm/"}, _target{"_blank"}},
                    img{{_src{"https://git.jcm.re/assets/img/logo.svg"}, _title{"Forgejo instance"}}}},
                a{{_href{"https://bsky.app/profile/jcm.re"}, _target{"_blank"}},
                    img{{_src{"https://web-cdn.bsky.app/static/apple-touch-icon.png"}, _title{"Bluesky"}}}},
                a{{_href{"https://wafrn.jcm.re/blog/jcm"}, _target{"_blank"}},
                    img{{_src{"https://wafrn.jcm.re/assets/icons/icon-512x512.png"}, _title{"Wafrn"}}}},
                a{{_href{"https://blog.jcm.re/"}, _target{"_blank"}},
                    img{{_src{"https://files.jcm.re/blog_icon.png"}, _title{"Blog"}}}},
                a{{_href{"https://keys.openpgp.org/vks/v1/by-fingerprint/7B839F0DA64069DD7832BB802F536DE3E9EC3AD8"}, _target{"_blank"}},
                    img{{_src{"https://www.openpgp.org/images/apple-touch-icon.png"}, _title{"OpenPGP"}}}},
            },
            p{"P.S.: I love Cyndi~! 🩷🩵"},
        };
    }};
    Window projects{"projects", "Projects", "images/projects.png", [](){
        enum class LinkType {
            GitHub,
            GitHug,
            Docs,
            CI,
            Snapcraft,
            OpenStore,
        };
        static constexpr std::array link_type_names = {
            "GitHub",
            "GitHug",
            "Documentation",
            "CI (Woodpecker)",
            "Snapcraft",
            "OpenStore",
        };
        static constexpr std::array link_type_icons = {
            "https://github.githubassets.com/assets/GitHub-Mark-ea2971cee799.png",
            "https://git.jcm.re/assets/img/logo.svg",
            "https://upload.wikimedia.org/wikipedia/commons/2/22/Document_%2889366%29_-_The_Noun_Project.svg",
            "https://woodpecker.jcm.re/favicons/favicon-light-default.svg",
            "https://snapcraft.io/static/snapcraft-favicon.png",
            "https://open-store.io/logo.png",
        };

        struct ProjectLink : public component<ProjectLink> {
            ProjectLink(LinkType type, const std::string& url) :
                component<ProjectLink>{
                    a{{_href{url}, _target{"_blank"}, _title{link_type_names[std::to_underlying(type)]}},
                        img{{_src{link_type_icons[std::to_underlying(type)]}, _alt{link_type_names[std::to_underlying(type)]}}}
                    }
                } {}
        };

        static const std::map<std::string_view, unsigned int> tag_color_map = {
            {"Java", 0xf8981d},
            {"Discord", 0x5865f2},
            {"C", 0x008add},
            {"C++", 0x00599c},
            {"WASM", 0x624de8},
            {"Vulkan", 0xad2729},
            {"OpenTelemetry", 0xf5a800},
            {"Ubuntu Touch", 0xf6561e},
            {"QML", 0x2ed03c},
            {"Waydroid", 0x00d48c},
        };
        struct ProjectTag : public component<ProjectTag> {
            ProjectTag(std::string_view tag) :
                component<ProjectTag>{
                    a{{_style{std::format("background-color: #{:06X};", tag_color_map.contains(tag) ? tag_color_map.at(tag) : (std::hash<std::string_view>{}(tag) & 0x00FFFFFF))}},tag}
                } {}
        };

        struct Project : public component<Project> {
            Project(const std::string& name, const std::string& url, std::list<ProjectLink>&& links, std::list<std::string_view>&& tags, const std::string& description) :
                Project(name, url, std::move(links), std::move(tags), fragment{description}) {}

            Project(const std::string& name, const std::string& url, std::list<ProjectLink>&& links, std::list<std::string_view>&& tags, fragment content) :
                component<Project> {
                    li{
                        p{
                            h4{{_class{"project-header"}},
                                a{{_href{url}, _target{"_blank"}}, name},
                                dv{{_class{"project-links"}}, each<ProjectLink>(std::move(links))},
                                dv{{_class{"project-tags"}}, each<ProjectTag>(std::move(tags))},
                            },
                            content
                        }
                    }
                } {}
        };

        return fragment{
            p{
                "I have plenty of public projects!<br>",
                "Some of them are hosted on ", a{{_href{"https://github.com/JnCrMx/"}, _target{"_blank"}}, "GitHub"},
                " and some on my own self-hosted ", a{{_href{"https://git.jcm.re/jcm/"}, _target{"_blank"}}, "Forgejo instance"},
                ".",
            },
            p{"Some of them are:"},
            ul{
                Project{"discord-game-sdk4j", "https://github.com/JnCrMx/discord-game-sdk4j",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/discord-game-sdk4j"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/discord-game-sdk4j"},
                     {LinkType::Docs, "https://docs.jcm.re/discord-game-sdk4j/"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/18"}},
                    {"Java", "Discord", "bindings", "library"},
                    "Java bindings for Discord's Game SDK.<br>"
                    "Now without using the native library and instead replicating the functionality entirely in pure Java."
                },
                Project{"xmbshell", "https://xmbshell.projects.jcm.re/",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/xmbshell"},
                     {LinkType::GitHug, "https://git.jcm.re/XMB-OS/xmbshell"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/16"},
                     {LinkType::Snapcraft, "https://snapcraft.io/xmbshell"}},
                    {"C++", "Vulkan", "desktop shell"},
                    fragment{
                        "A desktop shell mimicing the look and functionality of the XrossMediaBar.<br>",
                        "It is written in C++ and uses Vulkan (with a ",
                        a{{_href{"https://github.com/JnCrMx/dreamrender"}, _target{"_blank"}}, "self-written mini-framework"},
                        ") for rendering and ",
                        code{"glibmm"},"&amp;",code{"giomm"}," for everything else."
                }},
                Project{"cutie-logs", "https://github.com/JnCrMx/cutie-logs",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/cutie-logs"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/cutie-logs"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/29"}},
                    {"C++", "WASM", "OpenTelemetry", "full stack"},
                    fragment{
                        "A simple and lightweight OpenTelemetry log server combining collection, storage and analysis.<br>",
                        b{"Features:"}, ul{{_class{"columns-3"}},
                            li{"PostgreSQL database"},
                            li{"displaying and downloading logs as text"},
                            li{"displaying logs and attributes in a table"},
                            li{"powerful stencils to define log formats"},
                            li{"stencil functions for transforming attributes"},
                            li{"integrated IP lookup on frontend and backend"},
                            li{"multiple profiles for different applications"},
                            li{{_class{"gray"}}, "planned: rules for automatic cleanup"},
                            li{{_class{"gray"}}, "planned: rules for server-side custom attributes"},
                            li{{_class{"gray"}}, "planned: rules for server-side redaction"},
                        },
                        i{"Fun Fact: This application is &quot;full-stack&quot; C++, as it is used for both backend and frontend."},
                    }
                },
                Project{"fpga-dvi-hdmi-input", "https://github.com/JnCrMx/fpga-dvi-hdmi-input",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/website"}},
                    {"FPGA", "SystemVerilog", "C", "SoC", "HDMI", "DVI", "video"},
                    fragment{
                        "DVI/HDMI input for the Artix-7 FPGA with TileLink interface.<br>",
                        b{"Features:"}, ul{{_class{"columns-2"}},
                            li{"modular HDMI/DVI input written in System Verilog"},
                            li{"simple hardware access layer written in C"},
                            li{"dual framebuffers with locking and interrupts"},
                            li{"limited full HD resolution support"},
                            li{"configurable row and column skipping"},
                            li{"freely configurable EDID memory"},
                            li{"example applications showing frames in terminal"},
                        }
                    }
                },
                Project{"rsa-vhdl", "https://github.com/JnCrMx/rsa-vhdl",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/rsa-vhdl"}},
                    {"FPGA", "VHDL", "RSA", "crypto"},
                    "RSA and Montgomery multiplication modules written in VHDL."
                },
                Project{"website", "https://jcm.re",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/website"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/website"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/22"}},
                    {"C++", "WASM"},
                    fragment{
                        b{"This very website you are visiting right now!"}, "<br>"
                        "It is mostly written in C++ 26 using WASM and the ", code{"webxx"}, " library. Look at the \"Source Code\" window for more information."
                }},
                Project{"aom-sd-controller-fix", "https://github.com/JnCrMx/aom-sd-controller-fix",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/aom-sd-controller-fix"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/aom-sd-controller-fix"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/33"}},
                    {"C++", "patch", "video games", "modding"},
                    fragment{
                        "A patch for ", a{{_href{"https://store.steampowered.com/app/304530/Agents_of_Mayhem/"}, _target{"_blank"}}, "Agents of Mayhem"}, ", that prevents a crash on Steam Deck with Steam Input "
                        "and therefore allows you to play the game with proper controller inputs.<br>"
                        "In the future, I will try to add more fixes and features to this!"
                    }
                },
                Project{"ubuntu-touch-waydroid-files", "https://github.com/JnCrMx/ubuntu-touch-waydroid-files",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/ubuntu-touch-waydroid-files"},
                     {LinkType::OpenStore, "https://next.open-store.io/app/waydroid-files.jcm/"},
                     {LinkType::Snapcraft, "https://snapcraft.io/adb-file-explorer"}},
                    {"C++", "QML", "Ubuntu Touch", "ADB", "Waydroid"},
                    "An app to easily access files in a Waydroid container on Ubuntu Touch."
                },
                Project{"cpp-toy-os", "https://github.com/JnCrMx/cpp-toy-os",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/cpp-toy-os"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/cutie-os"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/27"}},
                    {"C++", "low level", "operating system", "kernel"},
                    "An operating system kernel for ARM written in modern C++, using coroutines.<br>"
                    "It is mostly a playground to try out how well certain modern features work in a freestanding environment (the answer is: pretty well)."
                },
                Project{"cheeky-imp", "https://git.jcm.re/cheeky-imp",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/cheeky-imp"},
                     {LinkType::GitHug, "https://git.jcm.re/cheeky-imp/cheeky-imp"},
                     {LinkType::Docs, "https://caddy.admin.jcm.re/docs/cheeky-imp/cheeky-imp/main/vulkan_layer/"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/17"}},
                    {"C++", "Vulkan", "video games", "modding"},
                    fragment{
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
                Project{"chocobotpp", "https://github.com/JnCrMx/chocobotpp",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/chocobotpp"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/chocobotpp"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/3"}},
                    {"C++", "Discord"},
                    fragment{
                        "A general purpose Discord bot written in C++ using the ", a{{_href{"https://dpp.dev/"}, _target{"_blank"}}, "D++ library"},
                        ", containing fun commands and mini-games.<br>",
                        b{"Features:"}, ul{{_class{"columns-3"}},
                            li{"simple economy system"},
                            li{"reminders"},
                            li{"some minigames"},
                            li{"some fun commands"},
                            li{"Christmas presents"},
                            li{"a (WIP) web dashboard"},
                            li{"simple custom commands"},
                            li{"freely definable command aliases"},
                        },
                }},
                Project{"VulkanBot", "https://vulkanbot.projects.jcm.re/",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/VulkanBot"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/VulkanBot"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/19"}},
                    {"C++", "Vulkan", "Discord"},
                    "A Discord bot that can render models using custom Vulkan shaders and send the result as an image or video."
                },
                Project{"webpp", "https://github.com/JnCrMx/webpp",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/webpp"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/webpp"}},
                    {"C++", "WASM", "library"},
                    "A simple C++ library for interacting with the DOM and other browser APIs.<br>"
                    "It is used in this website for handling events, creating elements, making fetch requests and much more."
                },
                Project{"cpp-snippets", "https://github.com/JnCrMx/cpp-snippets",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/cpp-snippets"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/cpp-snippets"}},
                    {"C++"},
                    "A collection of useful C++ snippets."
                },
                Project{"gpgfs", "https://github.com/JnCrMx/gpgfs",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/gpgfs"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/gpgfs"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/21"}},
                    {"C++", "FUSE", "crypto"},
                    "A FUSE filesystem that decrypts files on the access using GnuPG.<br>"
                    "I mainly use it for keeping credential files encrypted at rest."
                },
                Project{"dreamrender", "https://github.com/JnCrMx/dreamrender",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/dreamrender"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/dreamrender"}},
                    {"C++", "Vulkan", "library"},
                    "A simple Vulkan-based modern C++ render library used in some of my projects."
                },
            },
        };
    }};
    Window blog{"blog", "Blog", "images/blog.png", [](){
        return fragment{
            dv{{_id{"blog_container"}}}
        };
    }};
    Window source_code{"source_code", "Source Code", "images/source_code.png", [](){
        return fragment{
            p{
                "This website is mostly written in C++ 26 using WASM and the ", code{"webxx"}, " library.<br>",
                "It is compiled with ", code{"clang++"}, " and ", code{"lld"}, " and built with ", code{"CMake"}, ".<br>",
                "To provide a (more or less complete) standard library, ", code{"libc++-wasm32"} , " is used.<br>",
                "It is using C++ named modules and coroutines just for fun and to test how well these modern features work already ", i{"(quite well!)"}, ".<br>",
                "Interaction with the DOM and other browser APIs is done with self-made bindings (the ", code{"webpp"}, " library).<br>",
            },
            p{"You can find the source code of the main file (in which I am typing this text right now) here:"},
            details{
                summary{a{{_href{std::format("https://git.jcm.re/jcm/website/src/commit/{}/src/main.cpp", files::views::git_commit_hash)}, _target{"_blank"}}, "src/main.cpp"}},
                pre{src_main_sanitised},
            },
        };
    }};
    Window licenses{"licenses", "Licenses", "images/licenses.png", [](){
        return fragment{
            ul{ {_class{"licenses"}},
                li{details{
                    summary{a{{_href{"https://github.com/rthrfrd/webxx"}, _target{"_blank"}}, code{"webxx"}}},
                    pre{files::views::webxx_license},
                }},
                li{details{
                    summary{a{{_href{"https://github.com/leethomason/tinyxml2"}, _target{"_blank"}}, code{"TinyXML-2"}}},
                    pre{files::views::tinyxml2_license},
                }},
            },
        };
    }};
    Window build_info{"build_info", "Build Info", "images/build_info.png", [](){
        return fragment{
            "Build from commit ",
            a{{_href{std::format("https://git.jcm.re/jcm/website/commit/{}", files::views::git_commit_hash)}, _target{"blank"}},
              code{files::views::git_short_commit_hash}},
            std::format(
                " on {} at {} with {} version {}.{}.{}.",
                __DATE__, __TIME__, utils::cxx_compiler_name,
                utils::cxx_compiler_version_major, utils::cxx_compiler_version_minor, utils::cxx_compiler_version_patch
            )
        };
    }};
    Window c_interpreter{"c_interpreter", "C Interpreter", "images/c_interpreter.png", [](){
        return fragment{
            textarea{{_id{"c_interpreter_input"}, _placeholder{"Enter C code here..."}, _rows{"10"}, _cols{"80"}}},
            button{{_id{"c_interpreter_submit"}}, "Run"},
            textarea{{_id{"c_interpreter_output"}, _readonly{""}, _rows{"10"}, _cols{"80"}}},
        };
    }};
    Window cyndi{"cyndi", "Cyndi", "images/cyndi.png", [](){
        return fragment{
            h1{"I love you 🩷"},
            dv{{_id{"secret_content"}},
                input{{_id{"secret_password"}, _type{"password"}, _placeholder{"Password"}}},
                button{{_id{"secret_submit"}}, "💌"},
            },
        };
    }};
}

static std::array all_windows = {
    &windows::about_me,
    &windows::projects,
    &windows::blog,
    &windows::source_code,
    &windows::licenses,
    &windows::build_info,
    &windows::c_interpreter,
    &windows::cyndi,
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

    Window::setup();
    webpp::coro::submit([]() -> webpp::coroutine<void> {
        co_await webpp::coro::next_tick();
        windows::blog.open(400, 450);
        windows::about_me.open(75, 50);
        windows::projects.open(800, 100);
        windows::source_code.open(900, 500);
        windows::licenses.open(100, 550);
        windows::build_info.open(50, 800);
        //windows::c_interpreter.open(400, 100);

        std::string hash = webpp::eval("window.location.hash")["result"].as<std::string>().value_or("");
        bool cyndi = hash == "#cyndi";
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
                windows::blog.open();
                windows::about_me.open();
                windows::projects.open();
                windows::source_code.open();
                windows::licenses.open();
                windows::build_info.open();
                //windows::c_interpreter.open();

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

    using namespace webpp::coro;
    windows::blog.on_open += []() {
        submit([]()->coroutine<void> {
            std::string xml = co_await fetch("https://files.jcm.re/blog.xml").then(std::mem_fn(&webpp::response::co_text));
            tinyxml2::XMLDocument doc;
            doc.Parse(xml.data(), xml.size());
            tinyxml2::XMLElement* root = doc.FirstChildElement("rss");
            tinyxml2::XMLElement* channel = root->FirstChildElement("channel");
            for(tinyxml2::XMLElement* item = channel->FirstChildElement("item"); item; item = item->NextSiblingElement("item")) {
                std::string title = item->FirstChildElement("title")->GetText();
                std::string link = item->FirstChildElement("link")->GetText();
                std::string pubDate = item->FirstChildElement("pubDate")->GetText();
                std::string description = item->FirstChildElement("description")->GetText();
                using namespace Webxx;
                auto element = webpp::create_element_from_html(render(dv{
                    h2{a{{_href{link}, _target{"_blank"}}, title}},
                    p{i{pubDate}},
                    p{description},
                    hr{},
                }));
                webpp::get_element_by_id("blog_container")->append_child(*element);
            }
            co_return;
        }());
    };
    windows::cyndi.on_open += []() {
        submit([]()->coroutine<void> {
            co_await webpp::get_element_by_id("secret_submit")->event("click");
            std::string password = webpp::get_element_by_id("secret_password")->get_property<std::string>("value").value_or("");
            std::string res = co_await fetch("https://files.jcm.re/website_secret/"+password).then(std::mem_fn(&webpp::response::co_text));
            webpp::get_element_by_id("secret_content")->inner_html(res);
            co_return;
        }());
    };
    windows::c_interpreter.on_open += []() {
        webpp::get_element_by_id("c_interpreter_submit")->add_event_listener("click", [](webpp::event){
            std::string code = webpp::get_element_by_id("c_interpreter_input")->get_property<std::string>("value").value_or("");
            std::ostringstream output;

            {
                std::istringstream input{code};
                c_interpreter::lexer<1> lex{input};

                while(true) {
                    auto t = lex.next();
                    if(!t) {
                        output << "\nLexer error: " << t.error();
                        break;
                    }
                    output << utils::enum_name(t->kind);
                    if(t->kind == c_interpreter::token_kind::EOF_)
                        break;
                    std::visit([&output](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr(std::is_same_v<T, std::string>) {
                            output << "[" << std::quoted(arg) << "]";
                        } else if constexpr(!std::is_same_v<T, std::monostate>) {
                            output << "[" << arg << "]";
                        }
                    }, t->value);
                    output << " ";
                }
            }
            output << "\n";
            {
                std::istringstream input{code};
                c_interpreter::lexer<1> lex{input};
                c_interpreter::parser parser{lex};
                auto res = parser.parse_program();
                if(!res) {
                    output << "Parser error: " << res.error() << "\n";
                }
            }
            webpp::get_element_by_id("c_interpreter_output")->set_property("value", output.str());
        });
    };
    webpp::coro::submit(ganyu());

    return 0;
}
