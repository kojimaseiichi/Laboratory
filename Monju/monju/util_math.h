#pragma once
#ifndef _MONJU_UTIL_MATH_H__
#define _MONJU_UTIL_MATH_H__

namespace monju {
	namespace util_math {

		template <typename T>
		void intersect(T a, T b, T r)
		{
			std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(r, r.begin()));
		}

		template <typename T>
		bool has_intersection(T a, T b)
		{
			T r;
			intersect(a, b, r);
			return r.size() != 0;
		}
	}
}

#endif // _MONJU_UTIL_MATH_H__

