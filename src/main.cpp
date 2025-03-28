import std;
import web;
import web_coro;
import nlohmann_json;
import webxx;
import tinyxml2;

import utils;
import components;
import c_interpreter;

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
        constexpr std::string_view json_license{::files::json_license, sizeof(::files::json_license)};
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

    Window about_me{"about_me", "About Me", [](){
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
                a{{_href{"https://social.jcm.re/@jcm"}, _target{"_blank"}},
                    img{{_src{"https://joinmastodon.org/logos/logo-purple.svg"}, _title{"Mastodon"}}}},
                a{{_href{"https://blog.jcm.re/"}, _target{"_blank"}},
                    img{{_src{"https://files.jcm.re/blog_icon.png"}, _title{"Blog"}}}},
                a{{_href{"https://keys.openpgp.org/vks/v1/by-fingerprint/7B839F0DA64069DD7832BB802F536DE3E9EC3AD8"}, _target{"_blank"}},
                    img{{_src{"https://www.openpgp.org/images/apple-touch-icon.png"}, _title{"OpenPGP"}}}},
            },
            p{"P.S.: I love Cyndi~! 🩷🩵"},
        };
    }};
    Window projects{"projects", "Projects", [](){
        enum class LinkType {
            GitHub,
            GitHug,
            Docs,
            CI,
        };
        static constexpr std::array link_type_names = {
            "GitHub",
            "GitHug",
            "Documentation",
            "CI (Woodpecker)",
        };
        static constexpr std::array link_type_icons = {
            "https://github.githubassets.com/assets/GitHub-Mark-ea2971cee799.png",
            "https://git.jcm.re/assets/img/logo.svg",
            "https://upload.wikimedia.org/wikipedia/commons/2/22/Document_%2889366%29_-_The_Noun_Project.svg",
            "https://woodpecker.jcm.re/favicons/favicon-light-default.svg",
        };

        struct ProjectLink : public component<ProjectLink> {
            ProjectLink(LinkType type, const std::string& url) :
                component<ProjectLink>{
                    a{{_href{url}, _target{"_blank"}, _title{link_type_names[std::to_underlying(type)]}},
                        img{{_src{link_type_icons[std::to_underlying(type)]}, _alt{link_type_names[std::to_underlying(type)]}}}
                    }
                } {}
        };

        struct Project : public component<Project> {
            Project(const std::string& name, const std::string& url, std::list<ProjectLink>&& links, const std::string& description) :
                Project(name, url, std::move(links), fragment{description}) {}

            Project(const std::string& name, const std::string& url, std::list<ProjectLink>&& links, fragment content) :
                component<Project> {
                    li{
                        p{
                            h4{a{{_href{url}, _target{"_blank"}}, name},
                            dv{{_class{"project-links"}}, each<ProjectLink>(std::move(links))}},
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
                    "Java bindings for Discord's Game SDK.<br>"
                    "Now without using the native library and instead replicating the functionality entirely in pure Java."
                },
                Project{"xmbshell", "https://xmbshell.projects.jcm.re/",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/xmbshell"},
                     {LinkType::GitHug, "https://git.jcm.re/XMB-OS/xmbshell"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/16"}},
                    fragment{
                        "A desktop shell mimicing the look and functionality of the XrossMediaBar.<br>",
                        "It is written in C++ and uses Vulkan (with a ",
                        a{{_href{"https://github.com/JnCrMx/dreamrender"}, _target{"_blank"}}, "self-written mini-framework"},
                        ") for rendering and ",
                        code{"glibmm"},"&amp;",code{"giomm"}," for everything else."
                }},
                Project{"website", "https://jcm.re",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/website"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/website"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/22"}},
                    fragment{
                        b{"This very website you are visiting right now!"}, "<br>"
                        "It is mostly written in C++ 26 using WASM and the ", code{"webxx"}, " library. Look at the \"Source Code\" window for more information."
                }},
                Project{"cpp-toy-os", "https://github.com/JnCrMx/cpp-toy-os",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/cpp-toy-os"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/cutie-os"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/27"}},
                    "An operating system kernel for ARM written in modern C++, using coroutines.<br>"
                    "It is mostly a playground to try out how well certain modern features work in a freestanding environment (the answer is: pretty well)."
                },
                Project{"VulkanBot", "https://vulkanbot.projects.jcm.re/",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/VulkanBot"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/VulkanBot"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/19"}},
                    "A Discord bot that can render models using custom Vulkan shaders and send the result as an image or video."
                },
                Project{"cheeky-imp", "https://git.jcm.re/cheeky-imp",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/cheeky-imp"},
                     {LinkType::GitHug, "https://git.jcm.re/cheeky-imp/cheeky-imp"},
                     {LinkType::Docs, "https://caddy.admin.jcm.re/docs/cheeky-imp/cheeky-imp/main/vulkan_layer/"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/17"}},
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
                Project{"gpgfs", "https://github.com/JnCrMx/gpgfs",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/gpgfs"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/gpgfs"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/21"}},
                    "A FUSE filesystem that decrypts files on the access using GnuPG.<br>"
                    "I mainly use it for keeping credential files encrypted at rest."
                },
                Project{"chocobotpp", "https://github.com/JnCrMx/chocobotpp",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/chocobotpp"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/chocobotpp"},
                     {LinkType::CI, "https://woodpecker.jcm.re/repos/3"}},
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
                Project{"cpp-snippets", "https://github.com/JnCrMx/cpp-snippets",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/cpp-snippets"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/cpp-snippets"}},
                    "A collection of useful C++ snippets."
                },
                Project{"dreamrender", "https://github.com/JnCrMx/dreamrender",
                    {{LinkType::GitHub, "https://github.com/JnCrMx/dreamrender"},
                     {LinkType::GitHug, "https://git.jcm.re/jcm/dreamrender"}},
                    "A simple Vulkan-based modern C++ render library used in some of my projects."
                },
            },
        };
    }};
    Window blog{"blog", "Blog", [](){
        return fragment{
            dv{{_id{"blog_container"}}}
        };
    }};
    Window source_code{"source_code", "Source Code", [](){
        return fragment{
            p{
                "This website is mostly written in C++ 26 using WASM and the ", code{"webxx"}, " library.<br>",
                "It is compiled with ", code{"clang++"}, " and ", code{"lld"}, " and built with ", code{"CMake"}, ".<br>",
                "To provide a (more or less complete) standard library, ", code{"libc++-wasm32"} , " is used.<br>",
                "It is using C++ named modules and coroutines just for fun and to test how well these modern features work already ", i{"(quite well!)"}, ".<br>",
                "Interaction with the DOM and other browser APIs is done with self-made bindings (the ", code{"web"}, " and ", code{"web_coro"}, " modules).<br>",
            },
            p{"You can find the source code of the main file (in which I am typing this text right now) here:"},
            details{
                summary{a{{_href{std::format("https://git.jcm.re/jcm/website/src/commit/{}/src/main.cpp", files::views::git_commit_hash)}, _target{"_blank"}}, "src/main.cpp"}},
                pre{src_main_sanitised},
            },
        };
    }};
    Window licenses{"licenses", "Licenses", [](){
        return fragment{
            ul{ {_class{"licenses"}},
                li{details{
                    summary{a{{_href{"https://github.com/rthrfrd/webxx"}, _target{"_blank"}}, code{"webxx"}}},
                    pre{files::views::webxx_license},
                }},
                li{details{
                    summary{a{{_href{"https://github.com/nlohmann/json"}, _target{"_blank"}}, code{"nlohmann::json"}}},
                    pre{files::views::json_license},
                }},
                li{details{
                    summary{a{{_href{"https://github.com/leethomason/tinyxml2"}, _target{"_blank"}}, code{"TinyXML-2"}}},
                    pre{files::views::tinyxml2_license},
                }},
            },
        };
    }};
    Window build_info{"build_info", "Build Info", [](){
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
    Window c_interpreter{"c_interpreter", "C Interpreter", [](){
        return fragment{
            textarea{{_id{"c_interpreter_input"}, _placeholder{"Enter C code here..."}, _rows{"10"}, _cols{"80"}}},
            button{{_id{"c_interpreter_submit"}}, "Run"},
            textarea{{_id{"c_interpreter_output"}, _readonly{""}, _rows{"10"}, _cols{"80"}}},
        };
    }};
    Window cyndi{"cyndi", "Cyndi", [](){
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

auto page() {
    using namespace Webxx;
    return fragment{
        dv{{_id{"dock"}},

        },
        h1{"Hello from JCM!"},
        dv{{_id{"close_message"}}},
    };
}

static std::default_random_engine gen{std::random_device{}()};

auto ganyu() -> web::coro::coroutine<void> {
    using namespace Webxx;
    static std::uniform_real_distribution<float> chance_dist{};
    constexpr float ganyu_chance = 0.1f;
    constexpr auto ganyu_duration = std::chrono::seconds{60};

    while(true) {
        co_await web::coro::timeout(std::chrono::seconds{1});
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
        std::uniform_int_distribution<int> window_dist(0, open_windows.size()-1);
        auto target_window = open_windows[window_dist(gen)];

        const int height = 50;
        const int offset_x = 0;
        const int offset_y = -height;
        int z_index = web::get_style_property_int(target_window->id, "zIndex").value_or(0);

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
        web::add_element_html("main", Webxx::render(ganyu_elem));
        auto ref1 = target_window->on_move.add_unique([offset_x, offset_y](int x, int y) {
            web::set_style_property("ganyu", "left", "{}px", x+offset_x);
            web::set_style_property("ganyu", "top", "{}px", y+offset_y);
        });
        auto ref2 = target_window->on_focus.add_unique([target_window]() {
            int z_index = web::get_style_property_int(target_window->id, "zIndex").value_or(0);
            web::set_style_property("ganyu", "zIndex", "{}", z_index);
        });
        auto ref3 = target_window->on_maximize.add_unique([](bool maximized) {
            web::set_style_property("ganyu", "display", maximized ? "none" : "unset");
        });
        bool window_closed = false;
        auto ref4 = target_window->on_close.add_unique([&window_closed](){
            window_closed = true;
        });

        constexpr auto tick = std::chrono::milliseconds{100};
        for(unsigned int i=0; i<ganyu_duration/tick && !window_closed; i++) {
            co_await web::coro::timeout(std::chrono::milliseconds{100});
        }

        web::remove_element("ganyu");
    }

    co_return;
}

[[clang::export_name("main")]]
int my_main() {
    web::log("Hello World!");
    web::set_html("main", Webxx::render(page()));

    Window::setup();
    web::coro::submit([]()->web::coro::coroutine<void> {
        co_await web::coro::when_all(
            windows::blog.open(400, 450),
            windows::about_me.open(75, 50),
            windows::projects.open(800, 100),
            windows::source_code.open(900, 500),
            windows::licenses.open(100, 550),
            windows::build_info.open(50, 800)
            //windows::c_interpreter.open(400, 100)
        );

        std::string hash = web::eval("location.hash");
        bool cyndi = hash == "#cyndi";
        if(cyndi) {
            co_await windows::cyndi.open(500, 250);
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
        co_return;
    }());

    auto close_handler = []() {
        bool all_closed = true;
        for(auto& w : all_windows) {
            if(w->is_open()) {
                all_closed = false;
                break;
            }
        }

        if(all_closed) {
            using namespace Webxx;
            web::set_html("close_message", render(fragment{
                h2{"You closed all windows!"},
                button{{_id{"reopen_button"}}, "Reopen them all~!"}
            }));
            using namespace web::coro;
            submit_next([]()->coroutine<void> {
                co_await event{"reopen_button", "click"};

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

                    web::set_html("reopen_button", messages[r]);
                    co_await event{"reopen_button", "click"};
                }

                web::set_html("close_message", "");
                co_await web::coro::when_all(
                    windows::blog.open(),
                    windows::about_me.open(),
                    windows::projects.open(),
                    windows::source_code.open(),
                    windows::licenses.open(),
                    windows::build_info.open()
                    //windows::c_interpreter.open()
                );
                co_return;
            }());
        }
    };
    for(auto& w : all_windows) {
        w->on_close += close_handler;
    }

    using namespace web::coro;
    windows::blog.on_open += []() {
        submit([]()->coroutine<void> {
            std::string xml = co_await fetch("https://files.jcm.re/blog.xml");
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
                web::add_element_html("blog_container", render(fragment{
                    h2{a{{_href{link}, _target{"_blank"}}, title}},
                    p{i{pubDate}},
                    p{description},
                    hr{},
                }));
            }
            co_return;
        }());
    };
    windows::cyndi.on_open += []() {
        submit([]()->coroutine<void> {
            co_await event{"secret_submit", "click"};
            std::string password = web::get_property("secret_password", "value").value_or("");
            std::string res = co_await fetch("https://files.jcm.re/website_secret/"+password);
            web::set_html("secret_content", res);
            co_return;
        }());
    };
    windows::c_interpreter.on_open += []() {
        web::add_event_listener("c_interpreter_submit", "click", [](std::string_view){
            std::string code = web::get_property("c_interpreter_input", "value").value_or("");
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

            web::set_property("c_interpreter_output", "value", output.str());
        });
    };
    web::coro::submit(ganyu());

    return 0;
}
