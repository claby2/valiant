#ifndef VALIANT_ERROR_HPP
#define VALIANT_ERROR_HPP

#include <exception>
#include <stdexcept>

namespace valiant {
struct ValiantError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
};  // namespace valiant

#endif