#ifndef VALIANT_SHAPE_HPP
#define VALIANT_SHAPE_HPP

#include "color.hpp"

namespace valiant {
struct Shape {
    int width;
    int height;
    bool fill;
    Color color;

    Shape(int new_width = 0, int new_height = 0)
        : width(new_width), height(new_height), fill(true) {}
};
class Rectangle {
   public:
    Shape shape;
};
}  // namespace valiant

#endif
