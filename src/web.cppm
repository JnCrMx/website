module;

#include <chrono>
#include <format>
#include <functional>
#include <string_view>

export module web;

namespace web {
    [[clang::import_name("eval")]] void eval(const char*, size_t);
    [[clang::import_name("set_html")]] void set_html(const char*, size_t, const char*, size_t);
    [[clang::import_name("set_property")]] void set_property(const char*, size_t, const char*, size_t, const char*, size_t);
    [[clang::import_name("get_property")]] char* get_property(const char*, size_t, const char*, size_t);
    [[clang::import_name("log")]] void log(const char*, size_t);
    [[clang::import_name("add_event_listener")]] void add_event_listener(const char*, size_t, const char*, size_t, void*, bool);
    [[clang::import_name("set_timeout")]] void set_timeout(unsigned long, void*);

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

    export void eval(std::string_view code) {
        eval(code.data(), code.size());
    }
    export template<class... Args>
    void eval(std::format_string<Args...> code, Args&&... args) {
        std::string s = std::format(code, std::forward<Args>(args)...);
        eval(s.data(), s.size());
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

    using event_callback = std::function<void(std::string_view)>;
    struct callback_data {
        event_callback callback;
        bool once = false;
    };

    export void add_event_listener(std::string_view id, std::string_view event, event_callback callback, bool once = false) {
        callback_data* data = new callback_data{callback, once};
        add_event_listener(id.data(), id.size(), event.data(), event.size(), data, once);
    }

    export void set_timeout(std::chrono::milliseconds duration, event_callback callback) {
        callback_data* data = new callback_data{callback, true};
        set_timeout(duration.count(), data);
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
