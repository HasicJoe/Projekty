#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <glm/gtc/matrix_transform.hpp>

#include <student/gpu.hpp>
#include <tests/testCommon.hpp>

void vertexShaderTri(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&){
  if(inVertex.gl_VertexID == 0)outVertex.gl_Position = glm::vec4(-1.f,-1.f,-1.f,1.f);
  if(inVertex.gl_VertexID == 1)outVertex.gl_Position = glm::vec4(+1.f,-1.f,-1.f,1.f);
  if(inVertex.gl_VertexID == 2)outVertex.gl_Position = glm::vec4(-1.f,+1.f,-1.f,1.f);
}

size_t fragmentShaderInvocationCounter = 0;
void fragmentShaderCounter(OutFragment&,InFragment const&,Uniforms const&){
  fragmentShaderInvocationCounter++;
}

SCENARIO("fragment shader should be executed roughly pixelCount/2 when rasterizing triangle that covers half of the screen"){
  std::cerr << "17 - rasterization should produce correct number of fragments" << std::endl;
  auto gpu = std::make_shared<GPU>();
  auto w = 100;
  uint32_t h = 100;
  gpu->createFramebuffer(w,h);

  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderTri,fragmentShaderCounter);
  gpu->bindVertexPuller(vao);

  fragmentShaderInvocationCounter=0;
  gpu->useProgram(prg);

  gpu->clear(0,0,0,1);
  gpu->drawTriangles(3);


  uint32_t expectedCount = w*h/2;
  uint32_t err = w;

  if(expectedCount < fragmentShaderInvocationCounter)
    REQUIRE(fragmentShaderInvocationCounter <= expectedCount + err);
  else
    REQUIRE(fragmentShaderInvocationCounter >= expectedCount - err);
}

Uniforms fUnif;

void fragmentShaderUnif(OutFragment&,InFragment const&,Uniforms const&u){
  fUnif = u;
}

SCENARIO("fragment shader should receive correct uniform variables from active shader program"){
  std::cerr << "18 - fragment shader uniforms" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);

  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderTri,fragmentShaderUnif);
  gpu->bindVertexPuller(vao);

  gpu->useProgram(prg);
  auto const value =glm::ortho(1.f,2.f,3.f,5.f,6.f,8.f);
  auto const u = 3;
  gpu->programUniformMatrix4f(prg,u,value);
  gpu->drawTriangles(3);

  gpu = nullptr;

  REQUIRE(fUnif.uniform[u].m4 == value);
}

void vertexShaderPD(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&){
  float f0 = 0.1f;
  float f1 = 0.3f;
  float f2 = 4.f;
  if(inVertex.gl_VertexID == 0)outVertex.gl_Position = glm::vec4(-1.f,-1.f,-1.f,1.f)*f0;
  if(inVertex.gl_VertexID == 1)outVertex.gl_Position = glm::vec4(+1.f,-1.f,-1.f,1.f)*f1;
  if(inVertex.gl_VertexID == 2)outVertex.gl_Position = glm::vec4(-1.f,+1.f,-1.f,1.f)*f2;
}

SCENARIO("perspective division should be performed"){
  std::cerr << "19 - perspective division" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderPD,fragmentShaderCounter);
  gpu->bindVertexPuller(vao);

  gpu->useProgram(prg);
  fragmentShaderInvocationCounter = 0;
  gpu->drawTriangles(3);
  gpu = nullptr;

  uint32_t expectedCount = w*h/2;
  uint32_t err = w;
  if(expectedCount < fragmentShaderInvocationCounter)
    REQUIRE(fragmentShaderInvocationCounter <= expectedCount + err);
  else
    REQUIRE(fragmentShaderInvocationCounter >= expectedCount - err);
}


std::vector<glm::vec4>triVertices;

void initTriVertices(){
  triVertices.clear();
  triVertices.push_back(glm::vec4(-1.f,-1.f,0.f,1.f));
  triVertices.push_back(glm::vec4(+1.f,-1.f,0.f,1.f));
  triVertices.push_back(glm::vec4(-1.f,+1.f,0.f,1.f));
}

void vertexShaderInterp(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&){
  if(inVertex.gl_VertexID == 0){
    outVertex.gl_Position = triVertices.at(0);
    outVertex.attributes[0].v3 = glm::vec3(1.f,0.f,0.f);
  }
  if(inVertex.gl_VertexID == 1){
    outVertex.gl_Position = triVertices.at(1);
    outVertex.attributes[0].v3 = glm::vec3(0.f,1.f,0.f);
  }
  if(inVertex.gl_VertexID == 2){
    outVertex.gl_Position = triVertices.at(2);
    outVertex.attributes[0].v3 = glm::vec3(0.f,0.f,1.f);
  }
}


