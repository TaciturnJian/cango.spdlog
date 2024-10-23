// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
#pragma once

#include <charconv>
#include <chrono>
#include <concepts>
#include <iterator>
#include <limits>
#include <type_traits>
#include <spdlog/common.h>
#include <spdlog/fmt/fmt.h>

// Some fmt helpers to efficiently format and pad ints and strings
namespace spdlog::details::fmt_helper {
    inline void append_string_view(string_view_t view, memory_buf_t &dest) {
        auto *buf_ptr = view.data();
        dest.append(buf_ptr, buf_ptr + view.size());
    }

    template<typename T>
    void append_int(T n, memory_buf_t &dest) {
        // Buffer should be large enough to hold all digits (digits10 + 1) and a sign
        constexpr auto BUF_SIZE = std::numeric_limits<T>::digits10 + 2;
        char buf[BUF_SIZE];

        auto [ptr, ec] = std::to_chars(buf, buf + BUF_SIZE, n, 10);
        if (ec == std::errc()) { dest.append(buf, ptr); }
        else { throw_spdlog_ex("Failed to format int", static_cast<int>(ec)); }
    }

    /// @brief unfold the default solution to count the number of digits in an integer
    template<std::integral number_type>
    constexpr unsigned int count_digits_fallback(number_type number) {
        // taken from fmt: https://github.com/fmtlib/fmt/blob/8.0.1/include/fmt/format.h#L899-L912
        std::uint32_t count = 1;
        for (;;) {
            // Integer division is slow so do it for a group of four digits instead
            // of for every digit. The idea comes from the talk by Alexandrescu
            // "Three Optimization Tips for C++". See speed-test for a comparison.
            if (number < 10) return count;
            if (number < 100) return count + 1;
            if (number < 1000) return count + 2;
            if (number < 10000) return count + 3;
            number /= 10000u;
            count += 4;
        }
    }

    template<std::integral number_type>
    unsigned int count_digits(number_type number) {
        using count_type = std::conditional_t<(sizeof(number_type) > sizeof(uint32_t)), uint64_t, uint32_t>;
        return count_digits_fallback(static_cast<count_type>(number));
    }

    inline void pad2(int n, memory_buf_t &dest) {
        if (n >= 0 && n < 100) // 0-99
        {
            dest.push_back(static_cast<char>('0' + n / 10));
            dest.push_back(static_cast<char>('0' + n % 10));
        }
        else // unlikely, but just in case, let fmt deal with it
        {
            std::format_to(std::back_inserter(dest), "{:02}", n);
        }
    }

    template<typename T>
    void pad_uint(T n, unsigned int width, memory_buf_t &dest) {
        static_assert(std::is_unsigned<T>::value, "pad_uint must get unsigned T");
        for (auto digits = count_digits(n); digits < width; digits++) { dest.push_back('0'); }
        append_int(n, dest);
    }

    template<typename T>
    void pad3(T n, memory_buf_t &dest) {
        static_assert(std::is_unsigned<T>::value, "pad3 must get unsigned T");
        if (n < 1000) {
            dest.push_back(static_cast<char>(n / 100 + '0'));
            n = n % 100;
            dest.push_back(static_cast<char>((n / 10) + '0'));
            dest.push_back(static_cast<char>((n % 10) + '0'));
        }
        else { append_int(n, dest); }
    }

    template<typename T>
    void pad6(T n, memory_buf_t &dest) { pad_uint(n, 6, dest); }

    template<typename T>
    void pad9(T n, memory_buf_t &dest) { pad_uint(n, 9, dest); }

    // return fraction of a second of the given time_point.
    // e.g.
    // fraction<std::milliseconds>(tp) -> will return the millis part of the second
    template<typename ToDuration>
    inline ToDuration time_fraction(log_clock::time_point tp) {
        using std::chrono::duration_cast;
        using std::chrono::seconds;
        const auto duration = tp.time_since_epoch();
        const auto secs = duration_cast<seconds>(duration);
        return duration_cast<ToDuration>(duration) - duration_cast<ToDuration>(secs);
    }
}
