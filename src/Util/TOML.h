#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <toml++/toml.hpp>

template <class T>
concept TOMLScalar = std::is_arithmetic_v<T> || std::is_same_v<T, std::string>;

class TOMLError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

[[nodiscard]] inline toml::table LoadTOMLFile(const std::filesystem::path& a_path)
{
    const auto size = std::filesystem::file_size(a_path);

    std::vector<char> buffer;
    buffer.resize(static_cast<std::vector<char>::size_type>(size));

    if (std::ifstream file{ a_path, std::ios_base::in | std::ios_base::binary }) {
        file.read(buffer.data(), static_cast<std::streamsize>(size));
    } else {
        throw TOMLError("File could not be opened for reading");
    }

    std::string_view doc{ buffer.data(), buffer.size() };
    return toml::parse(doc, a_path.native());
}

[[nodiscard]] inline toml::table LoadTOMLFile(const std::string& a_path) = delete;
[[nodiscard]] inline toml::table LoadTOMLFile(std::string_view a_path) = delete;
[[nodiscard]] inline toml::table LoadTOMLFile(const char* a_path) = delete;

inline void SaveTOMLFile(const std::filesystem::path& a_path, const toml::table& a_table)
{
    if (std::ofstream file{ a_path }) {
        file << a_table << std::endl;
    } else {
        throw TOMLError("File could not be opened for writing");
    }
}

inline void SaveTOMLFile(const std::string& a_path, const toml::table& a_table) = delete;
inline void SaveTOMLFile(std::string_view a_path, const toml::table& a_table) = delete;
inline void SaveTOMLFile(const char* a_path, const toml::table& a_table) = delete;

template <TOMLScalar T, bool required = false>
inline void LoadTOMLValue(const toml::table& a_table, std::string_view a_key, T& a_target)
{
    auto node = a_table.get(a_key);
    if (!node) {
        if constexpr (required) {
            throw TOMLError(std::format("'{}' is required", a_key));
        } else {
            return;  // Leave target unchanged.
        }
    }

    auto value = node->value<T>();
    if (!value) {
        auto msg = std::format("Invalid '{}'", a_key);
        throw TOMLError(msg);
    }
    a_target = std::move(*value);
}

template <TOMLScalar T, bool required = false>
inline void LoadTOMLValue(const toml::table& a_table, std::string_view a_key, std::vector<T>& a_target)
{
    auto node = a_table.get(a_key);
    if (!node) {
        if constexpr (required) {
            throw TOMLError(std::format("'{}' is required", a_key));
        } else {
            return;  // Leave target unchanged.
        }
    }

    auto arr = node->as_array();
    if (!arr) {
        auto msg = std::format("'{}' is not an array", a_key);
        throw TOMLError(msg);
    }

    a_target.clear();
    a_target.reserve(arr->size());
    for (const auto& ele : *arr) {
        auto value = ele.value<T>();
        if (!value) {
            auto msg = std::format("Invalid '{}'", a_key);
            throw TOMLError(msg);
        }
        a_target.push_back(std::move(*value));
    }
}

template <TOMLScalar T>
inline void LoadTOMLValueRequired(const toml::table& a_table, std::string_view a_key, T& a_target)
{
    return LoadTOMLValue<T, true>(a_table, a_key, a_target);
}

template <TOMLScalar T>
inline void LoadTOMLValueRequired(const toml::table& a_table, std::string_view a_key, std::vector<T>& a_target)
{
    return LoadTOMLValue<T, true>(a_table, a_key, a_target);
}

template <TOMLScalar T>
inline void SaveTOMLValue(toml::table& a_table, std::string_view a_key, const T& a_source)
{
    auto [pos, ok] = a_table.insert(a_key, a_source);
    if (!ok) {
        auto msg = std::format("'{}' exists", a_key);
        throw TOMLError(msg);
    }
}

template <TOMLScalar T>
inline void SaveTOMLValue(toml::table& a_table, std::string_view a_key, const std::vector<T>& a_source)
{
    toml::array arr;
    arr.reserve(a_source.size());
    for (const auto& ele : a_source) {
        arr.push_back(ele);
    }

    auto [pos, ok] = a_table.insert(a_key, std::move(arr));
    if (!ok) {
        auto msg = std::format("'{}' exists", a_key);
        throw TOMLError(msg);
    }
}
