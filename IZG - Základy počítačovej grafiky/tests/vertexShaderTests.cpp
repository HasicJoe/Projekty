#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>

#include <glm/gtc/matrix_transform.hpp>

size_t vertexShaderInvocationCounter = 0;
void vertexShaderCounter(OutVertex&,InVertex const&in,Uniforms const&){
  vertexShaderInvocationCounter++;
}

void fragmentShaderEmpty(OutFragment&,InFragment const&,Uniforms const&){}

SCENARIO("vertex shader invocation counter tests"){
  std::cerr << "05 - vertex shader invocation counter tests" << std::endl;
  auto gpu = GPU();
  gpu.createFramebuffer(100,100);

  auto vao = gpu.createVertexPuller();
  auto prg = gpu.createProgram();

  gpu.attachShaders(prg,vertexShaderCounter,fragmentShaderEmpty);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);

  gpu.drawTriangles(3);

  REQUIRE(vertexShaderInvocationCounter == 3);

  vertexShaderInvocationCounter = 0;
  gpu.drawTriangles(6);
  REQUIRE(vertexShaderInvocationCounter == 6);

  vertexShaderInvocationCounter = 0;
  gpu.drawTriangles(1800);
  REQUIRE(vertexShaderInvocationCounter == 1800);
}

std::vector<uint32_t>gl_VertexIds;
void vertexShaderID(OutVertex&,InVertex const&in,Uniforms const&){
  gl_VertexIds.push_back(in.gl_VertexID);
}

SCENARIO("gl_VertexID value should contain vertex number when not using indexing"){
  std::cerr << "06 - vertex shader, gl_VertexID (no indexing)" << std::endl;
  auto gpu = GPU();
  gpu.createFramebuffer(100,100);

  auto vao = gpu.createVertexPuller();
  auto prg = gpu.createProgram();

  gpu.attachShaders(prg,vertexShaderID,fragmentShaderEmpty);

  gpu.bindVertexPuller(vao);
  gpu.useProgram(prg);

  uint32_t nofV = 9;

  gl_VertexIds = {};
  gpu.drawTriangles(nofV);

  std::vector<uint32_t>res(nofV);
  std::iota(std::begin(res),std::end(res),0);
  REQUIRE(gl_VertexIds == res);
}


