#pragma once

#define EPSILON	0.00001

#define RECURSIVE_RECURRANCE 3

#ifdef _DEBUG
#define ASSERT(x) if(!x) __debugbreak() 
#else
#define ASSERT(x)
#endif