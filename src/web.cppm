module;

#include <chrono>
#include <format>
#include <functional>
#include <string_view>

export module web;

namespace web {
    [[clang::import_name("eval")]] void eval(const char*, size_t);
    [[clang::import_name("set_html")]] void set_html(const char*, size_t, const char*, size_t);
    [[clang::import_name("log")]] void log(const char*, size_t);
    [[clang::import_name("add_event_listener")]] void add_event_listener(const char*, size_t, const char*, size_t, void*);
    [[clang::import_name("set_timeout")]] void set_timeout(unsigned long, void*);

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

    void log(std::string_view message) {
        log(message.data(), message.size());
    }
    export template<class... Args>
    void log(std::format_string<Args...> fmt, Args&&... args) {
        std::string s = std::format(fmt, std::forward<Args>(args)...);
        log(s.data(), s.size());
    }

    using event_callback = std::function<void()>;
    struct callback_data {
        event_callback callback;
        bool once = false;
    };

    export void add_event_listener(std::string_view id, std::string_view event, event_callback callback) {
        callback_data* data = new callback_data{callback};
        add_event_listener(id.data(), id.size(), event.data(), event.size(), data);
    }

    export void set_timeout(std::chrono::milliseconds duration, event_callback callback) {
        callback_data* data = new callback_data{callback, true};
        set_timeout(duration.count(), data);
    }

    [[clang::export_name("callback")]]
    void callback(void* ptr) {
        callback_data* data = static_cast<callback_data*>(ptr);
        data->callback();
        if(data->once) {
            delete data;
        }
    }
}
