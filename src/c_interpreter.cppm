module;

export module c_interpreter;

import std;
import utils;

namespace c_interpreter {

export enum class token_kind {
    IDENTIFIER,
    INTEGER,
    STRING,
    PLUS,
    MINUS,
    STAR,
    MOD,
    DIV,
    ASSIGN,
    EQ,
    NE,
    LT,
    LE,
    GT,
    GE,
    LROUNDBRACKET,
    RROUNDBRACKET,
    LSQUAREBRACKET,
    RSQUAREBRACKET,
    LCURLYBRACKET,
    RCURLYBRACKET,
    COLON,
    COMMA,
    IF,
    ELSE,
    WHILE,
    FOR,
    INT,
    CHAR,
    VOID,
    RETURN,
    RARROW,
    SEMICOLON,
    EOF_
};

export struct token {
    token_kind kind;
    std::variant<std::monostate, std::string, int> value;
};

export using lexer_error = std::string;

export template<unsigned int Lookahead>
class lexer {
public:
    lexer(std::istream& input) : m_input(input) {
        for (unsigned int i = 0; i < Lookahead+1; ++i) {
            m_buffer[i] = read().value_or(token{token_kind::EOF_});
        }
    }
    std::expected<token, lexer_error> next() {
        token result = m_buffer[0];
        for (unsigned int i = 0; i < Lookahead; ++i) {
            m_buffer[i] = m_buffer[i+1];
        }
        if(auto r = read()) {
            m_buffer[Lookahead] = r.value();
            return result;
        } else {
            return std::unexpected(r.error());
        }
    }
    token peek(unsigned int n = 0) {
        return m_buffer[n];
    }
    constexpr static unsigned int lookahead = Lookahead;
private:
    std::istream& m_input;
    std::array<token, Lookahead+1> m_buffer;
    static constexpr auto EOF = std::string::traits_type::eof();

    std::expected<token, lexer_error> read() {
        while(true) {
            char c = m_input.peek();
            if(std::isspace(c)) {
                m_input.get();
            } else if(c == '#') {
                m_input.get();
                c = m_input.peek();
                while(c != EOF && c != '\n' && c != '\r') {
                    m_input.get();
                    c = m_input.peek();
                }
            } else if(c == '+') {
                m_input.get();
                return token{token_kind::PLUS};
            } else if(c == '-') {
                m_input.get();
                c = m_input.peek();
                if(c == '>') {
                    m_input.get();
                    return token{token_kind::RARROW};
                } else {
                    return token{token_kind::MINUS};
                }
            } else if(c == '*') {
                m_input.get();
                return token{token_kind::STAR};
            } else if(c == '%') {
                m_input.get();
                return token{token_kind::MOD};
            } else if(c == '/') {
                m_input.get();
                return token{token_kind::DIV};
            } else if(c == '=') {
                m_input.get();
                c = m_input.peek();
                if(c == '=') {
                    m_input.get();
                    return token{token_kind::EQ};
                } else {
                    return token{token_kind::ASSIGN};
                }
            } else if(c == '!') {
                m_input.get();
                c = m_input.peek();
                if(c != '=') {
                    return std::unexpected(std::format("unknown lexeme '!{}'", c));
                }
                m_input.get();
                return token{token_kind::NE};
            } else if(c == '<') {
                m_input.get();
                c = m_input.peek();
                if(c == '=') {
                    m_input.get();
                    return token{token_kind::LE};
                } else {
                    return token{token_kind::LT};
                }
            } else if(c == '>') {
                m_input.get();
                c = m_input.peek();
                if(c == '=') {
                    m_input.get();
                    return token{token_kind::GE};
                } else {
                    return token{token_kind::GT};
                }
            } else if(c == '(') {
                m_input.get();
                return token{token_kind::LROUNDBRACKET};
            } else if(c == ')') {
                m_input.get();
                return token{token_kind::RROUNDBRACKET};
            } else if(c == ':') {
                m_input.get();
                return token{token_kind::COLON};
            } else if(c == '[') {
                m_input.get();
                return token{token_kind::LSQUAREBRACKET};
            } else if(c == ']') {
                m_input.get();
                return token{token_kind::RSQUAREBRACKET};
            } else if(c == '{') {
                m_input.get();
                return token{token_kind::LCURLYBRACKET};
            } else if(c == '}') {
                m_input.get();
                return token{token_kind::RCURLYBRACKET};
            } else if(c == ',') {
                m_input.get();
                return token{token_kind::COMMA};
            } else if(c == ';') {
                m_input.get();
                return token{token_kind::SEMICOLON};
            } else if(std::isalpha(c) || c == '_') {
                std::string name = "";
                while(c = m_input.peek(), std::isalnum(c) || c == '_') {
                    name += c;
                    m_input.get();
                }
                if(name == "if") {
                    return token{token_kind::IF};
                } else if(name == "else") {
                    return token{token_kind::ELSE};
                } else if(name == "while") {
                    return token{token_kind::WHILE};
                } else if(name == "for") {
                    return token{token_kind::FOR};
                } else if(name == "int") {
                    return token{token_kind::INT};
                } else if(name == "char") {
                    return token{token_kind::INT};
                } else if(name == "void") {
                    return token{token_kind::VOID};
                } else if(name == "return") {
                    return token{token_kind::RETURN};
                }
                return token{token_kind::IDENTIFIER, name};
            } else if(std::isdigit(c)) {
                std::string value = "";
                while(c = m_input.peek(), std::isdigit(c)) {
                    value += c;
                    m_input.get();
                }
                return token{token_kind::INTEGER, std::stoi(value)};
            } else if(c == '"') {
                std::string value;
                m_input >> std::quoted(value);
                return token{token_kind::STRING, value};
            } else if(c == EOF) {
                return token{token_kind::EOF_};
            } else {
                return std::unexpected(std::format("invalid character detected: '{}'", c));
            }
        }
    }
};

export namespace ast {
    enum class primitive_type {
        INT,
        CHAR,
        VOID
    };
    struct pointer_type;
    using type = std::variant<primitive_type, std::string, pointer_type>;
    struct pointer_type {
        std::shared_ptr<type> base;
    };

