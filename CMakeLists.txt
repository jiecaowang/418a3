cmake_minimum_required(VERSION 3.0.0)
project(RayTracer VERSION 0.1.0)

set_property(GLOBAL PROPERTY ENABLED_LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SOURCES
    Source/raytracer.cpp
    Source/BaseMaterials.cpp
    Source/bmp_io.cpp
    Source/Colour.cpp
    Source/Euclidean.cpp
    Source/Intersection.cpp
    Source/light_source.cpp
    Source/Materials.cpp
    Source/Optics.cpp
    Source/Ray.cpp
    Source/scene_object.cpp
    Source/Surface.cpp
)

add_executable(RayTracer ${SOURCES})
target_include_directories(RayTracer
    PRIVATE
        api)


add_executable(tests 
Tests/test_Colour.cpp)

target_link_libraries(tests
PRIVATE
GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tests)


set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../cmake-modules)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    message("Setup for code coverage")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
