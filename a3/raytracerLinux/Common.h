#pragma once

const float epsilon = 0.0001;
const int recursive_recurrance = 3;
const int anti_aliasing_factor = 0;

#define EPSILON	0.00001


#ifdef _DEBUG
#define ASSERT(x) if(!x) __debugbreak() 
#else
#define ASSERT(x)
#endif