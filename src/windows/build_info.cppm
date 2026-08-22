export module windows:build_info;

import std;
import webxx;
import utils;
import components;

namespace files {
    static constexpr char git_log[] = {
        #embed ".git/logs/HEAD"
    };

    namespace arrays {
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
        static constexpr std::string_view git_short_commit_hash = git_commit_hash.substr(0, 7);
    }
}

namespace windows {
    using namespace Webxx;

    export Window build_info{"build_info", "Build Info", "images/build_info.png", [](){
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
}
