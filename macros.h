#ifndef MACROS_H_66542242342
#define MACROS_H_66542242342



#include <vector>		//including these is really boring, so they are all here :P
#include <list>
#include <set>
#include <iostream>
#include <memory>
#include <functional>
#include <mutex>
#include <map>
#include <algorithm>
#include <queue>
#include <thread>
#include <atomic>
#include <cassert>




#include <cassert>

#ifndef NDEBUG
#define assertm(condition, message) do { \
if(!(condition)){std::cerr<<"Assert message: "<< message <<std::endl; \
assert((condition));}}while(false)
#else
#   define assertm(condition, message) do {}while(false)
#endif

#endif

