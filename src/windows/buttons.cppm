export module windows:buttons;

import std;
import webxx;
import components;

namespace windows {
    using namespace Webxx;

    struct button_data {
        std::string_view name;
        std::string_view alt;
        std::string_view target;
        bool large;
    };
    constexpr static char buttons_tsv[] = {
        #embed "buttons.tsv"
    };
    constexpr static auto button_datas = [](){
        using std::literals::operator""sv;

        constexpr std::string_view sv{buttons_tsv, sizeof(buttons_tsv)};
        constexpr unsigned int count = std::ranges::distance(std::views::split(sv, "\n"sv)) - 2;

        std::array<button_data, count> array{};
        for(auto [l, i] : std::views::zip(sv | std::views::split("\n"sv) | std::views::drop(1) | std::views::take(count), std::views::iota(0))) {
            auto& e = array[i];
            std::string_view lv{l};

            e.name      = std::string_view{*(lv | std::views::split("\t"sv) | std::views::drop(0)).begin()};
            e.alt       = std::string_view{*(lv | std::views::split("\t"sv) | std::views::drop(3)).begin()};
            e.target    = std::string_view{*(lv | std::views::split("\t"sv) | std::views::drop(1)).begin()};
            std::string_view large{*(lv | std::views::split("\t"sv) | std::views::drop(4)).begin()};
            e.large     = large == "true";

            if(e.name.empty()) {
                static_cast<void>("\"name\" must not be empty"[-1]);
            }
            if(e.name.find_first_of("/:#?") != std::string_view::npos) {
                static_cast<void>("\"name\" must not contain reserved characters"[-1]);
            }
            if(e.alt.empty()) {
                static_cast<void>("\"alt\" must not be empty for accessibility reasons"[-1]);
            }
            if(!e.target.starts_with("http://") && !e.target.starts_with("https://")) {
                static_cast<void>("\"url\" must start with either \"https://\" or \"http://\""[-1]);
            }
            if(large != "true" && large != "false") {
                static_cast<void>("\"friend\"/\"large\" must be either \"true\" or \"false\""[-1]);
            }
        }
        if(std::string_view{*(sv | std::views::split("\n"sv) | std::views::drop(count+1)).begin()} != "") {
            static_cast<void>("file must end with a newline character"[-1]);
        }

        std::array<std::string_view, count> names; unsigned int i = 0;
        for(const auto& e : array){
            for(unsigned int j = 0; j < i; j++) {
                if(e.name == names[j]) {
                    static_cast<void>("all buttons should have unique names"[-1]);
                }
            }
            names[i++] = e.name;
        }

        return array;
    }();

    constexpr auto b88x31(std::string_view name, std::string_view alt, std::string_view target, bool large = true) {
        return a{{_href{target}, _target{"_blank"}},
            img{{
                _src{std::format("/buttons/{}", name)},
                _alt{alt},
                _width{"88"},
                _height{"31"},
                _class{large ? "button-large" : "button-normal"}
            }}
        };
    }
    constexpr auto b88x31d(const button_data& data) {
        return b88x31(data.name, data.alt, data.target, data.large);
    }

    export Window buttons{"buttons", "Buttons", "images/buttons.png", [](){
        return fragment{
            dv{
                "I have a (for now badly made) 88x31 button! :D" "<br>",
                b88x31("jcm.png", "jcm.re", "https://jcm.re"),
                details{
                    summary{"Usage information"},
                    "You can do the following things with the above button (with or without asking me first):",
                    ul{
                        li{"embed it unmodified on your own website"},
                        li{"rehost it on your server and then embed the rehosted version on your website"},
                        li{"modify the button to improve accessibility in any way you see fit (e.g. adjusting colors, improving contrast, changing the font)"},
                        li{"slightly modify the button to better fit the aesthetic of your website (e.g. adding a border, adjusting colors)"},
                        li{"modify the button to bully or tease me in a light-hearted way"}
                    },
                    "You are explicitly not allowed to do the following things with the above button:",
                    ul{
                        li{"deface the button in a genuinely mean-spirited way"},
                        li{"use the button to link to malicious content"},
                        li{"use the button on a website or to link to a website that promotes, encourages, defends, practices, or contains without criticizing it: racism, sexism, homophobia, transphobia, ableism, or any other form of discrimination or hate speech"}
                    },
                    "Overall, please just be nice and cute :3"
                },
            },
            h4{"Buttons of cool people/robots/other beings :3"},
            p{i{"If you encounter any outdated buttons or alt texts, please let me know via ", a{{_href{"#feedback"}, _target{"blank"}}, "Feedback"}}, " so I can fix it. :3"},
            p{{_style{"display: flex; flex-wrap: wrap; gap: 8px;"}},
                each(button_datas, b88x31d)
            },
        };
    }};
}
