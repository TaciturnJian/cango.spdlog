#include <mutex>

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace spdlog {
    template class sinks::basic_file_sink<std::mutex>;
    template class sinks::basic_file_sink<details::null_mutex>;
}

#include <spdlog/sinks/rotating_file_sink.h>

namespace spdlog::sinks {
    template class rotating_file_sink<std::mutex>;
    template class rotating_file_sink<details::null_mutex>;
}
