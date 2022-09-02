#include "generic_class_of_exception.h"

namespace util {

GenericClassOfException::GenericClassOfException(std::string &&msg):
errorMsg_(std::move(msg)) {}

const char *GenericClassOfException::what() const noexcept {
    return errorMsg_.c_str();
}

} // namespace util