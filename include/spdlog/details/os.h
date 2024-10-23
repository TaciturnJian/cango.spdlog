

#pragma once

#include <ctime>
#include <spdlog/common.h>

namespace spdlog::details::os {
    inline log_clock::time_point now() noexcept { return log_clock::now(); }

    inline std::tm localtime(const std::time_t &time_tt) noexcept {
        #ifdef _WIN32
        std::tm tm{};
        ::localtime_s(&tm, &time_tt);
        #else
        std::tm tm{};
        ::localtime_r(&time_tt, &tm);
        #endif
        return tm;
    }

    inline std::tm localtime() noexcept {
        return localtime(time(nullptr));
    }

    inline std::tm gmtime(const std::time_t &time_tt) noexcept {
        #ifdef _WIN32
        std::tm tm{};
        ::gmtime_s(&tm, &time_tt);
        #else
        std::tm tm{};
        ::gmtime_r(&time_tt, &tm);
        #endif
        return tm;
    }

    inline std::tm gmtime() noexcept {
        return gmtime(time(nullptr));
    }

    // eol definition
    #if !defined(SPDLOG_EOL)
    #ifdef _WIN32
    #define SPDLOG_EOL "\r\n"
    #else
        #define SPDLOG_EOL "\n"
    #endif
    #endif

    SPDLOG_CONSTEXPR static const char *default_eol = SPDLOG_EOL;

    // folder separator
    #if !defined(SPDLOG_FOLDER_SEPS)
    #ifdef _WIN32
    #define SPDLOG_FOLDER_SEPS "\\/"
    #else
        #define SPDLOG_FOLDER_SEPS "/"
    #endif
    #endif

    SPDLOG_CONSTEXPR static const char folder_seps[] = SPDLOG_FOLDER_SEPS;
    SPDLOG_CONSTEXPR static const filename_t::value_type folder_seps_filename[] =
        SPDLOG_FILENAME_T(SPDLOG_FOLDER_SEPS);

    // fopen_s on non windows for writing
    bool fopen_s(FILE **fp, const filename_t &filename, const filename_t &mode);

    // Remove filename. return 0 on success
    int remove(const filename_t &filename) noexcept;

    // Remove file if exists. return 0 on success
    // Note: Non atomic (might return failure to delete if concurrently deleted by other process/thread)
    int remove_if_exists(const filename_t &filename) noexcept;

    int rename(const filename_t &filename1, const filename_t &filename2) noexcept;

    // Return if file exists.
    bool path_exists(const filename_t &filename) noexcept;

    // Return file size according to open FILE* object
    size_t filesize(FILE *f);

    // Return utc offset in minutes or throw spdlog_ex on failure
    int utc_minutes_offset(const std::tm &tm = details::os::localtime());

    // Return current thread id as size_t
    // It exists because the std::this_thread::get_id() is much slower(especially
    // under VS 2013)
    size_t _thread_id() noexcept;

    // Return current thread id as size_t (from thread local storage)
    size_t thread_id() noexcept;

    // This is avoid msvc issue in sleep_for that happens if the clock changes.
    // See https://github.com/gabime/spdlog/issues/609
    void sleep_for_millis(unsigned int milliseconds) noexcept;

    std::string filename_to_str(const filename_t &filename);

    int pid() noexcept;

    // Determine if the terminal supports colors
    // Source: https://github.com/agauniyal/rang/
    bool is_color_terminal() noexcept;

    // Determine if the terminal attached
    // Source: https://github.com/agauniyal/rang/
    bool in_terminal(FILE *file) noexcept;

    #if (defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT) || defined(SPDLOG_WCHAR_FILENAMES)) && defined(_WIN32)
void wstr_to_utf8buf(wstring_view_t wstr, memory_buf_t &target);

void utf8_to_wstrbuf(string_view_t str, wmemory_buf_t &target);
    #endif

    // Return directory name from given path or empty string
    // "abc/file" => "abc"
    // "abc/" => "abc"
    // "abc" => ""
    // "abc///" => "abc//"
    filename_t dir_name(const filename_t &path);

    // Create a dir from the given path.
    // Return true if succeeded or if this dir already exists.
    bool create_dir(const filename_t &path);

    // non thread safe, cross platform getenv/getenv_s
    // return empty string if field not found
    std::string getenv(const char *field);

    // Do fsync by FILE objectpointer.
    // Return true on success.
    bool fsync(FILE *fp);
} // namespace spdlog::details::os


#ifdef SPDLOG_HEADER_ONLY
    #include "os-inl.h"
#endif
