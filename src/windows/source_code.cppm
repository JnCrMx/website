export module windows:source_code;

import std;
import webxx;
import utils;
import components;

namespace files {
    static constexpr char src_main[] = { // secrets in this file will be optimized away :D
        #embed "src/main.cpp"
    };
    static constexpr char git_log[] = {
        #embed ".git/logs/HEAD"
    };

    namespace arrays {
        static constexpr auto src_main = std::to_array(::files::src_main);

        static constexpr auto git_log = std::to_array(::files::git_log);
        static constexpr auto git_commit_hash = [](){
            constexpr std::string_view sv{::files::git_log, sizeof(::files::git_log)};
            constexpr auto pos1 = sv.find_last_of('\n');
            constexpr auto pos2 = sv.find_last_of('\n', pos1-1);
            constexpr auto pos3 = pos2 == std::string_view::npos ? 0 : pos2; // the file might have only one line
            constexpr auto pos4 = sv.find(' ', pos3);

            return utils::substr<git_log, pos4+1, 40>();
        }();
    }
    namespace views {
        static constexpr std::string_view git_commit_hash{::files::arrays::git_commit_hash.data(), ::files::arrays::git_commit_hash.size()};
    }
}

static constexpr auto src_main_sanitised_array = [](){
    constexpr auto s0 = files::arrays::src_main;
    constexpr auto s1 = utils::replace_sub_str<s0, std::to_array("&"), std::to_array("&amp;")>();
    constexpr auto s2 = utils::replace_sub_str<s1, std::to_array("<"), std::to_array("&lt;")>();
    constexpr auto s3 = utils::replace_sub_str<s2, std::to_array(">"), std::to_array("&gt;")>();
    constexpr auto s4 = utils::replace_sub_str<s3, std::to_array("cyndi"), std::to_array("secret")>();
    constexpr auto s5 = utils::replace_sub_str<s4, std::to_array("Cyndi"), std::to_array("secret")>();
    constexpr auto s6 = utils::replace_sub_str<s5, std::to_array("x42054c01"), std::to_array("secret")>();
    return s6;
}();
static constexpr std::string_view src_main_sanitised{src_main_sanitised_array.data(), src_main_sanitised_array.size()};

namespace windows {
    using namespace Webxx;

    export Window source_code{"source_code", "Source Code", "images/source_code.png", [](){
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
}
