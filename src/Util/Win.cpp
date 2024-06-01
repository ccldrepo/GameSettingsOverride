#include "Win.h"

#include <Windows.h>

namespace Win
{
    namespace Internal
    {
        void* GetModuleFunc(const wchar_t* a_moduleName, const char* a_funcName) noexcept
        {
            HMODULE hModule = GetModuleHandleW(a_moduleName);
            if (!hModule) {
                return nullptr;
            }
            return GetProcAddress(hModule, a_funcName);
        }
    }

    std::optional<OsVersion> OsVersion::Get() noexcept
    {
        using RtlGetVersionFuncPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOEXW);

        auto RtlGetVersion = GetModuleFunc<RtlGetVersionFuncPtr>(L"ntdll.dll", "RtlGetVersion");
        if (!RtlGetVersion) {
            return std::nullopt;
        }

        RTL_OSVERSIONINFOEXW ovi{};
        ovi.dwOSVersionInfoSize = sizeof(ovi);
        if (RtlGetVersion(&ovi) != 0) {
            return std::nullopt;
        }
        return OsVersion{ ovi.dwMajorVersion, ovi.dwMinorVersion, ovi.dwBuildNumber };
    }
}
