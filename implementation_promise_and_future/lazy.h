#pragma once

#include <exception>
#include <optional>
#include <string>

namespace util {

template<typename T>
class Lazy {
public:
    typedef T ValueType;
    typedef std::runtime_error BadLazy;

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

    bool hasValue() const;
    ValueType &getValue();
    const ValueType &getValue() const;

private:
    bool isHasValue;
    std::optional<ValueType> value_;
};

template<typename T>
Lazy<T>::Lazy():
isHasValue(false),
value_() {}

template<typename T>
Lazy<T>::~Lazy() {
    isHasValue = false;
}

template<typename T>
Lazy<T>::Lazy(const ValueType &value):
isHasValue(true),
value_(value) {}

template<typename T>
Lazy<T>::Lazy(ValueType &&value):
isHasValue(true),
value_(std::move(value)) {}

template<typename T>
Lazy<T>::Lazy(Lazy<T> &&other) noexcept:
isHasValue(other.isHasValue),
value_(std::move(other.value_)) {
    other.isHasValue = false;
}

template<typename T>
Lazy<T> &Lazy<T>::operator=(Lazy<T> &&other) noexcept {
    isHasValue = other.isHasValue;
    value_ = std::move(other.value_);
    other.isHasValue = false;
    return *this;
}

template<typename T>
bool Lazy<T>::hasValue() const {
    return isHasValue;
}

template<typename T>
typename Lazy<T>::ValueType &Lazy<T>::getValue() {
    if (!isHasValue) {
        throw BadLazy("Attempt get value from empty field");
    }
    return value_.value();
}

template<typename T>
const typename Lazy<T>::ValueType &Lazy<T>::getValue() const {
    if (!isHasValue) {
        throw BadLazy("Attempt get value from empty field");
    }
    return value_.value();
}

template<typename T>
void Lazy<T>::setValue(const ValueType &value) {
    isHasValue = true;
    value_ = value;
}

template<typename T>
void Lazy<T>::setValue(ValueType &&value) {
    isHasValue = true;
    value_ = std::move(value);
}

} // namespace util