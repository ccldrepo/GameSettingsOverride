#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>

// The version of Windows operating system.
struct OsVersion
{
    uint32_t major;
    uint32_t minor;
    uint32_t build;

    constexpr OsVersion(uint32_t a_major, uint32_t a_minor, uint32_t a_build) noexcept :
        major(a_major), minor(a_minor), build(a_build)
    {}

    std::string string(std::string_view a_sep) const
    {
        return std::format("{}{}{}{}{}", major, a_sep, minor, a_sep, build);
    }

    std::wstring wstring(std::wstring_view a_sep) const
    {
        return std::format(L"{}{}{}{}{}", major, a_sep, minor, a_sep, build);
    }
};

std::optional<OsVersion> GetOsVersion() noexcept;
