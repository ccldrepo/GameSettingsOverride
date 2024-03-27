#include "GameSettings.h"

#include <toml++/toml.hpp>

namespace
{
    std::vector<std::string> ScanDir(const std::filesystem::path& a_root)
    {
        if (!std::filesystem::exists(a_root)) {
            SKSE::log::warn("\"{}\" does not exist.", a_root.generic_string());
            return {};
        }

        if (!std::filesystem::is_directory(a_root)) {
            SKSE::log::error("\"{}\" is not a directory.", a_root.generic_string());
            return {};
        }

        std::vector<std::string> paths;
        paths.reserve(8);
        for (auto& entry : std::filesystem::directory_iterator{ a_root }) {
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

    void SetSetting(RE::GameSettingCollection* a_collection, const std::string& a_name, const toml::node& a_node)
    {
        auto setting = a_collection->GetSetting(a_name.c_str());
        if (!setting) {
            SKSE::log::error("Unknown setting for {}.", a_name);
            return;
        }

        switch (a_node.type()) {
        case toml::node_type::boolean:
            {
                bool value = *a_node.value<bool>();
                setting->data.b = value;
                SKSE::log::info("Set {} = {}", a_name, value);
            }
            break;
        case toml::node_type::integer:
            {
                int32_t value = *a_node.value<int32_t>();
                setting->data.i = value;
                SKSE::log::info("Set {} = {}", a_name, value);
            }
            break;
        case toml::node_type::floating_point:
            {
                float value = *a_node.value<float>();
                setting->data.f = value;
                SKSE::log::info("Set {} = {:.6f}", a_name, value);
            }
            break;
        case toml::node_type::string:
            {
                // NOTE: Does this cause a memory leak£¿
                auto free_str = new std::string{ std::move(*a_node.value<std::string>()) };
                setting->data.s = free_str->data();
                SKSE::log::info("Set {} = {}", a_name, *free_str);
            }
            break;
        default:
            SKSE::log::error("Unknown data type for {}.", a_name);
            break;
        }
    }

    void LoadFile(RE::GameSettingCollection* a_collection, const std::string& a_path)
    {
        auto data = toml::parse_file(a_path);
        for (auto& [key, value] : data) {
            std::string name{ key.str() };
            SetSetting(a_collection, name, value);
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
