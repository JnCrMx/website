module;

#include <coroutine>
#include <chrono>

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
        }
        return this->promise().result;
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

export template<typename Return>
void submit(coroutine<Return>&& coro) {
    coro.resume();
}

export struct timeout {
    std::chrono::milliseconds duration;
    std::coroutine_handle<> handle;

    timeout(std::chrono::milliseconds duration) : duration(duration) {}

    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<promise<void>> handle) noexcept {
        this->handle = handle;
        web::set_timeout(duration, [handle]() {
            handle.resume();
        });
        return true;
    }
    void await_resume() const noexcept {}
};

export struct event {
    std::string_view id;
    std::string_view event_type;
    std::coroutine_handle<> handle;

    event(std::string_view id, std::string_view event_type) : id(id), event_type(event_type) {}

    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<promise<void>> handle) noexcept {
        this->handle = handle;
        web::add_event_listener(id, event_type, [handle]() {
            handle.resume();
        }, true);
        return true;
    }
    void await_resume() const noexcept {}
};

}
