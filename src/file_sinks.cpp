#include <mutex>

#include <cango/spdlog/details/null_mutex.h>
#include <cango/spdlog/sinks/basic_file_sink.h>

namespace spdlog {
    template class sinks::basic_file_sink<std::mutex>;
    template class sinks::basic_file_sink<details::null_mutex>;
}
