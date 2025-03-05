module;

export module web;

import std;

namespace web {
    using std::size_t;

    [[clang::import_name("eval")]] char* eval(const char* code, size_t len);
    [[clang::import_name("set_html")]] int set_html(const char* id, size_t id_len, const char* html, size_t html_len);
    [[clang::import_name("set_property")]] int set_property(const char* id, size_t id_len, const char* prop, size_t prop_len, const char* value, size_t value_len);
    [[clang::import_name("get_property")]] char* get_property(const char* id, size_t id_len, const char* prop, size_t prop_len);
    [[clang::import_name("set_style_property")]] int set_style_property(const char* id, size_t id_len, const char* style, size_t style_len, const char* value, size_t value_len);
    [[clang::import_name("get_style_property")]] char* get_style_property(const char* id, size_t id_len, const char* prop, size_t prop_len);
    [[clang::import_name("add_element")]] int add_element(const char* parent, size_t parent_len, const char* tag, size_t tag_len, const char* id, size_t id_len);
    [[clang::import_name("add_element_html")]] int add_element_html(const char* parent, size_t parent_len, const char* html, size_t html_len);
    [[clang::import_name("remove_element")]] int remove_element(const char* id, size_t id_len);
    [[clang::import_name("log")]] void log(const char* message, size_t len);
    [[clang::import_name("add_event_listener")]] int add_event_listener(const char* id, size_t id_len, const char* event, size_t event_len, void* callback_data, bool once, bool prevent_default);
    [[clang::import_name("set_timeout")]] void set_timeout(unsigned long millis, void* callback_data);
    [[clang::import_name("fetch")]] void fetch(const char* url, size_t url_len, void* callback_data);

    [[clang::export_name("new_string")]]
    char* new_string(size_t len) {
        return new char[len];
    }

    [[clang::export_name("delete_string")]]
    void delete_string(char* ptr) {
        delete[] ptr;
    }

    export enum class web_error {
        element_not_found = 1,
        property_not_found = 2,
        conversion_error = 3,
    };
    std::expected<void, web_error> from_web_error(int e) {
        if(e == std::to_underlying(web_error::element_not_found)) {
            return std::unexpected(web_error::element_not_found);
        } else if(e == std::to_underlying(web_error::property_not_found)) {
            return std::unexpected(web_error::property_not_found);
        }
        return {};
    }
    template<typename T>
    std::expected<T*, web_error> filter_error(T* t) {
        return from_web_error(reinterpret_cast<std::uintptr_t>(t)).transform([t]() {
            return t;
        });
    }

    void log(std::string_view message) {
        log(message.data(), message.size());
    }
    export template<class... Args>
    void log(std::format_string<Args...> fmt, Args&&... args) {
        std::string s = std::format(fmt, std::forward<Args>(args)...);
        log(s.data(), s.size());
    }

    export std::string eval(std::string_view code) {
        char* str_ptr = eval(code.data(), code.size());
        std::string s{str_ptr};
        delete_string(str_ptr);
        return s;
    }
    export template<class... Args>
    std::string eval(std::format_string<Args...> code, Args&&... args) {
        std::string s = std::format(code, std::forward<Args>(args)...);
        return eval(s);
    }

    export std::expected<void, web_error> set_html(std::string_view id, std::string_view html) {
        return from_web_error(set_html(id.data(), id.size(), html.data(), html.size()));
    }
    export template<class... Args>
    std::expected<void, web_error> set_html(std::string_view id, std::format_string<Args...> html, Args&&... args) {
        std::string s = std::format(html, std::forward<Args>(args)...);
        return from_web_error(set_html(id.data(), id.size(), s.data(), s.size()));
    }

    export std::expected<void, web_error> set_property(std::string_view id, std::string_view property, std::string_view value) {
        return from_web_error(set_property(id.data(), id.size(), property.data(), property.size(), value.data(), value.size()));
    }
    export template<class... Args>
    std::expected<void, web_error> set_property(std::string_view id, std::string_view property, std::format_string<Args...> value, Args&&... args) {
        std::string s = std::format(value, std::forward<Args>(args)...);
        return from_web_error(set_property(id.data(), id.size(), property.data(), property.size(), s.data(), s.size()));
    }

    export std::expected<std::string, web_error> get_property(std::string_view id, std::string_view property) {
        auto ptr = filter_error(get_property(id.data(), id.size(), property.data(), property.size()));
        if(!ptr) {
            return std::unexpected(ptr.error());
        }
        std::string s{*ptr};
        delete_string(*ptr);
        return s;
    }
    export std::expected<int, web_error> get_property_int(std::string_view id, std::string_view property) {
        return get_property(id, property).transform([](std::string_view s) {
            return std::stoi(std::string{s});
        });
    }

    std::set<std::string> parse_classes(std::string_view classes) {
        std::set<std::string> result;
        std::istringstream ss{std::string{classes}};
        std::string token;
        while(std::getline(ss, token, ' ')) {
            result.insert(token);
        }
        return result;
    }
    std::string join_classes(const std::set<std::string>& classes) {
        std::string result;
        for(const auto& c : classes) {
            result += c + " ";
        }
        return result;
    }

