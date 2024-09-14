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

auto page() {
    using namespace Webxx;

    return fragment{
        h1{"Hello ", i{"world!"}},
        button{{_id{"test"}}, "Click me!"},
        h2{{_id{"counter"}}, "Coroutine counter = 0"},
        details{
            summary{"Source Code (" __FILE__ ")"},
            pre{this_file_view},
        },
        details{
            summary{"Licenses"},
            details{
                summary{a{{_href{"https://github.com/rthrfrd/webxx"}}, code{"webxx"}}},
                pre{webxx_license_view},
            },
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
