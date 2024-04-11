#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

void* _GetModuleFunc(const wchar_t* a_moduleName, const char* a_funcName) noexcept;

template <class T>
    requires(std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>)
inline T GetModuleFunc(const wchar_t* a_moduleName, const char* a_funcName) noexcept
{
    return reinterpret_cast<T>(_GetModuleFunc(a_moduleName, a_funcName));
}

// The version of Windows operating system.
struct OsVersion
{
    constexpr OsVersion(std::uint32_t a_major, std::uint32_t a_minor, std::uint32_t a_build) noexcept :
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

    std::uint32_t major;
    std::uint32_t minor;
    std::uint32_t build;
};

std::optional<OsVersion> GetOsVersion() noexcept;
