export module windows:meow;

import webxx;
import components;

namespace windows {
    using namespace Webxx;

    export Window meow{"meow", "Meow at me :3", "images/meow.png", [](){
        return fragment{
            form{{_action{"https://mail.jcm.re/form"}, _method{"post"}, _target{"_blank"}},
                input{{_type{"submit"}, _name{"subject"}, _value{"meow~"}, _style{"font-size: xxx-large; width: 100%;"}}},
            }
        };
    }};
}
