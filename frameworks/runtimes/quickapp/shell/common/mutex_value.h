#ifndef _SHELL_MUTEXT_VALUE_H_
#define _SHELL_MUTEXT_VALUE_H_

#include <memory>
#include <mutex>

namespace shell {

template <typename T>
class Mutex;

template <typename T>
class Guard {
public:
    Guard(Guard<T>&& g)
        : guard_(std::move(g.guard_))
        , value_(g.value_)
    {
    }

    T* operator->() { return &value_; }
    T& operator*() { return value_; }

private:
    friend class Mutex<T>;
    Guard(std::mutex& mutex, T& val)
        : guard_(mutex)
        , value_(val)
    {
    }

    std::lock_guard<std::mutex> guard_;
    T& value_;
};

template <typename T>
class Mutex {
public:
    using value_type = T;

    Mutex() { }
    Mutex(const T& v)
        : value_(v)
    {
    }
    Mutex(const Mutex<T>& m)
        : value_(m.value())
    {
    }
    Mutex(const Mutex<T>&& m)
        : value_(std::move(m.value_))
        , mutex_(std::move(m.mutex_))
    {
    }

    /*unsafe*/ value_type& value() { return value_; }
    /*unsafe*/ const value_type& value() const { return value_; }

    void reset(const T& val)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        value_ = val;
    }

    void rest(T&& val)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        value_ = std::move(val);
    }

    value_type&& drop()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return std::move(value_);
    }

    Guard<T> lock()
    {
        return Guard<T>(mutex_, value_);
    }

private:
    value_type value_;
    std::mutex mutex_;
};

}

#endif
