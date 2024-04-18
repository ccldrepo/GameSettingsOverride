#pragma once

#include <memory>

// The singleton base class.
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
};
