#pragma once

#include <exception>
#include <optional>
#include <string>

#include "generic_class_of_exception.h"

namespace util {

template<typename T>
class Lazy {
public:
    typedef T ValueType;
    typedef GenericClassOfException BadLazy;

    Lazy();
    ~Lazy();
    Lazy(const ValueType &value);
    Lazy(ValueType &&value);
    Lazy(Lazy<T> &&other) noexcept;
    Lazy(const Lazy &other) = default;
    Lazy &operator=(Lazy &&other) noexcept;
    Lazy &operator=(const Lazy &other) = default;

    void setValue(const ValueType &value);
    void setValue(ValueType &&value);

    bool isInit() const;
    ValueType &getValue();
    const ValueType &getValue() const;

private:
    bool isInit_;
    std::optional<ValueType> value_;
};

template<typename T>
Lazy<T>::Lazy():
isInit_(false),
value_() {}

template<typename T>
Lazy<T>::~Lazy() {
    isInit_ = false;
}

template<typename T>
Lazy<T>::Lazy(const ValueType &value):
isInit_(true),
value_(value) {}

template<typename T>
Lazy<T>::Lazy(ValueType &&value):
isInit_(true),
value_(std::move(value)) {}

template<typename T>
Lazy<T>::Lazy(Lazy<T> &&other) noexcept:
isInit_(other.isInit_),
value_(std::move(other.value_)) {
    other.isInit_ = false;
}

template<typename T>
Lazy<T> &Lazy<T>::operator=(Lazy<T> &&other) noexcept {
    isInit_ = other.isInit_;
    value_ = std::move(other.value_);
    other.isInit_ = false;
    return *this;
}

template<typename T>
bool Lazy<T>::isInit() const {
    return isInit_;
}

template<typename T>
typename Lazy<T>::ValueType &Lazy<T>::getValue() {
    if (!isInit_) {
        throw BadLazy("Attempt get value from empty field");
    }
    return value_.value();
}

template<typename T>
const typename Lazy<T>::ValueType &Lazy<T>::getValue() const {
    if (!isInit_) {
        throw BadLazy("Attempt get value from empty field");
    }
    return value_.value();
}

template<typename T>
void Lazy<T>::setValue(const ValueType &value) {
    isInit_ = true;
    value_ = value;
}

template<typename T>
void Lazy<T>::setValue(ValueType &&value) {
    isInit_ = true;
    value_ = std::move(value);
}

} // namespace util