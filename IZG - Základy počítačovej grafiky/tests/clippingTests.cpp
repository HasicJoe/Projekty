#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>

#include <glm/gtc/matrix_transform.hpp>

std::vector<OutVertex>vsData;
void vertexShaderDataInject(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&){
  if(inVertex.gl_VertexID<vsData.size())
    outVertex = vsData[inVertex.gl_VertexID];
}

std::vector<InFragment>fsData;
void fragmentShaderDump(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&){
  fsData.push_back(inFragment);
}

SCENARIO("triangles (clock wise) that lie completely behind near plane should be clipped"){
  std::cerr << "24 - clipping - CW triangle behind near plane" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderDataInject,fragmentShaderDump);
  gpu->bindVertexPuller(vao);


  OutVertex a;
  vsData.clear();
  fsData.clear();
  //near 1, far 2
  vsData.push_back({{},glm::vec4(-1.f,-1.f,-7.f,-1.f)});
  vsData.push_back({{},glm::vec4(+1.f,-1.f,-7.f,-1.f)});
  vsData.push_back({{},glm::vec4(-1.f,+1.f,-7.f,-1.f)});

  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  REQUIRE(fsData.size() == 0);

}

SCENARIO("triangles (counter clock wise) that lie completely behind near plane should be clipped"){
  std::cerr << "24 - clipping - CCW triangle behind near plane" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderDataInject,fragmentShaderDump);
  gpu->bindVertexPuller(vao);


  OutVertex a;
  vsData.clear();
  fsData.clear();
  //near 1, far 2
  vsData.push_back({{},glm::vec4(-1.f,-1.f,-7.f,-1.f)});
  vsData.push_back({{},glm::vec4(-1.f,+1.f,-7.f,-1.f)});
  vsData.push_back({{},glm::vec4(+1.f,-1.f,-7.f,-1.f)});

  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  REQUIRE(fsData.size() == 0);

}


SCENARIO("triangles with 1 vertex behind near plane should be clipped"){
  std::cerr << "24 - clipping - 1 vertex behind near plane" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderDataInject,fragmentShaderDump);
  gpu->bindVertexPuller(vao);


  OutVertex a;
  vsData.clear();
  fsData.clear();
  
  float nnear = 1.f;
  float ffar  = 2.f;
  float e = - (ffar + nnear) / (ffar - nnear);
  float f = - 2.f * ffar * nnear / (ffar - nnear);
  float z = 2.f/3.f;

  vsData.push_back({{},glm::vec4(-2.f,-2.f,+2.f,+2.f)});
  vsData.push_back({{},glm::vec4(+2.f,-2.f,+2.f,+2.f)});
  vsData.push_back({{},glm::vec4(-z  ,+z  ,-z*e+f,+z)});

  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  uint32_t const expectedCount = 100*100/8*3;
  uint32_t const err           = 2*100;

  if(expectedCount < fsData.size())
    REQUIRE(fsData.size() <= expectedCount + err);
  else
    REQUIRE(fsData.size() >= expectedCount - err);

}

SCENARIO("triangles with 2 vertices behind near plane should be clipped"){
  std::cerr << "24 - clipping - 2 vertices behind near plane" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderDataInject,fragmentShaderDump);
  gpu->bindVertexPuller(vao);


  OutVertex a;
  vsData.clear();
  fsData.clear();
  
  float nnear = 1.f;
  float ffar  = 2.f;
  float e = - (ffar + nnear) / (ffar - nnear);
  float f = - 2.f * ffar * nnear / (ffar - nnear);
  float z = 2.f/3.f;

  vsData.push_back({{},glm::vec4(-z,-z,-z*e+f,+z)});
  vsData.push_back({{},glm::vec4(+z,-z,-z*e+f,+z)});
  vsData.push_back({{},glm::vec4(-2,+2,+2,+2)});

  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  uint32_t const expectedCount = 100*100/8*1;
  uint32_t const err           = 2*100;

  if(expectedCount < fsData.size())
    REQUIRE(fsData.size() <= expectedCount + err);
  else
    REQUIRE(fsData.size() >= expectedCount - err);

}

