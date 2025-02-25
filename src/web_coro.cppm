module;

#include <coroutine>
#include <chrono>
#include <string_view>

export module web_coro;

import web;

namespace web::coro {

export template<typename Return>
struct promise;

export template<typename Return>
struct coroutine : std::coroutine_handle<promise<Return>> {
    using promise_type = promise<Return>;

    ~coroutine() {
        if(this->done() && this->promise().caller) {
            this->destroy();
        }
    }

    template<typename T>
    std::coroutine_handle<> await_suspend(std::coroutine_handle<T> caller) const noexcept {
        this->promise().caller = caller;
        return *this;
    }
    bool await_ready() const noexcept { return false; }

    auto await_resume() {
        if constexpr (std::is_same_v<Return, void>) {
            return;
        } else {
            return this->promise().result;
        }
    }
};

template<typename Return>
struct final_awaiter {
    bool await_ready() const noexcept { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<promise<Return>> handle) const noexcept {
        auto caller = handle.promise().caller;
        if(caller) {
            return caller;
        } else {
            handle.destroy();
            return std::noop_coroutine();
        }
    }
    auto await_resume() const noexcept {}
};

template<typename Return>
struct promise_base {
    std::coroutine_handle<> caller;

    std::suspend_always initial_suspend() noexcept { return {}; }
    final_awaiter<Return> final_suspend() noexcept { return {}; }
    void unhandled_exception() noexcept {}
};

export template<typename Return>
struct promise : promise_base<Return>
{
    Return result{};

    void return_value(Return&& value) {
        result = std::move(value);
    }

    coroutine<Return> get_return_object() {
        return coroutine<Return>{std::coroutine_handle<promise<Return>>::from_promise(*this)};
    }
};

export template<>
struct promise<void> : promise_base<void>
{
    void return_void() {}
    coroutine<void> get_return_object() {
        return coroutine<void>{std::coroutine_handle<promise<void>>::from_promise(*this)};
    }
};

export template<typename... Returns>
[[nodiscard("This is a coroutine, you must either co_await or submit it.")]]
coroutine<void> when_all(coroutine<Returns>&&... coros) {
    (co_await coros, ...);
    co_return;
}

export template<typename Return>
void submit(coroutine<Return>&& coro) {
    coro.resume();
}

export template<typename Return>
void submit_next(coroutine<Return>&& coro) {
    web::set_timeout(std::chrono::milliseconds{0}, [coro = std::move(coro)](std::string_view) {
        coro.resume();
    });
}

export struct timeout {
    std::chrono::milliseconds duration;
    std::coroutine_handle<> handle;

    timeout(std::chrono::milliseconds duration) : duration(duration) {}

    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<> handle) noexcept {
        this->handle = handle;
        web::set_timeout(duration, [handle](std::string_view) {
            handle.resume();
        });
        return true;
    }
    void await_resume() const noexcept {}
};
export auto next_tick() {
    return timeout{std::chrono::milliseconds{0}};
}

export struct event {
    std::string_view id;
    std::string_view event_type;
    std::coroutine_handle<> handle;

    std::string data{};

    event(std::string_view id, std::string_view event_type) : id(id), event_type(event_type) {}

    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<> handle) noexcept {
        this->handle = handle;
        web::add_event_listener(id, event_type, [this](std::string_view data) {
            if(data.size()) {
                this->data = data;
            }
            this->handle.resume();
        }, true);
        return true;
    }
    std::string await_resume() const noexcept {
        return data;
    }
};

export struct fetch {
    std::string_view url;
    std::coroutine_handle<> handle;

    std::string data{};

    fetch(std::string_view url) : url(url) {}

    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<> handle) noexcept {
        this->handle = handle;
        web::fetch(url, [this](std::string_view data) {
            this->data = data;
            this->handle.resume();
        });
        return true;
    }
    std::string await_resume() const noexcept {
        return data;
    }
};

}
