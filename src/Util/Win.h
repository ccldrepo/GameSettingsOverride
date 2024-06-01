#pragma once

#include <compare>
#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace Win
{
    namespace Internal
    {
        [[nodiscard]] void* GetModuleFunc(const wchar_t* a_moduleName, const char* a_funcName) noexcept;
    }

    template <class T>
        requires(std::is_pointer_v<T> && std::is_function_v<std::remove_pointer_t<T>>)
    [[nodiscard]] inline T GetModuleFunc(const wchar_t* a_moduleName, const char* a_funcName) noexcept
    {
        return reinterpret_cast<T>(Internal::GetModuleFunc(a_moduleName, a_funcName));
    }

    // The version of Windows operating system.
    class OsVersion
    {
    public:
        [[nodiscard]] static std::optional<OsVersion> Get() noexcept;

        constexpr OsVersion(std::uint32_t a_major, std::uint32_t a_minor, std::uint32_t a_build) noexcept :
            _major(a_major), _minor(a_minor), _build(a_build)
        {}

        [[nodiscard]] constexpr std::uint32_t major() const noexcept { return _major; }
        [[nodiscard]] constexpr std::uint32_t minor() const noexcept { return _minor; }
        [[nodiscard]] constexpr std::uint32_t build() const noexcept { return _build; }

        [[nodiscard]] std::string string(std::string_view a_separator) const
        {
            return std::format("{1}{0}{2}{0}{3}", a_separator, _major, _minor, _build);
        }

        [[nodiscard]] std::wstring wstring(std::wstring_view a_separator) const
        {
            return std::format(L"{1}{0}{2}{0}{3}", a_separator, _major, _minor, _build);
        }

        friend bool operator==(const OsVersion&, const OsVersion&) = default;
        friend auto operator<=>(const OsVersion&, const OsVersion&) = default;

    private:
        std::uint32_t _major;
        std::uint32_t _minor;
        std::uint32_t _build;
    };
}
