cmake_minimum_required(VERSION 3.28)

project(cango_utility_third_party_spdlog)

aux_source_directory(src spdlog_sources)
add_library(cango_utility_third_party_spdlog ${spdlog_sources})
add_library(cango::utility::third_party::spdlog ALIAS cango_utility_third_party_spdlog)
set_target_properties(cango_utility_third_party_spdlog PROPERTIES CXX_STANDARD 26)
target_include_directories(cango_utility_third_party_spdlog PUBLIC include)
