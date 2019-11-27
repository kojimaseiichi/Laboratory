// ベクトル演算ユーティリティ
#pragma once
#ifndef MONJU_UTIL_MM_H__
#define MONJU_UTIL_MM_H__

#include <intrin.h>

namespace monju {
	namespace util_mm {

		/// <summary>廃止予定</summary>
		__int32 __calc_size(__m128i _shape);

		/// <summary>要素に負数が含まれているかチェック</summary>
		bool __check_not_negative(__m128i v);

		// <summary>スカラー単位で同じ値かチェック</summary>
		bool __check_equals(__m128i a, __m128i b);

	} // namespace util_mm
} // namespace monju

#endif // MONJU_UTIL_MM_H__

