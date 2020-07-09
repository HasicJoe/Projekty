#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>

SCENARIO("GPU shader program id tests"){
  std::cerr << "03 - GPU shader program id tests" << std::endl;
  auto gpu = GPU();
  REQUIRE(gpu.isProgram(emptyID) == false);
  REQUIRE(gpu.isProgram(0) == false);
  REQUIRE(gpu.isProgram(1) == false);
  REQUIRE(gpu.isProgram(2) == false);
  REQUIRE(gpu.isProgram(3) == false);

  VertexPullerID b0,b1;

  b0 = gpu.createProgram();
  REQUIRE(gpu.isProgram(b0) == true);
  REQUIRE(gpu.isBuffer(b0) == false);
  REQUIRE(gpu.isVertexPuller(b0) == false);
  gpu.deleteProgram(b0);
  REQUIRE(gpu.isProgram(b0) == false);

  b0 = gpu.createProgram();
  b1 = gpu.createProgram();
  REQUIRE(gpu.isProgram(b0) == true);
  REQUIRE(gpu.isProgram(b1) == true);
  REQUIRE(b0 != b1);

  gpu.deleteProgram(b0);
  REQUIRE(gpu.isProgram(b0) == false);
  REQUIRE(gpu.isProgram(b1) == true);
  b0 = gpu.createProgram();
  REQUIRE(gpu.isProgram(b0) == true);
  REQUIRE(gpu.isProgram(b1) == true);
  gpu.deleteProgram(b0);
  gpu.deleteProgram(b1);
  REQUIRE(gpu.isProgram(b0) == false);
  REQUIRE(gpu.isProgram(b1) == false);

  size_t N=100;
  std::vector<BufferID>ids;
  for(size_t i=0;i<N;++i){
    auto b = gpu.createProgram();
    ids.push_back(b);
  }

  for(size_t i=0;i<N-1;++i)
    for(size_t j=i+1;j<N;++j)
      REQUIRE(ids[i] != ids[j]);

  for(auto const&x:ids){
    REQUIRE(gpu.isProgram(x) == true);
    REQUIRE(gpu.isBuffer(x) == false);
    REQUIRE(gpu.isVertexPuller(x) == false);
  }

  for(auto const&x:ids)
    gpu.deleteProgram(x);

  for(auto const&x:ids){
    REQUIRE(gpu.isProgram(x) == false);
    REQUIRE(gpu.isBuffer(x) == false);
    REQUIRE(gpu.isVertexPuller(x) == false);
  }
}
