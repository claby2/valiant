project(valiant CXX)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_libraries(SDL2 SDL2main SDL2_image)

find_package(Catch2)
if(Catch2_FOUND)
  set(TESTS
      "tests/test_main.cpp"
      "tests/test_object.cpp"
      "tests/test_sprite_renderer.cpp"
      "tests/test_camera.cpp"
      "tests/test_time.cpp"
      "tests/test_renderer.cpp"
      "tests/test_color.cpp"
      "tests/test_shape.cpp"
      "tests/test_collider.cpp")
  add_executable(test ${TESTS})
  target_link_libraries(test Catch2::Catch2)
endif()

add_executable(example_basic "examples/example_basic.cpp")
add_executable(example_camera "examples/example_camera.cpp")
add_executable(example_rectangle "examples/example_rectangle.cpp")
add_executable(example_collider "examples/example_collider.cpp")
add_executable(example_pong "examples/example_pong.cpp")
