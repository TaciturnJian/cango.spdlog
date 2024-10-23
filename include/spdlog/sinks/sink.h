#pragma once

#include <spdlog/details/log_msg.h>
#include <spdlog/formatter.h>

namespace spdlog::sinks {
    class sink {
    public:
        virtual ~sink() = default;

        virtual void log(const details::log_msg &msg) = 0;

        virtual void flush() = 0;

        virtual void set_pattern(const std::string &pattern) = 0;

        virtual void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) = 0;

        void set_level(const level::level_enum log_level) { level_.store(log_level, std::memory_order_relaxed); }

        level::level_enum level() const {
            return static_cast<level::level_enum>(level_.load(std::memory_order_relaxed));
        }

        bool should_log(level::level_enum msg_level) const {
            return msg_level >= level_.load(std::memory_order_relaxed);
        }

    protected:
        // sink log level - default is all
        level_t level_{level::trace};
    };
}
