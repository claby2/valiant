#include <catch2/catch.hpp>

#include "../valiant/camera.hpp"
#include "../valiant/renderer.hpp"

TEST_CASE("Camera default values") {
    class Camera : public valiant::Camera {};
    Camera camera;
    valiant::Vector3 position = camera.transform.position;
    REQUIRE(position.x == 0);
    REQUIRE(position.y == 0);
    REQUIRE(position.z == 0);
    REQUIRE(camera.camera.size == valiant::DEFAULT_CAMERA_SIZE);
}

TEST_CASE("Camera methods") {
    class Camera : public valiant::Camera {
       public:
        Camera() : has_awake_(false), has_start_(false) {}
        void awake() override { has_awake_ = true; }
        void start() override { has_start_ = true; }
        inline bool get_awake() const { return has_awake_; }
        inline bool get_start() const { return has_start_; }

       private:
        bool has_awake_;
        bool has_start_;
    };
    valiant::Renderer renderer(valiant::DISABLE);
    Camera camera;
    renderer.add_camera(camera);
    renderer.run();
    bool has_awake = camera.get_awake();
    bool has_start = camera.get_start();
    REQUIRE(has_awake == true);
    REQUIRE(has_start == true);
}
