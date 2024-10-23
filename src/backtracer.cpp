#include <spdlog/details/backtracer.h>

namespace spdlog::details {
    backtracer::backtracer(const backtracer &other) {
        std::lock_guard lock(other.mutex_);
        enabled_ = other.enabled();
        messages_ = other.messages_;
    }

    backtracer::backtracer(backtracer &&other) noexcept {
        std::lock_guard lock(other.mutex_);
        enabled_ = other.enabled();
        messages_ = std::move(other.messages_);
    }

    backtracer &backtracer::operator=(backtracer other) {
        std::lock_guard lock(mutex_);
        enabled_ = other.enabled();
        messages_ = std::move(other.messages_);
        return *this;
    }

    void backtracer::enable(size_t size) {
        std::lock_guard lock{mutex_};
        enabled_.store(true, std::memory_order_relaxed);
        messages_ = circular_q<log_msg_buffer>{size};
    }

    void backtracer::disable() {
        std::lock_guard lock{mutex_};
        enabled_.store(false, std::memory_order_relaxed);
    }

    bool backtracer::enabled() const { return enabled_.load(std::memory_order_relaxed); }

    void backtracer::push_back(const log_msg &msg) {
        std::lock_guard lock{mutex_};
        messages_.push_back(log_msg_buffer{msg});
    }

    bool backtracer::empty() const {
        std::lock_guard lock{mutex_};
        return messages_.empty();
    }

    // pop all items in the q and apply the given fun on each of them.
    void backtracer::foreach_pop(std::function<void(const details::log_msg &)> fun) {
        std::lock_guard lock{mutex_};
        while (!messages_.empty()) {
            auto &front_msg = messages_.front();
            fun(front_msg);
            messages_.pop_front();
        }
    }
}
