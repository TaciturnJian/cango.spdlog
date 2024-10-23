#include <cstdio>
#include <io.h>     // _get_osfhandle(..)
#include <memory>
#include <mutex>

#include <spdlog/async.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/details/console_globals.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/windows_include.h>
#include <spdlog/sinks/stdout_sinks.h>


namespace spdlog::sinks {
    template<typename ConsoleMutex>
    stdout_sink_base<ConsoleMutex>::stdout_sink_base(FILE *file) :
        mutex_(ConsoleMutex::mutex()),
        file_(file),
        formatter_(details::make_unique<pattern_formatter>()) {
        #ifdef _WIN32
        // get windows handle from the FILE* object

        handle_ = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(file_)));

        // don't throw to support cases where no console is attached,
        // and let the log method to do nothing if (handle_ == INVALID_HANDLE_VALUE).
        // throw only if non stdout/stderr target is requested (probably regular file and not console).
        if (handle_ == INVALID_HANDLE_VALUE && file != stdout && file != stderr) {
            throw_spdlog_ex("spdlog::stdout_sink_base: _get_osfhandle() failed", errno);
        }
        #endif
    }

    template<typename ConsoleMutex>
    void stdout_sink_base<ConsoleMutex>::log(const details::log_msg &msg) {
        #ifdef _WIN32
        if (handle_ == INVALID_HANDLE_VALUE) { return; }
        std::lock_guard<mutex_t> lock(mutex_);
        memory_buf_t formatted;
        formatter_->format(msg, formatted);
        auto size = static_cast<DWORD>(formatted.size());
        DWORD bytes_written = 0;
        bool ok = WriteFile(handle_, formatted.data(), size, &bytes_written, nullptr) != 0;
        if (!ok) {
            throw_spdlog_ex("stdout_sink_base: WriteFile() failed. GetLastError(): " +
                            std::to_string(GetLastError()));
        }
        #else
    std::lock_guard<mutex_t> lock(mutex_);
    memory_buf_t formatted;
    formatter_->format(msg, formatted);
    ::fwrite(formatted.data(), sizeof(char), formatted.size(), file_);
        #endif                // WIN32
        fflush(file_); // flush every line to terminal
    }

    template<typename ConsoleMutex>
    void stdout_sink_base<ConsoleMutex>::flush() {
        std::lock_guard<mutex_t> lock(mutex_);
        fflush(file_);
    }

    template<typename ConsoleMutex>
    void stdout_sink_base<ConsoleMutex>::set_pattern(const std::string &pattern) {
        std::lock_guard<mutex_t> lock(mutex_);
        formatter_ = std::make_unique<pattern_formatter>(pattern);
    }

    template<typename ConsoleMutex>
    void stdout_sink_base<ConsoleMutex>::set_formatter(
        std::unique_ptr<formatter> sink_formatter) {
        std::lock_guard<mutex_t> lock(mutex_);
        formatter_ = std::move(sink_formatter);
    }

    // stdout sink
    template<typename ConsoleMutex>
    stdout_sink<ConsoleMutex>::stdout_sink() :
        stdout_sink_base<ConsoleMutex>(stdout) {}

    // stderr sink
    template<typename ConsoleMutex>
    stderr_sink<ConsoleMutex>::stderr_sink() :
        stdout_sink_base<ConsoleMutex>(stderr) {}
}

namespace spdlog {
    // factory methods
    template<typename Factory>
    std::shared_ptr<logger> stdout_logger_mt(const std::string &logger_name) {
        return Factory::template create<sinks::stdout_sink_mt>(logger_name);
    }

    template<typename Factory>
    std::shared_ptr<logger> stdout_logger_st(const std::string &logger_name) {
        return Factory::template create<sinks::stdout_sink_st>(logger_name);
    }

    template<typename Factory>
    std::shared_ptr<logger> stderr_logger_mt(const std::string &logger_name) {
        return Factory::template create<sinks::stderr_sink_mt>(logger_name);
    }

    template<typename Factory>
    std::shared_ptr<logger> stderr_logger_st(const std::string &logger_name) {
        return Factory::template create<sinks::stderr_sink_st>(logger_name);
    }
}

namespace spdlog::sinks {
    template class stdout_sink_base<details::console_mutex>;
    template class stdout_sink_base<details::console_nullmutex>;
    template class stdout_sink<details::console_mutex>;
    template class stdout_sink<details::console_nullmutex>;
    template class stderr_sink<details::console_mutex>;
    template class stderr_sink<details::console_nullmutex>;
}


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
