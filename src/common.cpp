#include <algorithm>
#include <iterator>

#include <spdlog/common.h>

namespace spdlog::level {
    static constexpr std::string_view level_string_views[]{
        "trace",
        "debug",
        "info",
        "warning",
        "error",
        "critical",
        "off"
    };

    static const char *short_level_names[]{"T", "D", "I", "W", "E", "C", "O"};

    const string_view_t &to_string_view(level_enum l) noexcept { return level_string_views[l]; }

    const char *to_short_c_str(level_enum l) noexcept { return short_level_names[l]; }

    level_enum from_str(const std::string &name) noexcept {
        auto it = std::find(std::begin(level_string_views), std::end(level_string_views), name);
        if (it != std::end(level_string_views))
            return static_cast<level_enum>(std::distance(
                std::begin(level_string_views), it));

        // check also for "warn" and "err" before giving up..
        if (name == "warn") { return warn; }
        if (name == "err") { return err; }
        return off;
    }
}


namespace spdlog {
    spdlog_ex::spdlog_ex(std::string msg) :
        msg_(std::move(msg)) {}

    spdlog_ex::spdlog_ex(const std::string &msg, int last_errno) {
        #ifdef SPDLOG_USE_STD_FORMAT
        msg_ = std::system_error(std::error_code(last_errno, std::generic_category()), msg).what();
        #else
    memory_buf_t outbuf;
    fmt::format_system_error(outbuf, last_errno, msg.c_str());
    msg_ = fmt::to_string(outbuf);
        #endif
    }

    const char *spdlog_ex::what() const noexcept { return msg_.c_str(); }

    void throw_spdlog_ex(const std::string &msg, const int last_errno) { throw(spdlog_ex(msg, last_errno)); }

    void throw_spdlog_ex(std::string msg) { throw(spdlog_ex(std::move(msg))); }
} // namespace spdlog
