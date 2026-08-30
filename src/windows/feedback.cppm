export module windows:feedback;

import webxx;
import components;

namespace windows {
    using namespace Webxx;

    export Window feedback{"feedback", "Feedback", "images/feedback.png", [](){
        return fragment{
            form{{_action{"https://mail.jcm.re/form"}, _method{"post"}},
                dv{
                    input{{_type{"submit"}, _name{"subject"}, _value{"praise 😊"}}},
                    input{{_type{"submit"}, _name{"subject"}, _value{"criticize 😡"}}},
                    input{{_type{"submit"}, _name{"subject"}, _value{"bully 😈"}}},
                },
                hr{},
                textarea{{_style{"width: 100%; margin-top: 5px;"}, _name{"message"}, _rows{"5"}, _placeholder{"Type your praise/critique/bullying here! (optional)"}}},
            }
        };
    }};
}
