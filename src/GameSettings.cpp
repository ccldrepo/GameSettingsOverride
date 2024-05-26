#include "GameSettings.h"

#include <toml++/toml.hpp>

#include "Util/TOML.h"

namespace
{
    inline RE::Color IntToColor(std::uint32_t a_int) noexcept
    {
        // Unpack integer to (red, green, blue, alpha).
        return RE::Color{ (a_int >> 24) & 0xFF, (a_int >> 16) & 0xFF, (a_int >> 8) & 0xFF, a_int & 0xFF };
    }

    inline std::vector<std::filesystem::path> ScanDir(const std::filesystem::path& a_root)
    {
        auto st = std::filesystem::status(a_root);

        if (!std::filesystem::exists(st)) {
            SKSE::log::warn("\"{}\" does not exist.", PathToStr(a_root));
            return {};
        }

        if (!std::filesystem::is_directory(st)) {
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

        switch (setting->GetType()) {
        case RE::Setting::Type::kBool:
            {
                if (auto value = a_node.value<bool>()) {
                    setting->data.b = *value;
                    SKSE::log::info("Set {} = {}", a_name, *value);
                } else {
                    SKSE::log::error("Setting '{}' should be bool.", a_name);
                }
            }
            break;
        case RE::Setting::Type::kFloat:
            {
                if (auto value = a_node.value<float>()) {
                    setting->data.f = *value;
                    SKSE::log::info("Set {} = {:.6f}", a_name, *value);
                } else {
                    SKSE::log::error("Setting '{}' should be float.", a_name);
                }
            }
            break;
        case RE::Setting::Type::kSignedInteger:
            {
                if (auto value = a_node.value<std::int32_t>()) {
                    setting->data.i = *value;
                    SKSE::log::info("Set {} = {}", a_name, *value);
                } else {
                    SKSE::log::error("Setting '{}' should be signed integer.", a_name);
                }
            }
            break;
        case RE::Setting::Type::kColor:
            {
                if (auto value = a_node.value<std::uint32_t>()) {
                    setting->data.r = IntToColor(*value);
                    SKSE::log::info("Set {} = 0x{:08X}", a_name, *value);
                } else {
                    SKSE::log::error("Setting '{}' should be color.", a_name);
                }
            }
            break;
        case RE::Setting::Type::kString:
            {
                if (auto value = a_node.value<std::string>()) {
                    // NOTE: Does this cause a memory leak?
                    auto free_str = new std::string{ *std::move(value) };
                    setting->data.s = free_str->data();
                    SKSE::log::info("Set {} = {}", a_name, *free_str);
                } else {
                    SKSE::log::error("Setting '{}' should be string.", a_name);
                }
            }
            break;
        case RE::Setting::Type::kUnsignedInteger:
            {
                if (auto value = a_node.value<std::uint32_t>()) {
                    setting->data.u = *value;
                    SKSE::log::info("Set {} = {}", a_name, *value);
                } else {
                    SKSE::log::error("Setting '{}' should be unsigned integer.", a_name);
                }
            }
            break;
        default:
            SKSE::log::error("Unknown data type for setting '{}'.", a_name);
            break;
        }
    }

    inline void LoadFile(const std::filesystem::path& a_path, RE::GameSettingCollection* a_collection)
    {
        for (auto data = LoadTOMLFile(a_path); auto& [key, value] : data) {
            SetSetting(a_collection, std::string{ key.str() }, value);
        }
    }
}

void GameSettings::Load(bool a_abort)
{
    for (auto collection = RE::GameSettingCollection::GetSingleton(); const auto& path : ScanDir(root)) {
        try {
            SKSE::log::info("\"{}\" is loading...", PathToStr(path));
            LoadFile(path, collection);
            SKSE::log::info("\"{}\" has finished loading.", PathToStr(path));
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
