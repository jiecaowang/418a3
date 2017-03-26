#pragma once

const double g_epsilon = 0.00001;
const int g_recursive_recurrance = 3;
const int g_anti_aliasing_factor = 0;

#define sqr(a) (a*a)

#ifdef _DEBUG
#define ASSERT(x) if(!x) __debugbreak() 
#else
#define ASSERT(x)
#endif