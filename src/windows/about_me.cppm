export module windows:about_me;

import webxx;
import components;

namespace windows {
    using namespace Webxx;

    export Window about_me{"about_me", "About Me", "images/about_me.png", [](){
        return fragment{
            h1{"JCM"},
            p{"I'm a software developer and computer engineering student."},
            p{{_class{"socials"}},
                a{{_href{"https://github.com/JnCrMx/"}, _target{"_blank"}},
                    img{{_src{"images/third_party/github.png"}, _title{"GitHub"}}}},
                a{{_href{"https://git.jcm.re/jcm/"}, _target{"_blank"}},
                    img{{_src{"images/third_party/githug.svg"}, _title{"Forgejo instance"}}}},
                a{{_href{"https://wafrn.jcm.re/blog/jcm"}, _target{"_blank"}},
                    img{{_src{"images/third_party/wafrn.png"}, _title{"Wafrn"}}}},
                a{{_href{"https://bsky.app/profile/jcm.re"}, _target{"_blank"}},
                    img{{_src{"images/third_party/bluesky.png"}, _title{"Bluesky"}}}},
                a{{_href{"https://keys.openpgp.org/vks/v1/by-fingerprint/7B839F0DA64069DD7832BB802F536DE3E9EC3AD8"}, _target{"_blank"}},
                    img{{_src{"images/third_party/openpgp.png"}, _title{"OpenPGP"}}}},
            },
            details{
                summary{b{"Things I like"}},
                ul{
                    li{"modern C++"},
                    li{"Vulkan"},
                    li{"WebAssembly"},
                    li{"VHDL"},
                    li{"FPGAs"},
                    li{"SBCs, especially RISC-V based ones and FPGA SoCs"},
                    li{"Kubernetes"},
                    li{"the Fediverse"},
                    li{"game modding"},
                    li{"low level programming"},
                    li{"Cyberpunk 2077"},
                    li{"Slime Rancher 1 & 2"},
                },
            },
            details{
                summary{b{"Silly fun facts about me"}},
                ul{
                    li{"I am running ", a{{_href{"https://wafrn.jcm.re/blog/icepi-zero-bot"}, _target{"_blank"}},
                        "\"the most popular fediverse connected FPGA development platform in the world\""},
                        sup{a{{_href{"https://mastodon.social/@poleguy/117533687396430100"}, _target{"_blank"}}, "[citation]"}},
                    },
                }
            },
            details{
                summary{b{"How to contact me"}},
                ul{
                    li{"E-Mail: ", a{{_href{"mailto:jcm@jcm.re"}, _target{"_blank"}}, "jcm@jcm.re"}},
                    li{"DM on Fedi: ", a{{_href{"https://wafrn.jcm.re/blog/jcm"}, _target{"_blank"}}, "@jcm@wafrn.jcm.re"}},
                    li{"Matrix: ", a{{_href{"https://matrix.to/#/@jcm:jcm.re"}, _target{"_blank"}}, "@jcm:jcm.re"}},
                }
            },
            p{"P.S.: I love Cyndi~! 🩷🩵"},
        };
    }};
}
