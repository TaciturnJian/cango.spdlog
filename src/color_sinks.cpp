#include <mutex>

#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>

#ifdef _WIN32
#include <spdlog/sinks/wincolor_sink.h>

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
#include "spdlog/sinks/stdout_color_sinks.h"

namespace spdlog {
    using sync_factory = synchronous_factory;

    /* Use macro to generate template instances for stdout and stderr color loggers
    #define SPDLOG_TEMPLATE_STD_COLOR(name) \
        template std::shared_ptr<logger> name##color_mt<sync_factory>(const std::string &logger_name, color_mode mode); \
        template std::shared_ptr<logger> name##color_mt<async_factory>(const std::string &logger_name, color_mode mode); \
        template std::shared_ptr<logger> name##color_st<sync_factory>(const std::string &logger_name, color_mode mode);\
        template std::shared_ptr<logger> name##color_st<async_factory>(const std::string &logger_name, color_mode mode);\
        // create template instances for stdout and stderr color loggers

    SPDLOG_TEMPLATE_STD_COLOR(stdout_);
    SPDLOG_TEMPLATE_STD_COLOR(stderr_);
    #undef SPDLOG_TEMPLATE_STD_COLOR
    */

    template std::shared_ptr<logger> stdout_color_mt<sync_factory>(const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> stdout_color_mt<async_factory>(const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> stdout_color_st<sync_factory>(const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> stdout_color_st<async_factory>(const std::string &logger_name, color_mode mode);;

    template std::shared_ptr<logger> stderr_color_mt<sync_factory>(const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> stderr_color_mt<async_factory>(const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> stderr_color_st<sync_factory>(const std::string &logger_name, color_mode mode);

    template std::shared_ptr<logger> stderr_color_st<async_factory>(const std::string &logger_name, color_mode mode);;
}
