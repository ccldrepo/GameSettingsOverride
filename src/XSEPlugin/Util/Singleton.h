#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <shared_mutex>

template <class T>
class Singleton
{
public:
    [[nodiscard]] static T* GetSingleton()
    {
        static T singleton;
        return std::addressof(singleton);
    }

    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    ~Singleton() = default;
};

template <class T>
class SingletonEx
{
public:
    [[nodiscard]] static T* GetSingleton() noexcept { return _singleton.get(); }

    [[nodiscard]] static std::shared_mutex& GetMutex() noexcept { return _mutex; }

    [[nodiscard]] static std::shared_lock<std::shared_mutex> LockShared() noexcept
    {
        return std::shared_lock<std::shared_mutex>{ _mutex };
    }

    [[nodiscard]] static std::unique_lock<std::shared_mutex> LockUnique() noexcept
    {
        return std::unique_lock<std::shared_mutex>{ _mutex };
    }

    [[nodiscard]] static bool IsVersionChanged(std::uint32_t a_version) noexcept
    {
        return _version.load() != a_version;
    }

    [[nodiscard]] static std::uint32_t GetVersion() noexcept { return _version.load(); }

    static void IncrementVersion() noexcept { _version.fetch_add(1); }

    SingletonEx(const SingletonEx&) = delete;
    SingletonEx(SingletonEx&&) = delete;
    SingletonEx& operator=(const SingletonEx&) = delete;
    SingletonEx& operator=(SingletonEx&&) = delete;

protected:
    SingletonEx() = default;
    ~SingletonEx() = default;

    struct Deleter
    {
        void operator()(T* a_ptr) const { delete a_ptr; }
    };

    static inline std::unique_ptr<T, Deleter> _singleton;
    static inline std::shared_mutex           _mutex;
    static inline std::atomic<std::uint32_t>  _version{ 0 };
};