SCENARIO("gl_VertexID value should contain index number when using 32bit indexing"){
  std::cerr << "07 - vertex shader, gl_VertexID (32bit indexing)" << std::endl;

  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<uint32_t> indices = {0,1,2,2,1,3};
  auto const indicesSize = indices.size() * sizeof(decltype(indices)::value_type);

  auto ebo = gpu->createBuffer(indicesSize);
  gpu->setBufferData(ebo,0,indicesSize,indices.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerIndexing(vao,IndexType::UINT32,ebo);

  auto prg = gpu->createProgram();

  gpu->attachShaders(prg,vertexShaderID,fragmentShaderEmpty);

  gpu->bindVertexPuller(vao);
  gpu->useProgram(prg);

  gl_VertexIds = {};
  gpu->drawTriangles(static_cast<uint32_t>(indices.size()));

  gpu = nullptr;

  REQUIRE(gl_VertexIds == indices);
}

SCENARIO("gl_VertexID value should contain index number when using 16bit indexing"){
  std::cerr << "08 - vertex shader, gl_VertexID (16bit indexing)" << std::endl;

  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<uint16_t> indices = {0,1,2,2,1,3};
  auto const indicesSize = indices.size() * sizeof(decltype(indices)::value_type);

  auto ebo = gpu->createBuffer(indicesSize);
  gpu->setBufferData(ebo,0,indicesSize,indices.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerIndexing(vao,IndexType::UINT16,ebo);

  auto prg = gpu->createProgram();

  gpu->attachShaders(prg,vertexShaderID,fragmentShaderEmpty);

  gpu->bindVertexPuller(vao);
  gpu->useProgram(prg);

  gl_VertexIds = {};
  gpu->drawTriangles(static_cast<uint32_t>(indices.size()));

  gpu = nullptr;

  std::vector<uint32_t>indCmp;
  for(auto x:indices)
    indCmp.push_back(x);
  REQUIRE(gl_VertexIds == indCmp);
}

SCENARIO("gl_VertexID value should contain index number when using 8bit indexing"){
  std::cerr << "09 - vertex shader, gl_VertexID (8bit indexing)" << std::endl;

  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<uint8_t> indices = {0,1,2,2,1,3};
  auto const indicesSize = indices.size() * sizeof(decltype(indices)::value_type);

  auto ebo = gpu->createBuffer(indicesSize);
  gpu->setBufferData(ebo,0,indicesSize,indices.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerIndexing(vao,IndexType::UINT8,ebo);

  auto prg = gpu->createProgram();

  gpu->attachShaders(prg,vertexShaderID,fragmentShaderEmpty);

  gpu->bindVertexPuller(vao);
  gpu->useProgram(prg);

  gl_VertexIds = {};
  gpu->drawTriangles(static_cast<uint32_t>(indices.size()));

  gpu = nullptr;

  std::vector<uint32_t>indCmp;
  for(auto x:indices)
    indCmp.push_back(x);
  REQUIRE(gl_VertexIds == indCmp);
}

Uniforms unif;
void vertexShaderUniforms(OutVertex&,InVertex const&,Uniforms const&u){
  unif = u;
}

SCENARIO("vertex shader should recieve uniforms from active shader program"){
  std::cerr << "10 - vertex shader, uniforms" << std::endl;
  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderUniforms,fragmentShaderEmpty);

  gpu->bindVertexPuller(vao);
  gpu->useProgram(prg);

  {
    auto value = 1.f;
    auto u = 0;
    gpu->programUniform1f(prg,u,value);
    gpu->drawTriangles(3);

    REQUIRE(unif.uniform[u].v1 == value);
  }

  {
    auto value = glm::vec2(2.f,3.f);
    auto u = 1;
    gpu->programUniform2f(prg,u,value);
    gpu->drawTriangles(3);

    REQUIRE(unif.uniform[u].v2 == value);
  }

  {
    auto value = glm::vec3(5.f,4.f,3.f);
    auto u = 0;
    gpu->programUniform3f(prg,u,value);
    gpu->drawTriangles(3);

    REQUIRE(unif.uniform[u].v3 == value);
  }

  {
    auto value = glm::vec4(8.f,-1.f,3.f,7.f);
    auto u = 3;
    gpu->programUniform4f(prg,u,value);
    gpu->drawTriangles(3);

    REQUIRE(unif.uniform[u].v4 == value);
  }

  {
    auto value = glm::perspective(1.f,1.3f,13.3f,9.8f);
    auto u = 4;
    gpu->programUniformMatrix4f(prg,u,value);
    gpu->drawTriangles(3);

    REQUIRE(unif.uniform[u].m4 == value);
  }

  {
    auto prg2 = gpu->createProgram();
    auto value = glm::vec4(1.f,1.3f,13.3f,9.8f);
    auto value2 = glm::vec4(8.f,8.f,8.f,8.f);
    auto u = 4;
    gpu->programUniform4f(prg,u,value);
    gpu->programUniform4f(prg2,u,value2);
    gpu->drawTriangles(3);

    REQUIRE(unif.uniform[u].v4 == value);

    gpu->deleteProgram(prg2);
  }
}

std::vector<InVertex>inVertices;
void vertexShaderVert(OutVertex&,InVertex const&i,Uniforms const&){
  inVertices.push_back(i);
}

SCENARIO("vertex shader should receive correct attributes from buffer"){
  std::cerr << "11 - vertex shader, attributes" << std::endl;
  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<float> vert = {0.f,1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f};
  auto vertSize = vert.size()*sizeof(decltype(vert)::value_type);
  auto vbo = gpu->createBuffer(vertSize);
  gpu->setBufferData(vbo,0,vertSize,vert.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerHead(vao,0,AttributeType::FLOAT,sizeof(float),0,vbo);
  gpu->enableVertexPullerHead(vao,0);

  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderVert,fragmentShaderEmpty);
  gpu->bindVertexPuller(vao);

  inVertices.clear();
  gpu->useProgram(prg);
  gpu->drawTriangles(static_cast<uint32_t>(vert.size()));

  gpu = nullptr;

  REQUIRE(inVertices.size() == 12);
  REQUIRE(inVertices[ 0].attributes[0].v1 ==  0.f);
  REQUIRE(inVertices[ 1].attributes[0].v1 ==  1.f);
  REQUIRE(inVertices[ 2].attributes[0].v1 ==  2.f);
  REQUIRE(inVertices[ 3].attributes[0].v1 ==  3.f);
  REQUIRE(inVertices[ 4].attributes[0].v1 ==  4.f);
  REQUIRE(inVertices[ 5].attributes[0].v1 ==  5.f);
  REQUIRE(inVertices[ 6].attributes[0].v1 ==  6.f);
  REQUIRE(inVertices[ 7].attributes[0].v1 ==  7.f);
  REQUIRE(inVertices[ 8].attributes[0].v1 ==  8.f);
  REQUIRE(inVertices[ 9].attributes[0].v1 ==  9.f);
  REQUIRE(inVertices[10].attributes[0].v1 == 10.f);
  REQUIRE(inVertices[11].attributes[0].v1 == 11.f);
}


SCENARIO("vertex shader should receive correct attributes from buffer when using offset"){
  std::cerr << "12 - vertex shader, attributes, offset" << std::endl;
  auto const offsetInFloats = 3;

  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<float> vert = {0.f,1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f};
  auto vertSize = vert.size()*sizeof(decltype(vert)::value_type);
  auto vbo = gpu->createBuffer(vertSize);
  gpu->setBufferData(vbo,0,vertSize,vert.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerHead(vao,0,AttributeType::FLOAT,sizeof(float),sizeof(float)*offsetInFloats,vbo);
  gpu->enableVertexPullerHead(vao,0);

  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderVert,fragmentShaderEmpty);
  gpu->bindVertexPuller(vao);

  inVertices.clear();
  auto vertToDraw = vert.size()-offsetInFloats;
  gpu->useProgram(prg);
  gpu->drawTriangles(static_cast<uint32_t>(vertToDraw));

  gpu = nullptr;

  REQUIRE(inVertices.size() == vertToDraw);
  REQUIRE(inVertices[ 0].attributes[0].v1 == 3.f );
  REQUIRE(inVertices[ 1].attributes[0].v1 == 4.f );
  REQUIRE(inVertices[ 2].attributes[0].v1 == 5.f );
  REQUIRE(inVertices[ 3].attributes[0].v1 == 6.f );
  REQUIRE(inVertices[ 4].attributes[0].v1 == 7.f );
  REQUIRE(inVertices[ 5].attributes[0].v1 == 8.f );
  REQUIRE(inVertices[ 6].attributes[0].v1 == 9.f );
  REQUIRE(inVertices[ 7].attributes[0].v1 == 10.f);
  REQUIRE(inVertices[ 8].attributes[0].v1 == 11.f);
}

SCENARIO("vertex shader should receive correct attributes from buffer when using stride"){
  std::cerr << "13 - vertex shader, attributes, stride" << std::endl;
  auto const strideInFloats = 4;

  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<float> vert = {0.f,1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f};
  auto vertSize = vert.size()*sizeof(decltype(vert)::value_type);
  auto vbo = gpu->createBuffer(vertSize);
  gpu->setBufferData(vbo,0,vertSize,vert.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerHead(vao,0,AttributeType::FLOAT,sizeof(float)*strideInFloats,0,vbo);
  gpu->enableVertexPullerHead(vao,0);

  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderVert,fragmentShaderEmpty);
  gpu->bindVertexPuller(vao);

  inVertices.clear();
  auto vertToDraw = vert.size()/strideInFloats;
  gpu->useProgram(prg);
  gpu->drawTriangles(static_cast<uint32_t>(vertToDraw));

  gpu = nullptr;

  REQUIRE(inVertices.size() == vertToDraw);
  REQUIRE(inVertices[ 0].attributes[0].v1 == 0.f );
  REQUIRE(inVertices[ 1].attributes[0].v1 == 4.f );
  REQUIRE(inVertices[ 2].attributes[0].v1 == 8.f );
}

SCENARIO("vertex shader should receive correct attributes from buffer when using offset and stride"){
  std::cerr << "14 - vertex shader, attributes, offset, stride" << std::endl;
  auto const strideInFloats = 4;
  auto const offsetInFloats = 2;

  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<float> vert = {0.f,1.f,2.f,3.f,4.f,5.f,6.f,7.f,8.f,9.f,10.f,11.f};
  auto vertSize = vert.size()*sizeof(decltype(vert)::value_type);
  auto vbo = gpu->createBuffer(vertSize);
  gpu->setBufferData(vbo,0,vertSize,vert.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerHead(vao,0,AttributeType::FLOAT,sizeof(float)*strideInFloats,sizeof(float)*offsetInFloats,vbo);
  gpu->enableVertexPullerHead(vao,0);

  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderVert,fragmentShaderEmpty);
  gpu->bindVertexPuller(vao);

  inVertices.clear();
  auto vertToDraw = vert.size()/strideInFloats;
  gpu->useProgram(prg);
  gpu->drawTriangles(static_cast<uint32_t>(vertToDraw));

  gpu = nullptr;

  REQUIRE(inVertices.size() == vertToDraw);
  REQUIRE(inVertices[ 0].attributes[0].v1 ==  2.f );
  REQUIRE(inVertices[ 1].attributes[0].v1 ==  6.f );
  REQUIRE(inVertices[ 2].attributes[0].v1 == 10.f );
}


SCENARIO("vertex shader should receive correct attributes when using multiple buffers"){
  std::cerr << "15 - vertex shader, attributes, offset, stride, multiple buffers" << std::endl;
  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  uint8_t b0[400];
  uint8_t b1[400];

  memset(b0,0,sizeof(b0));
  memset(b1,0,sizeof(b1));

  ((float*)(b0+2))[0] = 100.f;
  ((float*)(b0+2))[1] = 101.f;
  ((float*)(b0+2))[2] = 102.f;
  ((float*)(b0+2))[3] = 103.f;
  ((float*)(b0+2))[4] = 104.f;
  ((float*)(b0+2))[5] = 105.f;
  ((float*)(b0+2))[6] = 106.f;

  ((float*)(b1+7))[ 0] = 10.f;
  ((float*)(b1+7))[ 1] = 11.f;
  ((float*)(b1+7))[ 2] = 12.f;
  ((float*)(b1+7))[ 3] = 13.f;
  ((float*)(b1+7))[ 4] =  0.f;
  ((float*)(b1+7))[ 5] = 14.f;
  ((float*)(b1+7))[ 6] = 15.f;
  ((float*)(b1+7))[ 7] = 16.f;
  ((float*)(b1+7))[ 8] = 17.f;
  ((float*)(b1+7))[ 9] =  0.f;
  ((float*)(b1+7))[10] = 18.f;
  ((float*)(b1+7))[11] = 19.f;
  ((float*)(b1+7))[12] = 20.f;
  ((float*)(b1+7))[13] = 21.f;

  BufferID vbo0 = gpu->createBuffer(sizeof(b0));
  gpu->setBufferData(vbo0,0,sizeof(b0),b0);

  BufferID vbo1 = gpu->createBuffer(sizeof(b1));
  gpu->setBufferData(vbo1,0,sizeof(b1),b1);


  auto vao = gpu->createVertexPuller();

  gpu->setVertexPullerHead(vao,5,AttributeType::VEC3,sizeof(float)*2,2,vbo0);
  gpu->enableVertexPullerHead(vao,5);

  gpu->setVertexPullerHead(vao,3,AttributeType::VEC4,sizeof(float)*5,7,vbo1);
  gpu->enableVertexPullerHead(vao,3);

  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderVert,fragmentShaderEmpty);
  gpu->bindVertexPuller(vao);

  inVertices.clear();
  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  REQUIRE(inVertices.size() == 3);
  REQUIRE(inVertices[0].attributes[5].v3 == glm::vec3(100.f,101.f,102.f  ));
  REQUIRE(inVertices[0].attributes[3].v4 == glm::vec4(10.f,11.f,12.f,13.f));

  REQUIRE(inVertices[1].attributes[5].v3 == glm::vec3(102.f,103.f,104.f  ));
  REQUIRE(inVertices[1].attributes[3].v4 == glm::vec4(14.f,15.f,16.f,17.f));

  REQUIRE(inVertices[2].attributes[5].v3 == glm::vec3(104.f,105.f,106.f  ));
  REQUIRE(inVertices[2].attributes[3].v4 == glm::vec4(18.f,19.f,20.f,21.f));
}

SCENARIO("vertex shader should receive correct attributes when using offset and stride and indexing"){
  std::cerr << "16 - vertex shader, attributes, offset, stride, indexing" << std::endl;
  auto gpu = std::make_shared<GPU>();
  gpu->createFramebuffer(100,100);

  std::vector<float> vert = {0.f,1.f,2.f,3.f};
  auto vertSize = vert.size()*sizeof(decltype(vert)::value_type);
  BufferID vbo = gpu->createBuffer(vertSize);
  gpu->setBufferData(vbo,0,vertSize,vert.data());

  std::vector<uint16_t> indices = {0,1,2,2,1,3};
  auto indicesSize = indices.size()*sizeof(decltype(indices)::value_type);
  BufferID ebo = gpu->createBuffer(indicesSize);
  gpu->setBufferData(ebo,0,indicesSize,indices.data());

  auto vao = gpu->createVertexPuller();
  gpu->setVertexPullerHead(vao,0,AttributeType::FLOAT,sizeof(float),0,vbo);
  gpu->enableVertexPullerHead(vao,0);
  gpu->setVertexPullerIndexing(vao,IndexType::UINT16,ebo);

  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderVert,fragmentShaderEmpty);
  gpu->bindVertexPuller(vao);

  inVertices.clear();
  gpu->useProgram(prg);
  gpu->drawTriangles(static_cast<uint32_t>(indices.size()));

  gpu = nullptr;

  REQUIRE(inVertices.size() == 6);
  REQUIRE(inVertices[0].attributes[0].v1 == 0.f);
  REQUIRE(inVertices[1].attributes[0].v1 == 1.f);
  REQUIRE(inVertices[2].attributes[0].v1 == 2.f);
  REQUIRE(inVertices[3].attributes[0].v1 == 2.f);
  REQUIRE(inVertices[4].attributes[0].v1 == 1.f);
  REQUIRE(inVertices[5].attributes[0].v1 == 3.f);

}
