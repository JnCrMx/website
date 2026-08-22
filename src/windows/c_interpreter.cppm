export module windows:c_interpreter;

import std;
import webpp;
import webxx;
import utils;
import components;
import c_interpreter;

namespace windows {
    using namespace Webxx;

    export Window c_interpreter = [](){
        Window w{"c_interpreter", "C Interpreter", "images/c_interpreter.png", [](){
            return fragment{
                textarea{{_id{"c_interpreter_input"}, _placeholder{"Enter C code here..."}, _rows{"10"}, _cols{"80"}}},
                button{{_id{"c_interpreter_submit"}}, "Run"},
                textarea{{_id{"c_interpreter_output"}, _readonly{""}, _rows{"10"}, _cols{"80"}}},
            };
        }};

        using namespace webpp::coro;
        w.on_open += []() {
            webpp::get_element_by_id("c_interpreter_submit")->add_event_listener("click", [](webpp::event){
                std::string code = webpp::get_element_by_id("c_interpreter_input")->get_property<std::string>("value").value_or("");
                std::ostringstream output;

                {
                    std::istringstream input{code};
                    c_interpreter::lexer<1> lex{input};

                    while(true) {
                        auto t = lex.next();
                        if(!t) {
                            output << "\nLexer error: " << t.error();
                            break;
                        }
                        output << utils::enum_name(t->kind);
                        if(t->kind == c_interpreter::token_kind::EOF_)
                            break;
                        std::visit([&output](auto&& arg) {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr(std::is_same_v<T, std::string>) {
                                output << "[" << std::quoted(arg) << "]";
                            } else if constexpr(!std::is_same_v<T, std::monostate>) {
                                output << "[" << arg << "]";
                            }
                        }, t->value);
                        output << " ";
                    }
                }
                output << "\n";
                {
                    std::istringstream input{code};
                    c_interpreter::lexer<1> lex{input};
                    c_interpreter::parser parser{lex};
                    auto res = parser.parse_program();
                    if(!res) {
                        output << "Parser error: " << res.error() << "\n";
                    }
                }
                webpp::get_element_by_id("c_interpreter_output")->set_property("value", output.str());
            });
        };
        return w;
    }();
}
