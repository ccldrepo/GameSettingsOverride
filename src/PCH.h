#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <climits>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <execution>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <ios>
#include <istream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <ranges>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include <version>

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <REX/PS4.h>
#include <REX/W32.h>
#include <SKSE/SKSE.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <spdlog/spdlog.h>

using namespace std::literals::string_view_literals;

namespace SKSE::stl
{
    class log_message
    {
    public:
        explicit log_message(std::string a_msg) : msg(std::move(a_msg)) {}

        const std::string& string() const noexcept { return msg; }
        const char*        c_str() const noexcept { return msg.c_str(); }
        std::size_t        size() const noexcept { return msg.size(); }

    private:
        std::string msg;
    };

    inline void log_success(std::string a_msg, bool a_throw,
        std::source_location a_loc = std::source_location::current())
    {
        spdlog::log(spdlog::source_loc{ a_loc.file_name(), static_cast<int>(a_loc.line()), a_loc.function_name() },
            spdlog::level::info, a_msg);
        if (a_throw) {
            throw SKSE::stl::log_message(std::move(a_msg));
        }
    }

    [[noreturn]] inline void log_failure(std::string a_msg, bool a_throw,
        std::source_location a_loc = std::source_location::current())
    {
        if (!a_throw) {
            SKSE::stl::report_and_fail(a_msg, a_loc);
        } else {
            spdlog::log(spdlog::source_loc{ a_loc.file_name(), static_cast<int>(a_loc.line()), a_loc.function_name() },
                spdlog::level::err, a_msg);
            throw SKSE::stl::log_message(std::move(a_msg));
        }
    }
}

inline std::filesystem::path StrToPath(std::string_view a_str)
{
    auto wstr = SKSE::stl::utf8_to_utf16(a_str);
    if (!wstr) {
        return {};
    }
    return std::filesystem::path{ *std::move(wstr) };
}

inline std::string PathToStr(const std::filesystem::path& a_path)
{
    auto str = SKSE::stl::utf16_to_utf8(a_path.native());
    if (!str) {
        return {};
    }
    return *std::move(str);
}
