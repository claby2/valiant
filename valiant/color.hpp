#ifndef VALIANT_COLOR_HPP
#define VALIANT_COLOR_HPP

#include <cstdint>

namespace valiant {
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255,
          uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    void operator=(Color color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
    }
};
}  // namespace valiant

#endif
