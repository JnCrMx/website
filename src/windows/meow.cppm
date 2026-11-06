export module windows:meow;

import std;
import webpp;
import webxx;
import components;

namespace windows {
    using namespace Webxx;

    constexpr inline std::array messages = {
        "meow",
        "woof",
        "beep",
        "squeak",
    };

    export Window meow = [](){
        Window w{"meow", "Meow at me :3", "images/meow.png", [](){
            return fragment{
                form{{_action{"https://mail.jcm.re/form"}, _method{"post"}, _target{"_blank"}},
                    input{{_id{"meow_button"}, _type{"submit"}, _name{"subject"}, _value{messages[0]}, _style{"font-size: xxx-large; width: calc(100% - 2 * 5px); -webkit-touch-callout: none;"}}},
                },
                select{{_id{"meow_select"}, _style{"width: calc(100% - 2 * 5px)"}},
                    each(messages, [](auto m){
                        return option{{_value{m}}, m};
                    })
                }
            };
        }};

        using namespace webpp::coro;
        w.on_open += []() {
            auto button = *webpp::get_element_by_id("meow_button");
            auto select = *webpp::get_element_by_id("meow_select");

            select.add_event_listener("change", [](webpp::event e){
                auto button = *webpp::get_element_by_id("meow_button");
                button["value"] = *e.target()["value"].as<std::string>();
            });

            struct state {
                int touch_start{};
            };
            auto s = std::make_shared<state>();
            button.add_event_listener("contextmenu", [](webpp::event e){
                e.prevent_default();
                e.stop_propagation();
                e.stop_immediate_propagation();
            });
            button.add_event_listener("touchstart", [s](webpp::event e){
                s->touch_start = e.time_stamp();

                auto loading = img{{_id{"meow_touch_loading"}}};
                auto button = *webpp::get_element_by_id("meow_button");
                button.append_child(*webpp::create_element_from_html(Webxx::render(loading)));

                submit([](decltype(s) s)->webpp::coroutine<void> {
                    co_return;
                }(s));
            });
            button.add_event_listener("touchend", [s](webpp::event e){
                webpp::get_element_by_id("meow_touch_loading")->remove();

                auto touch_time = e.time_stamp() - s->touch_start;
                if(touch_time < 100) {
                    return;
                } else if(touch_time < 1000) {
                    e.prevent_default();
                    return;
                }
                e.prevent_default();
                // TODO: show context menu
            });
        };

        return w;
    }();
}