    export std::expected<void, web_error> add_class(std::string_view id, std::string_view class_name) {
        std::string classes = get_property(id, "classList").value_or("");
        auto class_set = parse_classes(classes);
        class_set.insert(std::string{class_name});
        return set_property(id, "classList", join_classes(class_set));
    }
    export std::expected<void, web_error> remove_class(std::string_view id, std::string_view class_name) {
        std::string classes = get_property(id, "classList").value_or("");
        auto class_set = parse_classes(classes);
        class_set.erase(std::string{class_name});
        return set_property(id, "classList", join_classes(class_set));
    }
    export std::expected<void, web_error> toggle_class(std::string_view id, std::string_view class_name) {
        std::string classes = get_property(id, "classList").value_or("");
        auto class_set = parse_classes(classes);
        if(class_set.contains(std::string{class_name})) {
            class_set.erase(std::string{class_name});
        } else {
            class_set.insert(std::string{class_name});
        }
        return set_property(id, "classList", join_classes(class_set));
    }
    export std::expected<bool, web_error> has_class(std::string_view id, std::string_view class_name) {
        auto classes = get_property(id, "classList").or_else([](web_error e) -> std::expected<std::string, web_error> {
            if(e == web_error::property_not_found) {
                return std::string{};
            } else {
                return std::unexpected(e);
            }
        });
        auto class_set = classes.transform(parse_classes);
        return class_set.transform([class_name](const std::set<std::string>& classes) {
            return classes.contains(std::string{class_name});
        });
    }

    export std::expected<void, web_error> set_style_property(std::string_view id, std::string_view property, std::string_view value) {
        return from_web_error(set_style_property(id.data(), id.size(), property.data(), property.size(), value.data(), value.size()));
    }
    export template<class... Args>
    std::expected<void, web_error> set_style_property(std::string_view id, std::string_view property, std::format_string<Args...> value, Args&&... args) {
        std::string s = std::format(value, std::forward<Args>(args)...);
        return from_web_error(set_style_property(id.data(), id.size(), property.data(), property.size(), s.data(), s.size()));
    }
    export std::expected<std::string, web_error> get_style_property(std::string_view id, std::string_view property) {
        auto ptr = filter_error(get_style_property(id.data(), id.size(), property.data(), property.size()));
        if(!ptr) {
            return std::unexpected(ptr.error());
        }
        std::string s{*ptr};
        delete_string(*ptr);
        return s;
    }
    export std::expected<int, web_error> get_style_property_int(std::string_view id, std::string_view property) {
        return get_style_property(id, property).transform([](std::string s) {
            int v;
            if(std::from_chars(s.data(), s.data() + s.size(), v)) {
                return v;
            } else {
                return 0;
            }
        });
    }

    export std::expected<void, web_error> add_element(std::string_view parent, std::string_view tag, std::string_view id) {
        return from_web_error(add_element(parent.data(), parent.size(), tag.data(), tag.size(), id.data(), id.size()));
    }

    export std::expected<void, web_error> add_element_html(std::string_view parent, std::string_view html) {
        return from_web_error(add_element_html(parent.data(), parent.size(), html.data(), html.size()));
    }
    export template<class... Args>
    std::expected<void, web_error> add_element_html(std::string_view parent, std::format_string<Args...> html, Args&&... args) {
        std::string s = std::format(html, std::forward<Args>(args)...);
        return from_web_error(add_element_html(parent.data(), parent.size(), s.data(), s.size()));
    }

    export std::expected<void, web_error> remove_element(std::string_view id) {
        return from_web_error(remove_element(id.data(), id.size()));
    }

    using event_callback = std::function<void(std::string_view)>;
    export class callback_data {
        public:
            void abandon() {
                callback = nullptr;
            }
        private:
            friend callback_data* add_event_listener(std::string_view, std::string_view, event_callback, bool, bool);
            friend callback_data* set_timeout(std::chrono::milliseconds, event_callback);
            friend callback_data* fetch(std::string_view, event_callback);
            friend void callback(void*, char*, size_t);

            callback_data(event_callback callback, bool once) : callback{callback}, once{once} {}
            event_callback callback;
            bool once = false;
            callback_data* sibling = nullptr;
    };

    export callback_data* add_event_listener(std::string_view id, std::string_view event, event_callback callback, bool once = false, bool prevent_default = false) {
        callback_data* data = new callback_data{callback, once};
        add_event_listener(id.data(), id.size(), event.data(), event.size(),
            data, once, prevent_default);
        return data;
    }

    export callback_data* set_timeout(std::chrono::milliseconds duration, event_callback callback) {
        callback_data* data = new callback_data{callback, true};
        set_timeout(duration.count(), data);
        return data;
    }

    export callback_data* fetch(std::string_view url, event_callback callback) {
        callback_data* data = new callback_data{callback, true};
        fetch(url.data(), url.size(), data);
        return data;
    }

    [[clang::export_name("callback")]]
    void callback(void* ptr, char* data, size_t len) {
        callback_data* cb = static_cast<callback_data*>(ptr);
        if(cb->callback) {
            cb->callback(data ? std::string_view{data, len} : std::string_view{});
        }
        if(cb->once) {
            delete cb;
        }
    }
}
