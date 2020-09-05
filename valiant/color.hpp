#ifndef VALIANT_COLOR_HPP
#define VALIANT_COLOR_HPP

#include <cstdint>

namespace valiant {
const int COLOR_MAXIMUM = 255;
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color(uint8_t red = COLOR_MAXIMUM, uint8_t green = COLOR_MAXIMUM,
          uint8_t blue = COLOR_MAXIMUM, uint8_t alpha = COLOR_MAXIMUM)
        : r(red), g(green), b(blue), a(alpha) {}

    Color& operator=(Color color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
        return *this;
    }
};
}  // namespace valiant

#endif
