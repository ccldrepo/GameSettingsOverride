#pragma once

#include <format>
#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

#include <toml++/toml.hpp>

class TOMLError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

template <class T>
inline void LoadTOMLValue(const toml::table& a_table, std::string_view a_key, T& a_target)
{
    auto node = a_table.get(a_key);
    if (!node) {
        return;  // Leave target unchanged.
    }

    auto value = node->value<T>();
    if (!value) {
        auto msg = std::format("Invalid {}", a_key);
        throw TOMLError(msg);
    }
    a_target = std::move(*value);
}

template <class T>
inline void LoadTOMLValue(const toml::table& a_table, std::string_view a_key, std::vector<T>& a_target)
{
    auto node = a_table.get(a_key);
    if (!node) {
        return;  // Leave target unchanged.
    }

    auto arr = node->as_array();
    if (!arr) {
        auto msg = std::format("{} is not array", a_key);
        throw TOMLError(msg);
    }

    a_target.clear();
    for (const auto& ele : *arr) {
        auto value = ele.value<T>();
        if (!value) {
            auto msg = std::format("Invalid {}", a_key);
            throw TOMLError(msg);
        }
        a_target.push_back(std::move(*value));
    }
}

template <class T>
inline void SaveTOMLValue(toml::table& a_table, std::string_view a_key, const T& a_source)
{
    auto [pos, ok] = a_table.insert(a_key, a_source);
    if (!ok) {
        auto msg = std::format("{} exists", a_key);
        throw TOMLError(msg);
    }
}

template <class T>
inline void SaveTOMLValue(toml::table& a_table, std::string_view a_key, const std::vector<T>& a_source)
{
    toml::array arr;
    for (const auto& ele : a_source) {
        arr.push_back(ele);
    }

    auto [pos, ok] = a_table.insert(a_key, std::move(arr));
    if (!ok) {
        auto msg = std::format("{} exists", a_key);
        throw TOMLError(msg);
    }
}
