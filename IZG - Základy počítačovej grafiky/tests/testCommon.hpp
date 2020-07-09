#pragma once

#include<cstddef>

float const extern floatErr;

bool equalFloats(float const& a, float const& b,float err = floatErr);

bool equalCounts(size_t a,size_t b,size_t err = 10);

bool greaterFloat(float a,float b,float err = floatErr);
