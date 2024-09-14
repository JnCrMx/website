import web;
import web_coro;

#include <coroutine>
#include <chrono>

#include "webxx.h"

auto page() {
    using namespace Webxx;

    return fragment{
        h1{"Hello ", i{"world!"}},
        button{{_id{"test"}}, "Click me!"},
        h2{{_id{"counter"}}, "counter = 0"},
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
        for(int i=0; i<100; i++) {
            web::set_html("counter", "counter = {}", i);
            co_await timeout(std::chrono::milliseconds(100));
        }
        co_return;
    }());

    return 0;
}
