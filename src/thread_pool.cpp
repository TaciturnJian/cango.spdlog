#include <cassert>
#include <spdlog/async.h>
#include <spdlog/common.h>
#include <spdlog/details/thread_pool.h>

namespace spdlog::details {
    thread_pool::thread_pool(size_t q_max_items,
        size_t threads_n,
        std::function<void()> on_thread_start,
        std::function<void()> on_thread_stop) :
        q_(q_max_items) {
        if (threads_n == 0 || threads_n > 1000) {
            throw_spdlog_ex(
                "spdlog::thread_pool(): invalid threads_n param (valid "
                "range is 1-1000)");
        }
        for (size_t i = 0; i < threads_n; i++) {
            threads_.emplace_back([this, on_thread_start, on_thread_stop] {
                on_thread_start();
                this->thread_pool::worker_loop_();
                on_thread_stop();
            });
        }
    }

    thread_pool::thread_pool(size_t q_max_items,
        size_t threads_n,
        std::function<void()> on_thread_start) :
        thread_pool(q_max_items, threads_n, on_thread_start, [] {}) {}

    thread_pool::thread_pool(size_t q_max_items, size_t threads_n) :
        thread_pool(
            q_max_items, threads_n, [] {}, [] {}) {}

    // message all threads to terminate gracefully join them
    thread_pool::~thread_pool() {
        SPDLOG_TRY {
            for (size_t i = 0; i < threads_.size(); i++) {
                post_async_msg_(async_msg(async_msg_type::terminate), async_overflow_policy::block);
            }

            for (auto &t: threads_) { t.join(); }
        }
        SPDLOG_CATCH_STD
    }

    void thread_pool::post_log(async_logger_ptr &&worker_ptr,
        const details::log_msg &msg,
        async_overflow_policy overflow_policy) {
        async_msg async_m(std::move(worker_ptr), async_msg_type::log, msg);
        post_async_msg_(std::move(async_m), overflow_policy);
    }

    std::future<void> thread_pool::post_flush(async_logger_ptr &&worker_ptr,
        async_overflow_policy overflow_policy) {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();
        post_async_msg_(async_msg(std::move(worker_ptr), async_msg_type::flush, std::move(promise)),
            overflow_policy);
        return future;
    }

    size_t thread_pool::overrun_counter() { return q_.overrun_counter(); }

    void thread_pool::reset_overrun_counter() { q_.reset_overrun_counter(); }

    size_t thread_pool::discard_counter() { return q_.discard_counter(); }

    void thread_pool::reset_discard_counter() { q_.reset_discard_counter(); }

    size_t thread_pool::queue_size() { return q_.size(); }

    void thread_pool::post_async_msg_(async_msg &&new_msg,
        async_overflow_policy overflow_policy) {
        if (overflow_policy == async_overflow_policy::block) { q_.enqueue(std::move(new_msg)); }
        else if (overflow_policy == async_overflow_policy::overrun_oldest) { q_.enqueue_nowait(std::move(new_msg)); }
        else {
            assert(overflow_policy == async_overflow_policy::discard_new);
            q_.enqueue_if_have_room(std::move(new_msg));
        }
    }

    void thread_pool::worker_loop_() { while (process_next_msg_()) {} }

    // process next message in the queue
    // return true if this thread should still be active (while no terminate msg
    // was received)
    bool thread_pool::process_next_msg_() {
        async_msg incoming_async_msg;
        q_.dequeue(incoming_async_msg);

        switch (incoming_async_msg.msg_type) {
        case async_msg_type::log: {
            incoming_async_msg.worker_ptr->backend_sink_it_(incoming_async_msg);
            return true;
        }
        case async_msg_type::flush: {
            incoming_async_msg.worker_ptr->backend_flush_();
            incoming_async_msg.flush_promise.set_value();
            return true;
        }

        case async_msg_type::terminate: { return false; }

        default: { assert(false); }
        }

        return true;
    }
}
