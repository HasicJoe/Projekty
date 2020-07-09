#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>

SCENARIO("GPU buffer id tests"){
  std::cerr << "00 - GPU buffer id tests" << std::endl;
  auto gpu = GPU();
  REQUIRE(gpu.isBuffer(emptyID) == false);
  REQUIRE(gpu.isBuffer(0) == false);
  REQUIRE(gpu.isBuffer(1) == false);
  REQUIRE(gpu.isBuffer(2) == false);
  REQUIRE(gpu.isBuffer(3) == false);

  BufferID b0,b1;

  b0 = gpu.createBuffer(8);
  REQUIRE(gpu.isBuffer(b0) == true);
  gpu.deleteBuffer(b0);
  REQUIRE(gpu.isBuffer(b0) == false);

  b0 = gpu.createBuffer(8);
  b1 = gpu.createBuffer(8);
  REQUIRE(gpu.isBuffer(b0) == true);
  REQUIRE(gpu.isBuffer(b1) == true);
  REQUIRE(b0 != b1);

  gpu.deleteBuffer(b0);
  REQUIRE(gpu.isBuffer(b0) == false);
  REQUIRE(gpu.isBuffer(b1) == true);
  b0 = gpu.createBuffer(8);
  REQUIRE(gpu.isBuffer(b0) == true);
  REQUIRE(gpu.isBuffer(b1) == true);
  gpu.deleteBuffer(b0);
  gpu.deleteBuffer(b1);
  REQUIRE(gpu.isBuffer(b0) == false);
  REQUIRE(gpu.isBuffer(b1) == false);

  size_t N=100;
  std::vector<BufferID>ids;
  for(size_t i=0;i<N;++i){
    auto b = gpu.createBuffer((i+1)*sizeof(float));
    ids.push_back(b);
  }

  for(size_t i=0;i<N-1;++i)
    for(size_t j=i+1;j<N;++j)
      REQUIRE(ids[i] != ids[j]);

  for(auto const&x:ids)
    REQUIRE(gpu.isBuffer(x) == true);

  for(auto const&x:ids)
    gpu.deleteBuffer(x);

  for(auto const&x:ids)
    REQUIRE(gpu.isBuffer(x) == false);

}

SCENARIO("GPU buffer data tests"){
  std::cerr << "01 - GPU buffer data tests" << std::endl;
  auto gpu = GPU();

  std::vector<uint8_t>inData = {
    9,8,7,6,5,4,3,2,1,0
  };

  auto const bufSize = inData.size()*sizeof(decltype(inData)::value_type);
  auto b = gpu.createBuffer(bufSize);

  gpu.setBufferData(b,0,bufSize,inData.data());

  std::vector<uint8_t>outData;
  outData.resize(inData.size());

  gpu.getBufferData(b,0,bufSize,outData.data());

  REQUIRE(inData == outData);

  uint8_t v[4] = {0x0a,0x0b,0x0c,0x0d};

  gpu.setBufferData(b,1,4,v);

  gpu.getBufferData(b,0,bufSize,outData.data());
  inData[1] = 0x0a;
  inData[2] = 0x0b;
  inData[3] = 0x0c;
  inData[4] = 0x0d;

  REQUIRE(inData == outData);

  gpu.getBufferData(b,2,3,v);
  REQUIRE(v[0] == 0x0b);
  REQUIRE(v[1] == 0x0c);
  REQUIRE(v[2] == 0x0d);
  REQUIRE(v[3] == 0x0d);

  gpu.deleteBuffer(b);
}

