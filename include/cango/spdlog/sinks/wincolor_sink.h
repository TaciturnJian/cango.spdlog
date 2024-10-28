#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#include <cango/spdlog/common.h>
#include <cango/spdlog/pattern_formatter.h>
#include <cango/spdlog/details/console_globals.h>
#include <cango/spdlog/details/windows_include.h>
#include <cango/spdlog/sinks/sink.h>

namespace spdlog::sinks {
    /*
     * Windows color console sink. Uses WriteConsoleA to write to the console with
     * colors
     */
    template<typename ConsoleMutex>
    class wincolor_sink : public sink {
    public:
        wincolor_sink(void *out_handle, color_mode mode);

        ~wincolor_sink() override;

        wincolor_sink(const wincolor_sink &other) = delete;

        wincolor_sink &operator=(const wincolor_sink &other) = delete;

        // change the color for the given level
        void set_color(level::level_enum level, std::uint16_t color);

        void log(const details::log_msg &msg) final;

        void flush() final;

        void set_pattern(const std::string &pattern) final;

        void set_formatter(std::unique_ptr<formatter> sink_formatter) final;

        void set_color_mode(color_mode mode);

    protected:
        using mutex_t = typename ConsoleMutex::mutex_t;
        void *out_handle_;
        mutex_t &mutex_;
        bool should_do_colors_{};
        std::unique_ptr<formatter> formatter_;
        std::array<std::uint16_t, level::n_levels> colors_{};

        // set foreground color and return the orig console attributes (for resetting later)
        std::uint16_t set_foreground_color_(std::uint16_t attribs) const;

        // print a range of formatted message to console
        void print_range_(const memory_buf_t &formatted, size_t start, size_t end) const;

        // in case we are redirected to file (not in console mode)
        void write_to_file_(const memory_buf_t &formatted) const;

        void set_color_mode_impl(color_mode mode);
    };

    template<typename ConsoleMutex>
    class wincolor_stdout_sink final : public wincolor_sink<ConsoleMutex> {
    public:
        explicit wincolor_stdout_sink(color_mode mode = color_mode::automatic);
    };

    template<typename ConsoleMutex>
    class wincolor_stderr_sink final : public wincolor_sink<ConsoleMutex> {
    public:
        explicit wincolor_stderr_sink(color_mode mode = color_mode::automatic);
    };

    using wincolor_stdout_sink_mt = wincolor_stdout_sink<details::console_mutex>;
    using wincolor_stdout_sink_st = wincolor_stdout_sink<details::console_nullmutex>;

    using wincolor_stderr_sink_mt = wincolor_stderr_sink<details::console_mutex>;
    using wincolor_stderr_sink_st = wincolor_stderr_sink<details::console_nullmutex>;

    template<typename ConsoleMutex>
    wincolor_sink<ConsoleMutex>::wincolor_sink(void *out_handle, const color_mode mode) :
        out_handle_(out_handle),
        mutex_(ConsoleMutex::mutex()),
        formatter_(details::make_unique<pattern_formatter>()) {
        set_color_mode_impl(mode);
        // set level colors

        constexpr auto green = FOREGROUND_GREEN;
        constexpr auto cyan =  green | FOREGROUND_BLUE;
        constexpr auto white = FOREGROUND_RED | cyan;
        constexpr auto yellow = FOREGROUND_RED | green;
        constexpr auto intense_yellow = yellow | FOREGROUND_INTENSITY;
        constexpr auto intense_red = FOREGROUND_RED | FOREGROUND_INTENSITY;
        constexpr auto intense_white_foreground_red_background = BACKGROUND_RED | white | FOREGROUND_INTENSITY;

        colors_[level::trace] = white;
        colors_[level::debug] = cyan;
        colors_[level::info] = green;
        colors_[level::warn] = intense_yellow;
        colors_[level::err] = intense_red;
        colors_[level::critical] = intense_white_foreground_red_background;
        colors_[level::off] = 0;
    }

    template<typename ConsoleMutex>
    wincolor_sink<ConsoleMutex>::~wincolor_sink() { this->flush(); }

