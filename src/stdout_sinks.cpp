#include <memory>

#include <cango/spdlog/async.h>
#include <cango/spdlog/pattern_formatter.h>
#include <cango/spdlog/sinks/stdout_sinks.h>

namespace spdlog {
    using sync_factory = synchronous_factory;

    template std::shared_ptr<logger> spdlog::stdout_logger_mt<sync_factory>(const std::string &logger_name);

    template std::shared_ptr<logger> spdlog::stdout_logger_st<sync_factory>(const std::string &logger_name);

    template std::shared_ptr<logger> spdlog::stderr_logger_mt<sync_factory>(const std::string &logger_name);

    template std::shared_ptr<logger> spdlog::stderr_logger_st<sync_factory>(const std::string &logger_name);

    template std::shared_ptr<logger> spdlog::stdout_logger_mt<async_factory>(const std::string &logger_name);

    template std::shared_ptr<logger> spdlog::stdout_logger_st<async_factory>(const std::string &logger_name);

    template std::shared_ptr<logger> spdlog::stderr_logger_mt<async_factory>(const std::string &logger_name);

    template std::shared_ptr<logger> spdlog::stderr_logger_st<async_factory>(const std::string &logger_name);
}
