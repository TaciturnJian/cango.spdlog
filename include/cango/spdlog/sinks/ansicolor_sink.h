#pragma once

#include <array>
#include <memory>
#include <mutex>
#include <string>
#include <cango/spdlog/details/console_globals.h>
#include <cango/spdlog/details/null_mutex.h>
#include <cango/spdlog/sinks/sink.h>
#include <cango/spdlog/pattern_formatter.h>


namespace spdlog::sinks {
    /**
     * This sink prefixes the output with an ANSI escape sequence color code
     * depending on the severity
     * of the message.
     * If no color terminal detected, omit the escape codes.
     */

    template<typename ConsoleMutex>
    class ansicolor_sink : public sink {
    public:
        using mutex_t = typename ConsoleMutex::mutex_t;

        ansicolor_sink(FILE *target_file, color_mode mode);

        ~ansicolor_sink() override = default;

        ansicolor_sink(const ansicolor_sink &other) = delete;

        ansicolor_sink(ansicolor_sink &&other) = delete;

        ansicolor_sink &operator=(const ansicolor_sink &other) = delete;

        ansicolor_sink &operator=(ansicolor_sink &&other) = delete;

        void set_color(level::level_enum color_level, string_view_t color);

        void set_color_mode(color_mode mode);

        bool should_color() const;

        void log(const details::log_msg &msg) override;

        void flush() override;

        void set_pattern(const std::string &pattern) final;

        void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;

        // Formatting codes
        const string_view_t reset = "\033[m";
        const string_view_t bold = "\033[1m";
        const string_view_t dark = "\033[2m";
        const string_view_t underline = "\033[4m";
        const string_view_t blink = "\033[5m";
        const string_view_t reverse = "\033[7m";
        const string_view_t concealed = "\033[8m";
        const string_view_t clear_line = "\033[K";

        // Foreground colors
        const string_view_t black = "\033[30m";
        const string_view_t red = "\033[31m";
        const string_view_t green = "\033[32m";
        const string_view_t yellow = "\033[33m";
        const string_view_t blue = "\033[34m";
        const string_view_t magenta = "\033[35m";
        const string_view_t cyan = "\033[36m";
        const string_view_t white = "\033[37m";

        /// Background colors
        const string_view_t on_black = "\033[40m";
        const string_view_t on_red = "\033[41m";
        const string_view_t on_green = "\033[42m";
        const string_view_t on_yellow = "\033[43m";
        const string_view_t on_blue = "\033[44m";
        const string_view_t on_magenta = "\033[45m";
        const string_view_t on_cyan = "\033[46m";
        const string_view_t on_white = "\033[47m";

        /// Bold colors
        const string_view_t yellow_bold = "\033[33m\033[1m";
        const string_view_t red_bold = "\033[31m\033[1m";
        const string_view_t bold_on_red = "\033[1m\033[41m";

    private:
        FILE *target_file_;
        mutex_t &mutex_;
        bool should_do_colors_;
        std::unique_ptr<spdlog::formatter> formatter_;
        std::array<std::string, level::n_levels> colors_;

        void print_ccode_(const string_view_t &color_code) const;

        void print_range_(const memory_buf_t &formatted, size_t start, size_t end) const;

        static std::string to_string_(const string_view_t &sv);
    };

    template<typename ConsoleMutex>
    class ansicolor_stdout_sink : public ansicolor_sink<ConsoleMutex> {
    public:
        explicit ansicolor_stdout_sink(color_mode mode = color_mode::automatic);
    };

    template<typename ConsoleMutex>
    class ansicolor_stderr_sink : public ansicolor_sink<ConsoleMutex> {
    public:
        explicit ansicolor_stderr_sink(color_mode mode = color_mode::automatic);
    };

    using ansicolor_stdout_sink_mt = ansicolor_stdout_sink<details::console_mutex>;
    using ansicolor_stdout_sink_st = ansicolor_stdout_sink<details::console_nullmutex>;

    using ansicolor_stderr_sink_mt = ansicolor_stderr_sink<details::console_mutex>;
    using ansicolor_stderr_sink_st = ansicolor_stderr_sink<details::console_nullmutex>;

