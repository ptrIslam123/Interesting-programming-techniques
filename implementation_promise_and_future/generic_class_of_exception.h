#pragma once

#include <exception>
#include <string>

namespace util {

class GenericClassOfException : public std::exception {
public:
    explicit GenericClassOfException(std::string &&msg);

    virtual const char *what() const noexcept override;

private:
    std::string errorMsg_;
};

} // namespace util