cmake_minimum_required(VERSION 3.28)

project(cango.utility.spdlog)

string(REPLACE "." "_" module_name ${PROJECT_NAME})
aux_source_directory(src spdlog_sources)
add_library(${module_name} ${spdlog_sources})
add_library(cango::utility::spdlog ALIAS ${module_name})
set_target_properties(${module_name} PROPERTIES CXX_STANDARD 26)
target_include_directories(${module_name} PUBLIC include)
