// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// ReSharper disable CppUnusedIncludeDirective
#include <spdlog/common-inl.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/details/registry-inl.h>
#include <spdlog/sinks/base_sink-inl.h>
#include <spdlog/sinks/sink-inl.h>
#include <spdlog/spdlog-inl.h>

#include <mutex>

// template instantiate logger constructor with sinks init list
template  spdlog::logger::logger(std::string name,
                                           sinks_init_list::iterator begin,
                                           sinks_init_list::iterator end);
template class  spdlog::sinks::base_sink<std::mutex>;
template class  spdlog::sinks::base_sink<spdlog::details::null_mutex>;
