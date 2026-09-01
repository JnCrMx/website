export module windows:buttons;

import std;
import webxx;
import components;

namespace windows {
    using namespace Webxx;

    constexpr auto b88x31(std::string_view name, std::string_view alt, std::string_view target, bool large = true) {
        return a{{_href{target}, _target{"_blank"}},
            img{{
                _src{std::format("/public/buttons/{}.png", name)},
                _alt{alt},
                _width{"88"},
                _height{"31"},
                _class{large ? "button-large" : "button-normal"}
            }}
        };
    }

    export Window buttons{"buttons", "Buttons", "images/buttons.png", [](){
        return fragment{
            dv{
                "I have a (for now badly made) 88x31 button! :D" "<br>",
                b88x31("jcm", "jcm.re", "https://jcm.re"),
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
            h4{"Buttons of friends and cool beings :3"},
            p{i{"If you encounter any outdated buttons or alt texts, please let me know via ", a{{_href{"#feedback"}, _target{"blank"}}, "Feedback"}}, " so I can fix it. :3"},
            p{{_style{"display: flex; flex-wrap: wrap; gap: 8px;"}},
                "Darkness there and nothing more..."
            },
        };
    }};
}
