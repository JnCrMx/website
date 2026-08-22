export module windows:licenses;

import std;
import webxx;
import components;

namespace files {
    static constexpr char webxx_license[] = {
        #embed "build/_deps/webxx-src/LICENSE.md"
    };
    static constexpr char tinyxml2_license[] = {
        #embed "build/_deps/tinyxml2-src/LICENSE.txt"
    };
    namespace views {
        static constexpr std::string_view webxx_license{::files::webxx_license, sizeof(::files::webxx_license)};
        static constexpr std::string_view tinyxml2_license{::files::tinyxml2_license, sizeof(::files::tinyxml2_license)};
    }
}

namespace windows {
    using namespace Webxx;

    export Window licenses{"licenses", "Licenses", "images/licenses.png", [](){
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
}
