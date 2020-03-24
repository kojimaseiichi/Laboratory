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

		float round_n(float number, int n);

		float approx2Exp(float x);

		float approx2Gaussian(float x, float variance);

		template <typename T>
		T clamp(T x, T lowerlimit, T upperlimit)
		{
			if (x <= lowerlimit)
				return lowerlimit;
			if (x >= upperlimit)
				return upperlimit;
			return x;
		}

		template <typename T>
		T smoothstep3(T edge0, T edge1, T x)
		{
			x = clamp<T>((x - edge0) / (edge1 - edge0), 0.0, 1.0);
			return x * x * (3 - 2 * x);
		}
	}
}

#endif // _MONJU_UTIL_MATH_H__

