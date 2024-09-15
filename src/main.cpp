import web;
import web_coro;

#include <coroutine>
#include <chrono>

#include "webxx.h"

constexpr char this_file[] = {
#embed __FILE__
};
constexpr std::string_view this_file_view{this_file, sizeof(this_file)};

constexpr char webxx_license[] = {
#embed "../build/_deps/webxx-src/LICENSE.md"
};
constexpr std::string_view webxx_license_view{webxx_license, sizeof(webxx_license)};

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


auto page() {
    using namespace Webxx;

    return fragment{
        h1{"Hello from JCM!"},
        button{{_id{"test"}}, "Click me!"},
        h2{{_id{"counter"}}, "Coroutine counter = 0"},
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

[[clang::export_name("main")]]
int main() {
    web::log("Hello World!");
    web::set_html("main", Webxx::render(page()));
    web::add_event_listener("test", "click", [i=0]() mutable {
        web::set_html("test", "You clicked me {} times!", ++i);
        if(i == 10) {
            web::set_timeout(std::chrono::milliseconds(1000), [](){
                web::eval("alert('You clicked me 10 times!')");
            });
        }
    });

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
