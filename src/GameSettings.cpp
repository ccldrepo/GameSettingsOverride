#include "GameSettings.h"

#include <toml++/toml.hpp>

namespace
{
    std::vector<std::string> ScanDir(const std::filesystem::path& root)
    {
        if (!std::filesystem::exists(root)) {
            SKSE::log::warn("\"{}\" does not exist.", root.generic_string());
            return {};
        }

        if (!std::filesystem::is_directory(root)) {
            SKSE::log::error("\"{}\" is not a directory.", root.generic_string());
            return {};
        }

        std::vector<std::string> paths;
        paths.reserve(8);
        for (auto& entry : std::filesystem::directory_iterator{ root }) {
            if (!entry.is_regular_file()) {
                continue;
            }

            auto& path = entry.path();
            if (path.extension().native() == L".toml"sv) {
                paths.push_back(path.generic_string());
            }
        }

        std::ranges::sort(paths);
        return paths;
    }

    void SetSetting(RE::GameSettingCollection* collection, const std::string& name, const toml::node& node)
    {
        auto setting = collection->GetSetting(name.c_str());
        if (!setting) {
            SKSE::log::error("Unknown setting for {}.", name);
            return;
        }

        switch (node.type()) {
        case toml::node_type::boolean:
            {
                bool value = *node.value<bool>();
                setting->data.b = value;
                SKSE::log::info("Set {} = {}", name, value);
            }
            break;
        case toml::node_type::integer:
            {
                int32_t value = *node.value<int32_t>();
                setting->data.i = value;
                SKSE::log::info("Set {} = {}", name, value);
            }
            break;
        case toml::node_type::floating_point:
            {
                float value = *node.value<float>();
                setting->data.f = value;
                SKSE::log::info("Set {} = {}", name, value);
            }
            break;
        case toml::node_type::string:
            {
                // NOTE: Does this cause a memory leak£¿
                auto free_str = new std::string{ std::move(*node.value<std::string>()) };
                setting->data.s = free_str->data();
                SKSE::log::info("Set {} = {}", name, *free_str);
            }
            break;
        default:
            SKSE::log::error("Unknown data type for {}.", name);
            break;
        }
    }

    void LoadFile(RE::GameSettingCollection* collection, const std::string& path)
    {
        auto data = toml::parse_file(path);
        for (auto& [key, value] : data) {
            std::string name{ key.str() };
            SetSetting(collection, name, value);
        }
    }
}

void GameSettings::Load()
{
    auto collection = RE::GameSettingCollection::GetSingleton();
    auto paths = ScanDir(root);
    for (auto& path : paths) {
        try {
            LoadFile(collection, path);
            SKSE::log::info("Successfully loaded \"{}\".", path);
        } catch (const toml::parse_error& e) {
            auto msg = std::format("Failed to load \"{}\" (error occurred at line {}, column {}): {}.", path,
                e.source().begin.line, e.source().begin.column, e.what());
            SKSE::stl::report_and_fail(msg);
        } catch (const std::exception& e) {
            auto msg = std::format("Failed to load \"{}\": {}.", path, e.what());
            SKSE::stl::report_and_fail(msg);
        }
    }
}

const std::filesystem::path GameSettings::root{ "Data/SKSE/Plugins/ccld_GameSettingsOverride/"sv };
