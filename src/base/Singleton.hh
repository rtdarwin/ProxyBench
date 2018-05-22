#pragma once

template <typename T>
class Singleton
{
public:
    static T* get()
    {
        // C++11 promises thread-safty of static variable
        static T instance;
        return &instance;
    }

protected:
    Singleton() {}
    ~Singleton() {}

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};
