

#pragma once

#include <spdlog/common.h>
#include <spdlog/details/os.h>

namespace spdlog::details {
    struct log_msg {
        log_msg();

        log_msg(log_clock::time_point log_time,
            source_loc loc,
            string_view_t a_logger_name,
            level::level_enum lvl,
            string_view_t msg);

        log_msg(source_loc loc,
            string_view_t a_logger_name,
            level::level_enum lvl,
            string_view_t msg);

        log_msg(string_view_t a_logger_name,
            level::level_enum lvl,
            string_view_t msg);

        log_msg(const log_msg &other);

        log_msg &operator=(const log_msg &other);

        string_view_t logger_name;
        level::level_enum level{level::off};
        log_clock::time_point time;
        size_t thread_id{0};

        // wrapping the formatted text with color (updated by pattern_formatter).
        mutable size_t color_range_start{0};
        mutable size_t color_range_end{0};

        source_loc source;
        string_view_t payload;
    };
} // namespace spdlog::details
