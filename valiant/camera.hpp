#ifndef VALIANT_CAMERA_HPP
#define VALIANT_CAMERA_HPP

#include "object.hpp"
#include "time.hpp"

namespace valiant {
struct CameraData {
    float size;
    Vector3 position;

    bool operator==(const CameraData& camera_data) const {
        return (size == camera_data.size && position == camera_data.position);
    }
};

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
