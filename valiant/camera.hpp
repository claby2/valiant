#ifndef VALIANT_CAMERA_HPP
#define VALIANT_CAMERA_HPP

#include "object.hpp"
#include "time.hpp"

namespace valiant {
const float DEFAULT_CAMERA_SIZE = 1.;
class CameraComponent {
   public:
    float size;
    explicit CameraComponent(float new_size = DEFAULT_CAMERA_SIZE)
        : size(new_size) {}
};
class Camera {
   public:
    CameraComponent camera;
    Transform transform;
    Input input;
    Time time;
    std::string tag = "untagged";
    virtual void awake() {}
    virtual void start() {}
    virtual void update() {}
};
}  // namespace valiant
#endif