std::vector<glm::vec2>samplingLocations;

size_t whichSample(glm::vec4 const&coord){
  //std::cerr << "x: " << coord.data[0] << " y: " << coord.data[1] << std::endl;
  for(size_t i=0;i<samplingLocations.size();++i){
    if(
      equalFloats(coord[0] , samplingLocations.at(i)[0] + 0.5f) &&
      equalFloats(coord[1] , samplingLocations.at(i)[1] + 0.5f)
    )return i;
  }
  return samplingLocations.size()+1;
}

std::vector<size_t>hitSamples;
std::vector<glm::vec3>sampleColors;
std::vector<glm::vec4>fragCoords;
void fragmentShaderInterp(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&){
  size_t id = whichSample(inFragment.gl_FragCoord);
  if(id < samplingLocations.size()){
    hitSamples.push_back(id);
    glm::vec3 col = inFragment.attributes[0].v3;
    sampleColors.push_back(col);
    fragCoords.push_back(inFragment.gl_FragCoord);
  }
}

SCENARIO("rasterization should create fragments that are inside triangle (gl_FragCoord.xy should contain correct screen space position)"){
  std::cerr << "20 - fragments inside triangle with correct gl_FragCoord.xy" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderInterp,fragmentShaderInterp);
  gpu->bindVertexPuller(vao);

  
  samplingLocations.clear();
  samplingLocations.push_back({0 ,0 });
  samplingLocations.push_back({99,0 });
  samplingLocations.push_back({0 ,98});
  samplingLocations.push_back({90,5 });
  samplingLocations.push_back({33,33});
  samplingLocations.push_back({33,40});
  samplingLocations.push_back({40,33});
  hitSamples.clear();
  sampleColors.clear();
  fragCoords.clear();
  initTriVertices();
  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  REQUIRE(hitSamples.size() == samplingLocations.size());
}

SCENARIO("rasterization should not create fragments that are outside triangle (gl_FragCoord.xy should contain correct screen space position)"){
  std::cerr << "21 - fragments outside triangle" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderInterp,fragmentShaderInterp);
  gpu->bindVertexPuller(vao);


  samplingLocations.clear();
  samplingLocations.push_back({99,1 });
  samplingLocations.push_back({1 ,99});
  samplingLocations.push_back({51,51});
  samplingLocations.push_back({60,60});
  samplingLocations.push_back({60,70});
  samplingLocations.push_back({70,60});
  samplingLocations.push_back({99,99});
  samplingLocations.push_back({10,10});

  hitSamples.clear();
  sampleColors.clear();
  fragCoords.clear();
  initTriVertices();
  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  REQUIRE(hitSamples.size() == 1);

}



SCENARIO("rasterization should interpolate vertex attributes using barycentric coordinates"){
  std::cerr << "22 - vertex attributes interpolated to fragment attributes" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderInterp,fragmentShaderInterp);
  gpu->setVS2FSType(prg,0,AttributeType::VEC3);
  gpu->bindVertexPuller(vao);

  glm::vec2 fragCoord = {30.f,20.f};

  samplingLocations.clear();
  samplingLocations.push_back(fragCoord);
  hitSamples.clear();
  sampleColors.clear();
  fragCoords.clear();
  initTriVertices();
  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  gpu = nullptr;

  REQUIRE(hitSamples.size() == samplingLocations.size());
  float area = w*h/2.f;
  float l2 = ((fragCoord[1]+.5f)*w/2.f) / area;
  float l1 = ((fragCoord[0]+.5f)*h/2.f) / area;
  float l0 = 1.f - l1 - l2;
  REQUIRE(equalFloats(sampleColors.at(0)[0],l0));
  REQUIRE(equalFloats(sampleColors.at(0)[1],l1));
  REQUIRE(equalFloats(sampleColors.at(0)[2],l2));

}

