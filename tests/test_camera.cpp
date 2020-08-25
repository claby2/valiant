#include <catch2/catch.hpp>

#include "../valiant/camera.hpp"
#include "../valiant/renderer.hpp"

TEST_CASE() {
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
    valiant::Renderer renderer(valiant::RenderMode::DISABLE);
    Camera camera;
    renderer.add_camera(camera);
    renderer.run();
    bool has_awake = camera.get_awake();
    bool has_start = camera.get_start();
    REQUIRE(has_awake == true);
    REQUIRE(has_start == true);
}