    // change the color for the given level
    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::set_color(level::level_enum level,
        const std::uint16_t color) {
        std::lock_guard<mutex_t> lock(mutex_);
        colors_[static_cast<size_t>(level)] = color;
    }

    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::log(const details::log_msg &msg) {
        if (out_handle_ == nullptr || out_handle_ == INVALID_HANDLE_VALUE) { return; }

        std::lock_guard<mutex_t> lock(mutex_);
        msg.color_range_start = 0;
        msg.color_range_end = 0;
        memory_buf_t formatted;
        formatter_->format(msg, formatted);
        if (should_do_colors_ && msg.color_range_end > msg.color_range_start) {
            // before color range
            print_range_(formatted, 0, msg.color_range_start);
            // in color range
            auto orig_attribs =
                static_cast<WORD>(set_foreground_color_(colors_[static_cast<size_t>(msg.level)]));
            print_range_(formatted, msg.color_range_start, msg.color_range_end);
            // reset to orig colors
            SetConsoleTextAttribute(static_cast<HANDLE>(out_handle_), orig_attribs);
            print_range_(formatted, msg.color_range_end, formatted.size());
        }
        else // print without colors if color range is invalid (or color is disabled)
        {
            write_to_file_(formatted);
        }
    }

    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::flush() {
        // windows console always flushed?
    }

    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::set_pattern(const std::string &pattern) {
        std::lock_guard<mutex_t> lock(mutex_);
        formatter_ = std::make_unique<pattern_formatter>(pattern);
    }

    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::set_formatter(std::unique_ptr<formatter> sink_formatter) {
        std::lock_guard<mutex_t> lock(mutex_);
        formatter_ = std::move(sink_formatter);
    }

    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::set_color_mode(color_mode mode) {
        std::lock_guard<mutex_t> lock(mutex_);
        set_color_mode_impl(mode);
    }

    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::set_color_mode_impl(color_mode mode) {
        if (mode == color_mode::automatic) {
            // should do colors only if out_handle_  points to actual console.
            DWORD console_mode;
            bool in_console = GetConsoleMode(static_cast<HANDLE>(out_handle_), &console_mode) != 0;
            should_do_colors_ = in_console;
        }
        else { should_do_colors_ = mode == color_mode::always ? true : false; }
    }

    // set foreground color and return the orig console attributes (for resetting later)
    template<typename ConsoleMutex>
    std::uint16_t wincolor_sink<ConsoleMutex>::set_foreground_color_(std::uint16_t attribs) const {
        CONSOLE_SCREEN_BUFFER_INFO orig_buffer_info;
        if (!GetConsoleScreenBufferInfo(static_cast<HANDLE>(out_handle_), &orig_buffer_info)) {
            // just return white if failed getting console info
            return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }

        // change only the foreground bits (lowest 4 bits)
        auto new_attribs = static_cast<WORD>(attribs) | (orig_buffer_info.wAttributes & 0xfff0);
        auto ignored =
            SetConsoleTextAttribute(static_cast<HANDLE>(out_handle_), static_cast<WORD>(new_attribs));
        (void) (ignored);
        return static_cast<std::uint16_t>(orig_buffer_info.wAttributes); // return orig attribs
    }

    // print a range of formatted message to console
    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::print_range_(const memory_buf_t &formatted,
        size_t start,
        size_t end) const {
        if (end > start) {
            auto size = static_cast<DWORD>(end - start);
            auto ignored = WriteConsoleA(static_cast<HANDLE>(out_handle_), formatted.data() + start,
                size, nullptr, nullptr);
            (void) (ignored);
        }
    }

    template<typename ConsoleMutex>
    void wincolor_sink<ConsoleMutex>::write_to_file_(const memory_buf_t &formatted) const {
        const auto size = static_cast<DWORD>(formatted.size());
        DWORD bytes_written = 0;
        auto ignored = WriteFile(static_cast<HANDLE>(out_handle_), formatted.data(), size,
            &bytes_written, nullptr);
        (void) (ignored);
    }

    // wincolor_stdout_sink
    template<typename ConsoleMutex>
    wincolor_stdout_sink<ConsoleMutex>::wincolor_stdout_sink(color_mode mode) :
        wincolor_sink<ConsoleMutex>(GetStdHandle(STD_OUTPUT_HANDLE), mode) {}

    // wincolor_stderr_sink
    template<typename ConsoleMutex>
    wincolor_stderr_sink<ConsoleMutex>::wincolor_stderr_sink(color_mode mode) :
        wincolor_sink<ConsoleMutex>(GetStdHandle(STD_ERROR_HANDLE), mode) {}
}

