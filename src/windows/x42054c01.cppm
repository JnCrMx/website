export module windows:x42054c01;

import std;
import webpp;
import webxx;
import components;

namespace windows {
    using namespace Webxx;

    export Window x42054c01 = [](){
        Window w{"x42054c01", "x42054c01", "images/x42054c01.png", [](){
            return fragment{
                h3{{_id{"x42054c01_status"}}, "Connecting..."},
                dv{{_style{"display: flex; flex-direction: column; width: 100%;"}},
                    input{{_id{"x42054c01_slider_1"}, _type{"range"}, _min{"0"}, _max{"100"}, _value{"0"}, _style{"width: 100%;"}, _disabled{}}},
                    input{{_id{"x42054c01_slider_2"}, _type{"range"}, _min{"0"}, _max{"100"}, _value{"0"}, _style{"width: 100%;"}, _disabled{}}},
                    input{{_id{"x42054c01_slider_3"}, _type{"range"}, _min{"0"}, _max{"100"}, _value{"0"}, _style{"width: 100%;"}, _disabled{}}},
                }
            };
        }};

        using std::literals::operator""s;
        using std::literals::operator""sv;
        static std::unique_ptr<webpp::websocket> ws;
        w.on_open += []() {
            ws = std::make_unique<webpp::websocket>(webpp::websocket::create("wss://x42054c01.d.jcm.re/ws"));
            static bool error = false;

            static auto get_sliders = [](){
                return std::views::iota(0) |
                    std::views::take(3) |
                    std::views::transform([](unsigned int i){return *webpp::get_element_by_id(std::format("x42054c01_slider_{}", i+1));});
            };

            ws->on_message([](webpp::event e){
                if(!x42054c01.is_open()) {
                    return;
                }
                auto message = *e["data"].as<std::string>();
                auto view = message |
                    std::views::split(" "sv) |
                    std::views::take(3) |
                    std::views::transform([](auto&& a){return std::string_view{a};}) |
                    std::views::transform([](auto&& a){int result{}; std::from_chars(a.data(), a.data() + a.size(), result); return result;});
                auto sliders = get_sliders();
                for(auto [v, slider] : std::views::zip(view, sliders)) {
                    slider["value"] = v;
                }
            });

            static auto update = [](){
                if(!x42054c01.is_open()) {
                    return;
                }
                auto message = get_sliders() |
                    std::views::transform([](auto&& a){return *a["value"].template as<int>();}) |
                    std::views::transform([](auto&& a){return std::to_string(a) + " ";}) |
                    std::views::join |
                    std::ranges::to<std::string>();
                std::string_view message_view{message};
                message_view.remove_suffix(1);

                ws->send(message_view);
            };

            static std::vector<webpp::callback_data*> slider_listeners;
            ws->on_open([](webpp::event e){
                if(!x42054c01.is_open()) {
                    return;
                }
                auto status = *webpp::get_element_by_id("x42054c01_status");
                status.inner_text("Connected!");
                status.style()["color"] = "#007700";

                slider_listeners = get_sliders() |
                    std::views::transform([](webpp::element&& s){return s.add_event_listener("input", [](webpp::event){update();});}) |
                    std::ranges::to<std::vector<webpp::callback_data*>>();
                for(auto s : get_sliders()) {
                    s["disabled"] = false;
                }
            });
            ws->on_error([](webpp::event e){
                if(!x42054c01.is_open()) {
                    return;
                }
                auto status = *webpp::get_element_by_id("x42054c01_status");
                status.inner_text("Failed to connect.");
                status.style()["color"] = "#FF0000";
                error = true;
            });
            ws->on_close([](webpp::event e){
                for(auto* cb : slider_listeners) {
                    cb->abandon();
                }
                slider_listeners.clear(); // we leak memory here, but that is kinda okay I guess

                if(!x42054c01.is_open()) {
                    return;
                }
                if(!error) {
                    auto status = *webpp::get_element_by_id("x42054c01_status");
                    status.inner_text("Disconnected.");
                    status.style()["color"] = "#FF0000";
                }
                for(auto s : get_sliders()) {
                    s["disabled"] = true;
                }
            });
        };
        w.on_close += [](){
            if(ws) {
                ws->close();
                ws.reset();
            }
        };
        return w;
    }();
}
