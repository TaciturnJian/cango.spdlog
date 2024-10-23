// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <mutex>

#include <spdlog/async.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/stdout_sinks-inl.h>

template class  spdlog::sinks::stdout_sink_base<spdlog::details::console_mutex>;
template class  spdlog::sinks::stdout_sink_base<spdlog::details::console_nullmutex>;
template class  spdlog::sinks::stdout_sink<spdlog::details::console_mutex>;
template class  spdlog::sinks::stdout_sink<spdlog::details::console_nullmutex>;
template class  spdlog::sinks::stderr_sink<spdlog::details::console_mutex>;
template class  spdlog::sinks::stderr_sink<spdlog::details::console_nullmutex>;

template  std::shared_ptr<spdlog::logger>
spdlog::stdout_logger_mt<spdlog::synchronous_factory>(const std::string &logger_name);
template  std::shared_ptr<spdlog::logger>
spdlog::stdout_logger_st<spdlog::synchronous_factory>(const std::string &logger_name);
template  std::shared_ptr<spdlog::logger>
spdlog::stderr_logger_mt<spdlog::synchronous_factory>(const std::string &logger_name);
template  std::shared_ptr<spdlog::logger>
spdlog::stderr_logger_st<spdlog::synchronous_factory>(const std::string &logger_name);

template  std::shared_ptr<spdlog::logger> spdlog::stdout_logger_mt<spdlog::async_factory>(
    const std::string &logger_name);
template  std::shared_ptr<spdlog::logger> spdlog::stdout_logger_st<spdlog::async_factory>(
    const std::string &logger_name);
template  std::shared_ptr<spdlog::logger> spdlog::stderr_logger_mt<spdlog::async_factory>(
    const std::string &logger_name);
template  std::shared_ptr<spdlog::logger> spdlog::stderr_logger_st<spdlog::async_factory>(
    const std::string &logger_name);
