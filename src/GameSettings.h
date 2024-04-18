#pragma once

#include <PCH.h>

class GameSettings
{
public:
    static void Load();

    static inline const std::filesystem::path root{ L"Data/SKSE/Plugins/ccld_GameSettingsOverride/"sv };
};