SCENARIO("rasterization should interpolate vertex attributes using barycentric coordinates with perspective correction and interpolate correct fragment depth"){
  std::cerr << "23 - perspective correct interpolation of vertex attributes to fragment attributes" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderInterp,fragmentShaderInterp);
  gpu->setVS2FSType(prg,0,AttributeType::VEC3);
  gpu->bindVertexPuller(vao);

  glm::vec2 fragCoord = {30.f,20.f};

  samplingLocations.clear();
  samplingLocations.push_back(fragCoord);
  hitSamples.clear();
  sampleColors.clear();
  fragCoords.clear();

  float hc[3] = {1,2,.5};
  float zz[3] = {.9f,.4f,.8f};
  triVertices.clear();
  triVertices.push_back({-hc[0],-hc[0],zz[0],hc[0]});
  triVertices.push_back({+hc[1],-hc[1],zz[1],hc[1]});
  triVertices.push_back({-hc[2],+hc[2],zz[2],hc[2]});

  gpu->useProgram(prg);
  gpu->drawTriangles(3);
  gpu = nullptr;

  REQUIRE(hitSamples.size() == samplingLocations.size());
  float area = w*h/2.f;
  float l2 = ((fragCoord[1]+.5f)*w/2.f) / area;
  float l1 = ((fragCoord[0]+.5f)*h/2.f) / area;
  float l0 = 1.f - l1 - l2;
  float divisor = l0/hc[0] + l1/hc[1] + l2/hc[2];
  REQUIRE(equalFloats(sampleColors.at(0)[0],l0/hc[0]/divisor));
  REQUIRE(equalFloats(sampleColors.at(0)[1],l1/hc[1]/divisor));
  REQUIRE(equalFloats(sampleColors.at(0)[2],l2/hc[2]/divisor));

  REQUIRE(equalFloats(fragCoords.at(0)[2],
        (zz[0]/hc[0]*l0/hc[0] + zz[1]/hc[1]*l1/hc[1] + zz[2]/hc[2]*l2/hc[2])/divisor));
}


void fragmentShaderWhite(OutFragment&outFragment,InFragment const&,Uniforms const&){
  outFragment.gl_FragColor = glm::vec4(1.f);
}

SCENARIO("per fragment operations should eliminate fragments that do not pass depth test"){
  std::cerr << "24 - depth test" << std::endl;
  auto gpu = std::make_shared<GPU>();
  uint32_t w=100;
  uint32_t h=100;
  gpu->createFramebuffer(w,h);
  auto vao = gpu->createVertexPuller();
  auto prg = gpu->createProgram();
  gpu->attachShaders(prg,vertexShaderTri,fragmentShaderWhite);
  gpu->bindVertexPuller(vao);
  
  auto fcolor = gpu->getFramebufferColor();
  auto fdepth = gpu->getFramebufferDepth();

  REQUIRE(fcolor != nullptr);
  REQUIRE(fdepth != nullptr);
  //clear framebuffer
  for(uint32_t y=0;y<h;++y)
    for(uint32_t x=0;x<w;++x){
      uint32_t p = y*w+x;
      for(int i=0;i<4;++i)
        fcolor[p*4+i] = 0;
      fdepth[p] = +1.f;
    }

  //set one pixel depth to near plane
  fdepth[10*w+10] = -1.f;

  gpu->useProgram(prg);
  gpu->drawTriangles(3);

  std::vector<uint8_t>color;
  std::vector<float>depth;

  for(uint32_t i=0;i<w*h;++i){
    for(int k=0;k<4;++k)
    color.push_back(fcolor[i*4+k]);
    depth.push_back(fdepth[i]);
  }

  gpu = nullptr;

  //point outside of triangle
  REQUIRE(equalFloats(depth[80*w+80],+1.f));
  REQUIRE(equalFloats(depth[80*w+80],+1.f));
  REQUIRE(equalFloats(depth[80*w+80],+1.f));

  REQUIRE(color[(80*w+80)*4+0]==0);
  REQUIRE(color[(80*w+80)*4+1]==0);
  REQUIRE(color[(80*w+80)*4+2]==0);

  //point inside of triangle
  REQUIRE(equalFloats(depth[20*w+20],-1.f));
  REQUIRE(equalFloats(depth[20*w+20],-1.f));
  REQUIRE(equalFloats(depth[20*w+20],-1.f));

  REQUIRE(color[(20*w+20)*4+0]==255);
  REQUIRE(color[(20*w+20)*4+1]==255);
  REQUIRE(color[(20*w+20)*4+2]==255);

  //point inside of triangle (with modified depth)
  REQUIRE(equalFloats(depth[10*w+10],-1.f));
  REQUIRE(equalFloats(depth[10*w+10],-1.f));
  REQUIRE(equalFloats(depth[10*w+10],-1.f));

  REQUIRE(color[(10*w+10)*4+0]==0.f);
  REQUIRE(color[(10*w+10)*4+1]==0.f);
  REQUIRE(color[(10*w+10)*4+2]==0.f);

}
