// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <mutex>

#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
//
// color sinks
//
#ifdef _WIN32
#include <spdlog/sinks/wincolor_sink-inl.h>

namespace spdlog::sinks {
    template class wincolor_sink<details::console_mutex>;
    template class wincolor_sink<details::console_nullmutex>;
    template class wincolor_stdout_sink<details::console_mutex>;
    template class wincolor_stdout_sink<details::console_nullmutex>;
    template class wincolor_stderr_sink<details::console_mutex>;
    template class wincolor_stderr_sink<details::console_nullmutex>;
}
#else
#include "spdlog/sinks/ansicolor_sink-inl.h"

namespace spdlog::sinks {
    template class ansicolor_sink<details::console_mutex>;
    template class ansicolor_sink<details::console_nullmutex>;
    template class ansicolor_stdout_sink<details::console_mutex>;
    template class ansicolor_stdout_sink<details::console_nullmutex>;
    template class ansicolor_stderr_sink<details::console_mutex>;
    template class ansicolor_stderr_sink<details::console_nullmutex>;
}
#endif

// factory methods for color loggers
#include "spdlog/sinks/stdout_color_sinks-inl.h"

namespace spdlog {
    template std::shared_ptr<logger> spdlog::stdout_color_mt<synchronous_factory>(
        const std::string &logger_name,
        color_mode mode);

    template std::shared_ptr<logger> spdlog::stdout_color_st<synchronous_factory>(
        const std::string &logger_name,
        color_mode mode);

    template std::shared_ptr<logger> spdlog::stderr_color_mt<synchronous_factory>(
        const std::string &logger_name,
        color_mode mode);

    template std::shared_ptr<logger> spdlog::stderr_color_st<synchronous_factory>(
        const std::string &logger_name,
        color_mode mode);

    template std::shared_ptr<logger> spdlog::stdout_color_mt<async_factory>(
        const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> spdlog::stdout_color_st<async_factory>(
        const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> spdlog::stderr_color_mt<async_factory>(
        const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> spdlog::stderr_color_st<async_factory>(
        const std::string &logger_name, color_mode mode);
}
