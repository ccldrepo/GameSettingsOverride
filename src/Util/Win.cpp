#include "Win.h"

#include <Windows.h>

using RtlGetVersionFuncPtr = NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOEXW);

std::optional<OsVersion> GetOsVersion() noexcept
{
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        return std::nullopt;
    }

    auto RtlGetVersion = reinterpret_cast<RtlGetVersionFuncPtr>(GetProcAddress(hNtdll, "RtlGetVersion"));
    if (!RtlGetVersion) {
        return std::nullopt;
    }

    RTL_OSVERSIONINFOEXW ovi;
    ovi.dwOSVersionInfoSize = sizeof(ovi);
    if (RtlGetVersion(&ovi) != 0) {
        return std::nullopt;
    }
    return OsVersion{ ovi.dwMajorVersion, ovi.dwMinorVersion, ovi.dwBuildNumber };
}
