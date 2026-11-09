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
        "waf",
        "yip",
        "eepeep",
        "*pat*",
        "*boop*",
        "*scratch*",
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
                },
                select{{_id{"meow_select_popup"}, _style{"height: 0px; width: 0px; margin: 0px; padding: 0px; opacity: 0;"}},
                    each(messages, [](auto m){
                        return option{{_value{m}}, m};
                    }),
                    option{{_value{"miiiiii"}}, "miiiiii"},
                }
            };
        }};

        using namespace webpp::coro;
        w.on_open += []() {
            auto button = *webpp::get_element_by_id("meow_button");

            webpp::get_element_by_id("meow_select")->add_event_listener("change", [](webpp::event e){
                std::string s = *e.target()["value"].as<std::string>();
                (*webpp::get_element_by_id("meow_button"))["value"] = s;
                (*webpp::get_element_by_id("meow_select_popup"))["selectedIndex"] = static_cast<int>(std::distance(messages.begin(), std::find(messages.begin(), messages.end(), s)));
            });
            webpp::get_element_by_id("meow_select_popup")->add_event_listener("change", [](webpp::event e){
                std::string s = *e.target()["value"].as<std::string>();
                (*webpp::get_element_by_id("meow_button"))["value"] = s;
                (*webpp::get_element_by_id("meow_select"))["selectedIndex"] = static_cast<int>(std::distance(messages.begin(), std::find(messages.begin(), messages.end(), s)));
            });

            struct state {
                int touch_start{};
            };
            auto s = std::make_shared<state>(); // we are leaking here I think >_<
            button.add_event_listener("contextmenu", [](webpp::event e){
                e.prevent_default();
                e.stop_propagation();
                e.stop_immediate_propagation();
            });
            button.add_event_listener("touchstart", [s](webpp::event e){
                s->touch_start = e.time_stamp();

                auto touch = *(*e["changedTouches"].as<webpp::js_object>())["0"].as<webpp::js_object>();
                int x = *touch["clientX"].as<int>();
                int y = *touch["clientY"].as<int>();

                auto loading = img{{_id{"meow_touch_loading"}, _src{"ganyu.png"}, _style{std::format("position: fixed; left: {}px; top: {}px; z-index: 999999999; transform: translate(-50%, -50%); width: 5rem; height: 5rem;", x, y)}}};
                auto main = *webpp::get_element_by_id("main");
                main.append_child(*webpp::create_element_from_html(Webxx::render(loading)));
            });
            button.add_event_listener("touchend", [s](webpp::event e){
                webpp::get_element_by_id("meow_touch_loading")->remove();

                auto touch_time = e.time_stamp() - s->touch_start;
                if(touch_time < 100) {
                    return;
                } else if(touch_time < 500) {
                    e.prevent_default();
                    return;
                }
                e.prevent_default();

                webpp::eval("document.getElementById(\"meow_select_popup\").showPicker()");
            });
        };

        return w;
    }();
}
