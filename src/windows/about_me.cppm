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
                a{{_href{"https://bsky.app/profile/jcm.re"}, _target{"_blank"}},
                    img{{_src{"images/third_party/bluesky.png"}, _title{"Bluesky"}}}},
                a{{_href{"https://wafrn.jcm.re/blog/jcm"}, _target{"_blank"}},
                    img{{_src{"images/third_party/wafrn.png"}, _title{"Wafrn"}}}},
                a{{_href{"https://blog.jcm.re/"}, _target{"_blank"}},
                    img{{_src{"images/third_party/blog.png"}, _title{"Blog"}}}},
                a{{_href{"https://keys.openpgp.org/vks/v1/by-fingerprint/7B839F0DA64069DD7832BB802F536DE3E9EC3AD8"}, _target{"_blank"}},
                    img{{_src{"images/third_party/openpgp.png"}, _title{"OpenPGP"}}}},
            },
            p{"P.S.: I love Cyndi~! 🩷🩵"},
        };
    }};
}
