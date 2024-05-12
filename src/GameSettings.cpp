#include "GameSettings.h"

#include <toml++/toml.hpp>

#include "Util/TOML.h"

namespace
{
    inline std::vector<std::filesystem::path> ScanDir(const std::filesystem::path& a_root)
    {
        if (!std::filesystem::exists(a_root)) {
            SKSE::log::warn("\"{}\" does not exist.", PathToStr(a_root));
            return {};
        }

        if (!std::filesystem::is_directory(a_root)) {
            SKSE::log::error("\"{}\" is not a directory.", PathToStr(a_root));
            return {};
        }

        std::vector<std::filesystem::path> paths;
        paths.reserve(8);
        for (const auto& entry : std::filesystem::directory_iterator{ a_root }) {
            if (!entry.is_regular_file()) {
                continue;
            }

            if (const auto& path = entry.path(); path.extension().native() == L".toml"sv) {
                paths.push_back(path);
            }
        }

        std::ranges::sort(paths);
        return paths;
    }

    inline void SetSetting(RE::GameSettingCollection* a_collection, const std::string& a_name, const toml::node& a_node)
    {
        auto setting = a_collection->GetSetting(a_name.c_str());
        if (!setting) {
            SKSE::log::error("Unknown setting '{}'.", a_name);
            return;
        }

        switch (a_node.type()) {
        case toml::node_type::boolean:
            {
                if (!a_name.starts_with("b"sv)) {
                    SKSE::log::warn("'{}' is recognized as bool, but it not starts with 'b'.", a_name);
                }
                bool value = *a_node.value<bool>();
                setting->data.b = value;
                SKSE::log::info("Set {} = {}", a_name, value);
            }
            break;
        case toml::node_type::integer:
            {
                if (!a_name.starts_with("i"sv)) {
                    SKSE::log::warn("'{}' is recognized as integer, but it not starts with 'i'.", a_name);
                }
                int32_t value = *a_node.value<int32_t>();
                setting->data.i = value;
                SKSE::log::info("Set {} = {}", a_name, value);
            }
            break;
        case toml::node_type::floating_point:
            {
                if (!a_name.starts_with("f"sv)) {
                    SKSE::log::warn("'{}' is recognized as float, but it not starts with 'f'.", a_name);
                }
                float value = *a_node.value<float>();
                setting->data.f = value;
                SKSE::log::info("Set {} = {:.6f}", a_name, value);
            }
            break;
        case toml::node_type::string:
            {
                if (!a_name.starts_with("s"sv)) {
                    SKSE::log::warn("'{}' is recognized as string, but it not starts with 's'.", a_name);
                }
                // NOTE: Does this cause a memory leak?
                auto free_str = new std::string{ std::move(*a_node.value<std::string>()) };
                setting->data.s = free_str->data();
                SKSE::log::info("Set {} = {}", a_name, *free_str);
            }
            break;
        default:
            SKSE::log::error("Unknown data type for setting '{}'.", a_name);
            break;
        }
    }

    inline void LoadFile(const std::filesystem::path& a_path, RE::GameSettingCollection* a_collection)
    {
        auto data = LoadTOMLFile(a_path);
        for (auto& [key, value] : data) {
            std::string name{ key.str() };
            SetSetting(a_collection, name, value);
        }
    }
}

void GameSettings::Load(bool a_abort)
{
    for (auto collection = RE::GameSettingCollection::GetSingleton(); const auto& path : ScanDir(root)) {
        try {
            SKSE::log::info("Loading \"{}\"...", PathToStr(path));
            LoadFile(path, collection);
            SKSE::log::info("Successfully loaded \"{}\".", PathToStr(path));
        } catch (const toml::parse_error& e) {
            auto msg = std::format("Failed to load \"{}\" (error occurred at line {}, column {}): {}.", PathToStr(path),
                e.source().begin.line, e.source().begin.column, e.what());
            SKSE::stl::report_fatal_error(msg, a_abort);
        } catch (const std::exception& e) {
            auto msg = std::format("Failed to load \"{}\": {}.", PathToStr(path), e.what());
            SKSE::stl::report_fatal_error(msg, a_abort);
        }
    }
}
