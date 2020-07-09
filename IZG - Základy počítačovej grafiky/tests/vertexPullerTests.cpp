#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>

SCENARIO("GPU vertex puller id tests"){
  std::cerr << "02 - GPU vertex puller id tests" << std::endl;
  auto gpu = GPU();
  REQUIRE(gpu.isVertexPuller(emptyID) == false);
  REQUIRE(gpu.isVertexPuller(0) == false);
  REQUIRE(gpu.isVertexPuller(1) == false);
  REQUIRE(gpu.isVertexPuller(2) == false);
  REQUIRE(gpu.isVertexPuller(3) == false);

  VertexPullerID b0,b1;

  b0 = gpu.createVertexPuller();
  REQUIRE(gpu.isVertexPuller(b0) == true);
  REQUIRE(gpu.isBuffer(b0) == false);
  gpu.deleteVertexPuller(b0);
  REQUIRE(gpu.isVertexPuller(b0) == false);

  b0 = gpu.createVertexPuller();
  b1 = gpu.createVertexPuller();
  REQUIRE(gpu.isVertexPuller(b0) == true);
  REQUIRE(gpu.isVertexPuller(b1) == true);
  REQUIRE(b0 != b1);

  gpu.deleteVertexPuller(b0);
  REQUIRE(gpu.isVertexPuller(b0) == false);
  REQUIRE(gpu.isVertexPuller(b1) == true);
  b0 = gpu.createVertexPuller();
  REQUIRE(gpu.isVertexPuller(b0) == true);
  REQUIRE(gpu.isVertexPuller(b1) == true);
  gpu.deleteVertexPuller(b0);
  gpu.deleteVertexPuller(b1);
  REQUIRE(gpu.isVertexPuller(b0) == false);
  REQUIRE(gpu.isVertexPuller(b1) == false);

  size_t N=100;
  std::vector<BufferID>ids;
  for(size_t i=0;i<N;++i){
    auto b = gpu.createVertexPuller();
    ids.push_back(b);
  }

  for(size_t i=0;i<N-1;++i)
    for(size_t j=i+1;j<N;++j)
      REQUIRE(ids[i] != ids[j]);

  for(auto const&x:ids){
    REQUIRE(gpu.isVertexPuller(x) == true);
    REQUIRE(gpu.isBuffer(x) == false);
  }

  for(auto const&x:ids)
    gpu.deleteVertexPuller(x);

  for(auto const&x:ids){
    REQUIRE(gpu.isVertexPuller(x) == false);
    REQUIRE(gpu.isBuffer(x) == false);
  }
}

