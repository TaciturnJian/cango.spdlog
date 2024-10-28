#pragma once

#ifdef _WIN32
#include <cango/spdlog/sinks/wincolor_sink.h>
#else
    #include <cango/spdlog/sinks/ansicolor_sink.h>
#endif

#include <cango/spdlog/details/synchronous_factory.h>

namespace spdlog {
    namespace sinks {
        #ifdef _WIN32
        using stdout_color_sink_mt = wincolor_stdout_sink_mt;
        using stdout_color_sink_st = wincolor_stdout_sink_st;
        using stderr_color_sink_mt = wincolor_stderr_sink_mt;
        using stderr_color_sink_st = wincolor_stderr_sink_st;
        #else
        using stdout_color_sink_mt = ansicolor_stdout_sink_mt;
        using stdout_color_sink_st = ansicolor_stdout_sink_st;
        using stderr_color_sink_mt = ansicolor_stderr_sink_mt;
        using stderr_color_sink_st = ansicolor_stderr_sink_st;
        #endif
    } // namespace sinks

    template<typename Factory = synchronous_factory>
    std::shared_ptr<logger> stdout_color_mt(
        const std::string &logger_name,
        color_mode mode = color_mode::automatic) {
        return Factory::template create<sinks::stdout_color_sink_mt>(logger_name, mode);
    }

    template<typename Factory = synchronous_factory>
    std::shared_ptr<logger> stdout_color_st(
        const std::string &logger_name,
        color_mode mode = color_mode::automatic) {
        return Factory::template create<sinks::stdout_color_sink_st>(logger_name, mode);
    }

    template<typename Factory = synchronous_factory>
    std::shared_ptr<logger> stderr_color_mt(
        const std::string &logger_name,
        color_mode mode = color_mode::automatic) {
        return Factory::template create<sinks::stderr_color_sink_mt>(logger_name, mode);
    }

    template<typename Factory = synchronous_factory>
    std::shared_ptr<logger> stderr_color_st(
        const std::string &logger_name,
        color_mode mode = color_mode::automatic) {
        return Factory::template create<sinks::stderr_color_sink_st>(logger_name, mode);
    }
}
