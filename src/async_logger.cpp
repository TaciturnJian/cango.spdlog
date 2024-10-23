#include <spdlog/async_logger.h>
#include <spdlog/details/thread_pool.h>
#include <spdlog/sinks/sink.h>

#include <memory>
#include <string>

namespace spdlog {
    async_logger::async_logger(std::string logger_name,
        sinks_init_list sinks_list,
        std::weak_ptr<details::thread_pool> tp,
        async_overflow_policy overflow_policy) :
        async_logger(std::move(logger_name),
            sinks_list.begin(),
            sinks_list.end(),
            std::move(tp),
            overflow_policy) {}

    async_logger::async_logger(std::string logger_name,
        sink_ptr single_sink,
        std::weak_ptr<details::thread_pool> tp,
        const async_overflow_policy overflow_policy) :
        async_logger(
            std::move(logger_name), {std::move(single_sink)}, std::move(tp), overflow_policy) {}

    // send the log message to the thread pool
    void async_logger::sink_it_(const details::log_msg &msg)
    try {
        if (auto pool_ptr = thread_pool_.lock()) { pool_ptr->post_log(shared_from_this(), msg, overflow_policy_); }
        else { throw_spdlog_ex("async log: thread pool doesn't exist anymore"); }
    }
    SPDLOG_LOGGER_CATCH(msg.source)

    // send flush request to the thread pool
    void async_logger::flush_()
    try {
        auto pool_ptr = thread_pool_.lock();
        if (!pool_ptr) { throw_spdlog_ex("async flush: thread pool doesn't exist anymore"); }

        std::future<void> future = pool_ptr->post_flush(shared_from_this(), overflow_policy_);
        // Wait for the flush operation to complete.
        // This might throw exception if the flush message get dropped because of overflow.
        future.get();
    }
    SPDLOG_LOGGER_CATCH(source_loc())


    //
    // backend functions - called from the thread pool to do the actual job
    //
    void async_logger::backend_sink_it_(const details::log_msg &msg) {
        for (auto &sink: sinks_) {
            if (sink->should_log(msg.level)) {
                try { sink->log(msg); }
                SPDLOG_LOGGER_CATCH(msg.source)
            }
        }

        if (should_flush_(msg)) { backend_flush_(); }
    }

    void async_logger::backend_flush_() {
        for (auto &sink: sinks_) {
            try { sink->flush(); }
            SPDLOG_LOGGER_CATCH(source_loc())
        }
    }

    std::shared_ptr<logger> async_logger::clone(std::string new_name) {
        auto cloned = std::make_shared<async_logger>(*this);
        cloned->name_ = std::move(new_name);
        return cloned;
    }
}