        template<typename ConsoleMutex>
    ansicolor_sink<ConsoleMutex>::ansicolor_sink(FILE *target_file, color_mode mode) :
        target_file_(target_file),
        mutex_(ConsoleMutex::mutex()),
        formatter_(details::make_unique<pattern_formatter>()) {
        set_color_mode(mode);
        colors_.at(level::trace) = to_string_(white);
        colors_.at(level::debug) = to_string_(cyan);
        colors_.at(level::info) = to_string_(green);
        colors_.at(level::warn) = to_string_(yellow_bold);
        colors_.at(level::err) = to_string_(red_bold);
        colors_.at(level::critical) = to_string_(bold_on_red);
        colors_.at(level::off) = to_string_(reset);
    }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::set_color(level::level_enum color_level,
        string_view_t color) {
        std::lock_guard<mutex_t> lock(mutex_);
        colors_.at(static_cast<size_t>(color_level)) = to_string_(color);
    }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::log(const details::log_msg &msg) {
        // Wrap the originally formatted message in color codes.
        // If color is not supported in the terminal, log as is instead.
        std::lock_guard<mutex_t> lock(mutex_);
        msg.color_range_start = 0;
        msg.color_range_end = 0;
        memory_buf_t formatted;
        formatter_->format(msg, formatted);
        if (should_do_colors_ && msg.color_range_end > msg.color_range_start) {
            // before color range
            print_range_(formatted, 0, msg.color_range_start);
            // in color range
            print_ccode_(colors_.at(static_cast<size_t>(msg.level)));
            print_range_(formatted, msg.color_range_start, msg.color_range_end);
            print_ccode_(reset);
            // after color range
            print_range_(formatted, msg.color_range_end, formatted.size());
        }
        else // no color
        {
            print_range_(formatted, 0, formatted.size());
        }
        fflush(target_file_);
    }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::flush() {
        std::lock_guard<mutex_t> lock(mutex_);
        fflush(target_file_);
    }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::set_pattern(const std::string &pattern) {
        std::lock_guard<mutex_t> lock(mutex_);
        formatter_ = std::unique_ptr<formatter>(new pattern_formatter(pattern));
    }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::set_formatter(
        std::unique_ptr<formatter> sink_formatter) {
        std::lock_guard<mutex_t> lock(mutex_);
        formatter_ = std::move(sink_formatter);
    }

    template<typename ConsoleMutex>
    bool ansicolor_sink<ConsoleMutex>::should_color() const { return should_do_colors_; }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::set_color_mode(color_mode mode) {
        switch (mode) {
        case color_mode::always:
            should_do_colors_ = true;
            return;
        case color_mode::automatic:
            should_do_colors_ =
                details::os::in_terminal(target_file_) && details::os::is_color_terminal();
            return;
        case color_mode::never:
            should_do_colors_ = false;
            return;
        default:
            should_do_colors_ = false;
        }
    }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::print_ccode_(const string_view_t &color_code) const {
        fwrite(color_code.data(), sizeof(char), color_code.size(), target_file_);
    }

    template<typename ConsoleMutex>
    void ansicolor_sink<ConsoleMutex>::print_range_(const memory_buf_t &formatted,
        size_t start,
        size_t end) const { fwrite(formatted.data() + start, sizeof(char), end - start, target_file_); }

    template<typename ConsoleMutex>
    std::string ansicolor_sink<ConsoleMutex>::to_string_(const string_view_t &sv) {
        return {sv.data(), sv.size()};
    }

    // ansicolor_stdout_sink
    template<typename ConsoleMutex>
    ansicolor_stdout_sink<ConsoleMutex>::ansicolor_stdout_sink(color_mode mode) :
        ansicolor_sink<ConsoleMutex>(stdout, mode) {}

    // ansicolor_stderr_sink
    template<typename ConsoleMutex>
    ansicolor_stderr_sink<ConsoleMutex>::ansicolor_stderr_sink(color_mode mode) :
        ansicolor_sink<ConsoleMutex>(stderr, mode) {}
}
