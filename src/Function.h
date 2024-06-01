// This is SDK for MFM developer. Drop this file to your project.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#define MFMAPI extern "C" [[maybe_unused]] __declspec(dllexport)

////////////////////////////////////////////////////////////////////////////////
// MFMAPI_Type
//
// The allowed types of mod functions.
enum class MFMAPI_Type : std::uint32_t
{
    kVoid = 0,
    kMessage = 1,
    kMessageBox = 2,
};

inline MFMAPI_Type MFMAPI_Type_StrToEnum(std::string_view a_str)
{
    using namespace std::literals::string_view_literals;

    if (a_str.empty() || a_str == "Void"sv) {
        return MFMAPI_Type::kVoid;
    } else if (a_str == "Message"sv) {
        return MFMAPI_Type::kMessage;
    } else if (a_str == "MessageBox"sv) {
        return MFMAPI_Type::kMessageBox;
    } else {
        return MFMAPI_Type::kVoid;
    }
}

inline std::string MFMAPI_Type_EnumToStr(MFMAPI_Type a_enum)
{
    using namespace std::literals::string_literals;

    switch (a_enum) {
    case MFMAPI_Type::kVoid:
        return "Void"s;
    case MFMAPI_Type::kMessage:
        return "Message"s;
    case MFMAPI_Type::kMessageBox:
        return "MessageBox"s;
    default:
        return "Void"s;
    }
}

extern "C"
{
    using MFMAPI_Void = void (*)();
    // Copy your message to `a_msg` buffer with `a_len` length.
    using MFMAPI_Message = void (*)(char* a_msg, std::size_t a_len);
    // Copy your message to `a_msg` buffer with `a_len` length.
    using MFMAPI_MessageBox = MFMAPI_Message;
}

////////////////////////////////////////////////////////////////////////////////
// MFMAPI_PreAction
//
// The action before invoking a mod function.
enum class MFMAPI_PreAction : std::uint32_t
{
    kNone = 0,
    kCloseMenu = 1,
    kCloseMenuAndResetPath = 2,
};

inline MFMAPI_PreAction MFMAPI_PreAction_StrToEnum(std::string_view a_str)
{
    using namespace std::literals::string_view_literals;

    if (a_str.empty() || a_str == "None"sv) {
        return MFMAPI_PreAction::kNone;
    } else if (a_str == "CloseMenu"sv) {
        return MFMAPI_PreAction::kCloseMenu;
    } else if (a_str == "CloseMenuAndResetPath"sv) {
        return MFMAPI_PreAction::kCloseMenuAndResetPath;
    } else {
        return MFMAPI_PreAction::kNone;
    }
}

inline std::string MFMAPI_PreAction_EnumToStr(MFMAPI_PreAction a_enum)
{
    using namespace std::literals::string_literals;

    switch (a_enum) {
    case MFMAPI_PreAction::kNone:
        return "None"s;
    case MFMAPI_PreAction::kCloseMenu:
        return "CloseMenu"s;
    case MFMAPI_PreAction::kCloseMenuAndResetPath:
        return "CloseMenuAndResetPath"s;
    default:
        return "None"s;
    }
}

////////////////////////////////////////////////////////////////////////////////
// MFMAPI_PostAction
//
// The action after invoking a mod function.
enum class MFMAPI_PostAction : std::uint32_t
{
    kNone = 0,
    kCloseMenu = 1,
    kCloseMenuAndResetPath = 2,
};

inline MFMAPI_PostAction MFMAPI_PostAction_StrToEnum(std::string_view a_str)
{
    using namespace std::literals::string_view_literals;

    if (a_str.empty() || a_str == "None"sv) {
        return MFMAPI_PostAction::kNone;
    } else if (a_str == "CloseMenu"sv) {
        return MFMAPI_PostAction::kCloseMenu;
    } else if (a_str == "CloseMenuAndResetPath"sv) {
        return MFMAPI_PostAction::kCloseMenuAndResetPath;
    } else {
        return MFMAPI_PostAction::kNone;
    }
}

inline std::string MFMAPI_PostAction_EnumToStr(MFMAPI_PostAction a_enum)
{
    using namespace std::literals::string_literals;

    switch (a_enum) {
    case MFMAPI_PostAction::kNone:
        return "None"s;
    case MFMAPI_PostAction::kCloseMenu:
        return "CloseMenu"s;
    case MFMAPI_PostAction::kCloseMenuAndResetPath:
        return "CloseMenuAndResetPath"s;
    default:
        return "None"s;
    }
}