    struct parameter_declaration {
        std::string name;
        type type;
    };

    using expression = std::variant<std::monostate>;
    struct return_statement {
        std::shared_ptr<expression> value;
    };

    using statement = std::variant<return_statement>;

    struct function_definition{
        std::string name;
        type return_type;
        std::vector<parameter_declaration> parameters;
        std::vector<statement> body;
    };

    struct program {
        std::vector<function_definition> functions;
    };
}
export using parser_error = std::string;

#define TRY(x) if(auto r = (x); !r) { return std::unexpected(r.error()); }
#define TRY_USE(x, y) if(auto r = (x)) { y; } else { return std::unexpected(r.error()); }
#define MATCH(x) if(auto r = match((x)); !r) { return std::unexpected(r.error()); }
#define MATCH_USE(x, y) if(auto r = match((x))) { y; } else { return std::unexpected(r.error()); }

export class parser {
    public:
        parser(lexer<1>& lexer) : m_lexer(lexer) {}

        std::expected<ast::program, parser_error> parse_program() {
            ast::program result;
            while(m_lexer.peek().kind != token_kind::EOF_) {
                TRY_USE(parse_function_definition(), result.functions.push_back(r.value()));
            }
            return result;
        }
    protected:
        std::expected<ast::function_definition, parser_error> parse_function_definition() {
            ast::function_definition result;
            TRY_USE(parse_type(), result.return_type = r.value());
            MATCH_USE(token_kind::IDENTIFIER, result.name = std::get<std::string>(r->value));

            MATCH(token_kind::LROUNDBRACKET);

            while(!check(token_kind::RROUNDBRACKET)) {
                TRY_USE(parse_parameter_declaration(), result.parameters.push_back(r.value()));
                if(check(token_kind::COMMA)) {
                    next();
                } else {
                    break;
                }
            }
            MATCH(token_kind::RROUNDBRACKET);
            MATCH(token_kind::LCURLYBRACKET);
            while(!check(token_kind::RCURLYBRACKET)) {
                TRY_USE(parse_statement(), result.body.push_back(r.value()));
            }
            MATCH(token_kind::RCURLYBRACKET);
            return result;
        }
        std::expected<ast::type, parser_error> parse_type() {
            if(check(token_kind::INT)) {
                next();
                return ast::primitive_type::INT;
            } else if(check(token_kind::CHAR)) {
                next();
                return ast::primitive_type::CHAR;
            } else if(check(token_kind::VOID)) {
                next();
                return ast::primitive_type::VOID;
            } else {
                std::string ident;
                MATCH_USE(token_kind::IDENTIFIER, ident = std::get<std::string>(r->value));
                return ident;
            }
        }
        std::expected<ast::parameter_declaration, parser_error> parse_parameter_declaration() {
            ast::parameter_declaration result;
            TRY_USE(parse_type(), result.type = r.value());
            MATCH_USE(token_kind::IDENTIFIER, result.name = std::get<std::string>(r->value));
            return result;
        }
        std::expected<ast::statement, parser_error> parse_statement() {
            if(check(token_kind::RETURN)) {
                return parse_return_statement();
            } else {
                return std::unexpected("unexpected token");
            }
        }
        std::expected<ast::return_statement, parser_error> parse_return_statement() {
            ast::return_statement result;
            next();
            if(check(token_kind::SEMICOLON)) {
                next();
                return result;
            } else {
                TRY_USE(parse_expression(), result.value = std::make_shared<ast::expression>(r.value()));
                MATCH(token_kind::SEMICOLON);
                return result;
            }
        }
        std::expected<ast::expression, parser_error> parse_expression() {
            return std::unexpected("not implemented");
        }
    private:
        lexer<1>& m_lexer;

        template<typename... Kinds>
        bool check(Kinds... kinds) {
            unsigned int i=0;
            static_assert(sizeof...(kinds) > 0, "check must be called with at least one argument");
            static_assert(sizeof...(kinds) <= std::remove_reference_t<decltype(m_lexer)>::lookahead+1, "check must be called with at most Lookahead arguments");
            for(token_kind kind : {kinds...}) {
                if(m_lexer.peek(i).kind != kind) {
                    return false;
                }
                i++;
            }
            return true;
        }
        std::expected<token, parser_error> match(token_kind kind) {
            return m_lexer.next()
                .transform_error([](lexer_error e) { return parser_error("LEXER: "+e); })
                .and_then([kind](token t) -> std::expected<token, parser_error> {
                    if(t.kind != kind) {
                        return std::unexpected(std::format("unexpected token {}, expected {}",
                            utils::enum_name(t.kind), utils::enum_name(kind)));
                    }
                    return t;
                });
        }
        void next() {
            m_lexer.next();
        }
};

}
