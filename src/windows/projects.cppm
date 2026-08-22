export module windows:projects;

import std;
import webxx;
import components;

namespace windows {
    using namespace Webxx;

    export Window projects{"projects", "Projects", "images/projects.png", [](){
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
            "images/third_party/github.png",
            "images/third_party/githug.svg",
            "images/third_party/documentation.svg",
            "images/third_party/woodpecker.svg",
            "images/third_party/snapcraft.png",
            "images/third_party/open-store.png",
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
}
