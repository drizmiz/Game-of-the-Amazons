
#pragma once

#ifndef __GNUC__
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#endif

#ifndef _PREPROCESSER_ONLY		// to generate a single cpp

#include <cassert>

#include <algorithm>
#include <bitset>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>
#include <array>
#include <chrono>
#include <condition_variable>
#include <forward_list>
#include <future>
#include <initializer_list>
#include <mutex>
#include <random>
#include <regex>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#endif

#ifndef __GNUC__
#pragma warning(disable: 26446)		// gsl::at()
#pragma warning(disable: 26482)		// constexpr array indexs
#pragma warning(disable: 26440)		// make noexcept functions noexcept
#pragma warning(disable: 26496)		// make unchanged variables const
#pragma warning(disable: 26429)		// not_null tag
// #pragma warning(disable: 26481)	// don't decay array to pointer ( as 26485 )
#endif

#define STD ::std::
