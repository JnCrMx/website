module;

#include <chrono>
#include <format>
#include <functional>
#include <set>
#include <sstream>
#include <string_view>

export module web;

namespace web {
    [[clang::import_name("eval")]] char* eval(const char* code, size_t len);
    [[clang::import_name("set_html")]] void set_html(const char* id, size_t id_len, const char* html, size_t html_len);
    [[clang::import_name("set_property")]] void set_property(const char* id, size_t id_len, const char* prop, size_t prop_len, const char* value, size_t value_len);
    [[clang::import_name("get_property")]] char* get_property(const char* id, size_t id_len, const char* prop, size_t prop_len);
    [[clang::import_name("set_style_property")]] void set_style_property(const char* id, size_t id_len, const char* style, size_t style_len, const char* value, size_t value_len);
    [[clang::import_name("add_element")]] void add_element(const char* parent, size_t parent_len, const char* tag, size_t tag_len, const char* id, size_t id_len);
    [[clang::import_name("add_element_html")]] void add_element_html(const char* parent, size_t parent_len, const char* html, size_t html_len);
    [[clang::import_name("remove_element")]] void remove_element(const char* id, size_t id_len);
    [[clang::import_name("log")]] void log(const char* message, size_t len);
    [[clang::import_name("add_event_listener")]] void add_event_listener(const char* id, size_t id_len, const char* event, size_t event_len, void* callback_data, bool once, bool prevent_default);
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

    export void set_html(std::string_view id, std::string_view html) {
        set_html(id.data(), id.size(), html.data(), html.size());
    }
    export template<class... Args>
    void set_html(std::string_view id, std::format_string<Args...> html, Args&&... args) {
        std::string s = std::format(html, std::forward<Args>(args)...);
        set_html(id.data(), id.size(), s.data(), s.size());
    }

    export void set_property(std::string_view id, std::string_view property, std::string_view value) {
        set_property(id.data(), id.size(), property.data(), property.size(), value.data(), value.size());
    }
    export template<class... Args>
    void set_property(std::string_view id, std::string_view property, std::format_string<Args...> value, Args&&... args) {
        std::string s = std::format(value, std::forward<Args>(args)...);
        set_property(id.data(), id.size(), property.data(), property.size(), s.data(), s.size());
    }

    export std::string get_property(std::string_view id, std::string_view property) {
        char* ptr = get_property(id.data(), id.size(), property.data(), property.size());
        std::string s{ptr};
        delete_string(ptr);
        return s;
    }
    export int get_property_int(std::string_view id, std::string_view property) {
        return std::stoi(get_property(id, property));
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

    export void add_class(std::string_view id, std::string_view class_name) {
        std::string classes = get_property(id, "classList");
        auto class_set = parse_classes(classes);
        class_set.insert(std::string{class_name});
        set_property(id, "classList", join_classes(class_set));
    }
    export void remove_class(std::string_view id, std::string_view class_name) {
        std::string classes = get_property(id, "classList");
        auto class_set = parse_classes(classes);
        class_set.erase(std::string{class_name});
        set_property(id, "classList", join_classes(class_set));
    }
    export void toggle_class(std::string_view id, std::string_view class_name) {
        std::string classes = get_property(id, "classList");
        auto class_set = parse_classes(classes);
        if(class_set.contains(std::string{class_name})) {
            class_set.erase(std::string{class_name});
        } else {
            class_set.insert(std::string{class_name});
        }
        set_property(id, "classList", join_classes(class_set));
    }
    export bool has_class(std::string_view id, std::string_view class_name) {
        std::string classes = get_property(id, "classList");
        auto class_set = parse_classes(classes);
        return class_set.contains(std::string{class_name});
    }

    export void set_style_property(std::string_view id, std::string_view property, std::string_view value) {
        set_style_property(id.data(), id.size(), property.data(), property.size(), value.data(), value.size());
    }
    export template<class... Args>
    void set_style_property(std::string_view id, std::string_view property, std::format_string<Args...> value, Args&&... args) {
        std::string s = std::format(value, std::forward<Args>(args)...);
        set_style_property(id.data(), id.size(), property.data(), property.size(), s.data(), s.size());
    }

    export void add_element(std::string_view parent, std::string_view tag, std::string_view id) {
        add_element(parent.data(), parent.size(), tag.data(), tag.size(), id.data(), id.size());
    }
    export template<class... Args>
    void add_element(std::string_view parent, std::string_view tag, std::string_view id, std::format_string<Args...> inner_html, Args&&... args) {
        add_element(parent.data(), parent.size(), tag.data(), tag.size(), id.data(), id.size());
        set_html(id, inner_html, std::forward<Args>(args)...);
    }

    export void add_element_html(std::string_view parent, std::string_view html) {
        add_element_html(parent.data(), parent.size(), html.data(), html.size());
    }
    export template<class... Args>
    void add_element_html(std::string_view parent, std::format_string<Args...> html, Args&&... args) {
        std::string s = std::format(html, std::forward<Args>(args)...);
        add_element_html(parent.data(), parent.size(), s.data(), s.size());
    }

    export void remove_element(std::string_view id) {
        remove_element(id.data(), id.size());
    }

    using event_callback = std::function<void(std::string_view)>;
    struct callback_data {
        event_callback callback;
        bool once = false;
    };

    export void add_event_listener(std::string_view id, std::string_view event, event_callback callback, bool once = false, bool prevent_default = false) {
        add_event_listener(id.data(), id.size(), event.data(), event.size(),
            new callback_data{callback, once}, once, prevent_default);
    }

    export void set_timeout(std::chrono::milliseconds duration, event_callback callback) {
        set_timeout(duration.count(), new callback_data{callback, true});
    }

    export void fetch(std::string_view url, event_callback callback) {
        fetch(url.data(), url.size(), new callback_data{callback, true});
    }

    [[clang::export_name("callback")]]
    void callback(void* ptr, char* data, size_t len) {
        callback_data* cb = static_cast<callback_data*>(ptr);
        cb->callback(data ? std::string_view{data, len} : std::string_view{});
        if(cb->once) {
            delete cb;
        }
    }
}
