#include <cango/spdlog/details/log_msg.h>

namespace spdlog::details {
    log_msg::log_msg() = default;

    log_msg::log_msg(
        const log_clock::time_point log_time,
        source_loc loc,
        const string_view_t a_logger_name,
        const level::level_enum lvl,
        const string_view_t msg):
        logger_name(a_logger_name),
        level(lvl),
        time(log_time),
        thread_id(os::thread_id()),
        source(loc),
        payload(msg) {}

    log_msg::log_msg(
        source_loc loc,
        const string_view_t a_logger_name,
        const level::level_enum lvl,
        const string_view_t msg):
        log_msg(os::now(), loc, a_logger_name, lvl, msg) {}

    log_msg::log_msg(
        const string_view_t a_logger_name,
        const level::level_enum lvl,
        const string_view_t msg):
        log_msg(os::now(), source_loc{}, a_logger_name, lvl, msg) {}

    log_msg::log_msg(const log_msg &other) = default;

    log_msg &log_msg::operator=(const log_msg &other) = default;
}
