// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink-inl.h>
#include <spdlog/sinks/basic_file_sink-inl.h>

#include <mutex>

namespace spdlog {
    template class sinks::basic_file_sink<std::mutex>;
    template class sinks::basic_file_sink<details::null_mutex>;
}

#include <spdlog/sinks/rotating_file_sink-inl.h>

namespace spdlog::sinks {
    template class rotating_file_sink<std::mutex>;
    template class rotating_file_sink<details::null_mutex>;
}
