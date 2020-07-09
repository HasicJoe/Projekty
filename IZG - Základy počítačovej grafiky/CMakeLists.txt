cmake_minimum_required(VERSION 3.12.0)

PROJECT(izgProject)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_STANDARD 99)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

set(SOURCES 
  student/arguments.hpp
  student/main.cpp
  student/fwd.hpp
  student/gpu.hpp
  student/gpu.cpp
  student/window.hpp
  student/window.cpp
  student/method.hpp
  student/application.cpp
  student/application.hpp
  student/timer.hpp
  student/bunny.hpp
  student/bunny.cpp
  student/emptyMethod.hpp
  student/triangleMethod.hpp
  student/triangleMethod.cpp
  student/triangleClip1Method.hpp
  student/triangleClip1Method.cpp
  student/triangleClip2Method.hpp
  student/triangleClip2Method.cpp
  student/triangle3DMethod.hpp
  student/triangle3DMethod.cpp
  student/triangleBufferMethod.hpp
  student/triangleBufferMethod.cpp
  student/czFlagMethod.hpp
  student/czFlagMethod.cpp
  student/phongMethod.hpp
  student/phongMethod.cpp
  tests/renderPhongFrame.hpp
  tests/renderPhongFrame.cpp
  tests/takeScreenShot.hpp
  tests/takeScreenShot.cpp
  tests/conformanceTests.hpp
  tests/conformanceTests.cpp
  tests/performanceTest.hpp
  tests/performanceTest.cpp
  tests/testCommon.hpp
  tests/testCommon.cpp
  tests/bufferTests.cpp
  tests/vertexPullerTests.cpp
  tests/programTests.cpp
  tests/framebufferTests.cpp
  tests/vertexShaderTests.cpp
  tests/fragmentShaderTests.cpp
  tests/clippingTests.cpp
  tests/phongMethodTests.cpp
  )

#option(GLM_QUIET "" ON)
#option(GLM_TEST_ENABLE "" OFF)
#add_subdirectory(glm-0.9.9.6)
#add_library(glm ALIAS glm_static)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm-0.9.9.6)

add_subdirectory(MealyMachine)
add_subdirectory(TxtUtils)
add_subdirectory(ArgumentViewer)
add_subdirectory(BasicCamera)


option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
add_subdirectory(SDL2-2.0.10)
add_library(SDL2::SDL2 ALIAS SDL2-static)
add_library(SDL2::SDL2main ALIAS SDL2main)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} 
  SDL2::SDL2
  SDL2::SDL2main
  ArgumentViewer::ArgumentViewer
  BasicCamera::BasicCamera
  )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

option(${PROJECT_NAME}_BUILD_INTERNAL_TESTS "toggle building of internal tests")
if(${PROJECT_NAME}_BUILD_INTERNAL_TESTS)
  add_subdirectory(tests/internalTests)
endif()

