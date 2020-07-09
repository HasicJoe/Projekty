#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>

SCENARIO("Framebuffer tests"){
  std::cerr << "04 - framebuffer tests" << std::endl;
  auto gpu = GPU();
  gpu.createFramebuffer(100,120);
  REQUIRE(gpu.getFramebufferWidth() == 100);
  REQUIRE(gpu.getFramebufferHeight() == 120);
  REQUIRE(gpu.getFramebufferColor() != nullptr);
  REQUIRE(gpu.getFramebufferDepth() != nullptr);

  gpu.resizeFramebuffer(500,510);
  REQUIRE(gpu.getFramebufferWidth() == 500);
  REQUIRE(gpu.getFramebufferHeight() == 510);
  REQUIRE(gpu.getFramebufferColor() != nullptr);
  REQUIRE(gpu.getFramebufferDepth() != nullptr);

  gpu.resizeFramebuffer(1337,13);
  REQUIRE(gpu.getFramebufferWidth() == 1337);
  REQUIRE(gpu.getFramebufferHeight() == 13);
  REQUIRE(gpu.getFramebufferColor() != nullptr);
  REQUIRE(gpu.getFramebufferDepth() != nullptr);

  gpu.deleteFramebuffer();
}
