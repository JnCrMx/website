export module windows:cyndi;

import std;
import webpp;
import webxx;
import components;

namespace windows {
    using namespace Webxx;

    export Window cyndi = [](){
        Window w{"cyndi", "Cyndi", "images/cyndi.png", [](){
            return fragment{
                h1{"I love you 🩷"},
                dv{{_id{"secret_content"}},
                    input{{_id{"secret_password"}, _type{"password"}, _placeholder{"Password"}}},
                    button{{_id{"secret_submit"}}, "💌"},
                },
            };
        }};

        using namespace webpp::coro;
        w.on_open += []() {
            submit([]()->coroutine<void> {
                co_await webpp::get_element_by_id("secret_submit")->event("click");
                std::string password = webpp::get_element_by_id("secret_password")->get_property<std::string>("value").value_or("");
                std::string res = co_await fetch("https://files.jcm.re/website_secret/"+password).then(std::mem_fn(&webpp::response::co_text));
                webpp::get_element_by_id("secret_content")->inner_html(res);
                co_return;
            }());
        };
        return w;
    }();
}
