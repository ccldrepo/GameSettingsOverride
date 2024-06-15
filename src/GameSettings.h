#pragma once

class GameSettings
{
public:
    static void Load(bool a_abort = true);

    static inline const std::filesystem::path root{ L"Data/SKSE/Plugins/ccld_GameSettingsOverride/"sv };
};
