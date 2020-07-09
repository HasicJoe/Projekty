#include <tests/testCommon.hpp>

#include <glm/glm.hpp>

float const floatErr = 0.001f;

bool equalFloats(float const& a, float const& b,float err) {
  return glm::abs(a - b) <= err;
}

bool equalCounts(size_t a,size_t b,size_t err){
  if(a<b)return (b-a)<err;
  return (a-b)<err;
}

bool greaterFloat(float a,float b,float err){
  return a>b-err;
}
