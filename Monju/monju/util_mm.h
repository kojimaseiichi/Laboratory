// �x�N�g�����Z���[�e�B���e�B
#pragma once
#ifndef MONJU_UTIL_MM_H__
#define MONJU_UTIL_MM_H__

#include <intrin.h>

namespace monju {
	namespace util_mm {

		/// <summary>�p�~�\��</summary>
		__int32 __calc_size(__m128i _shape);

		/// <summary>�v�f�ɕ������܂܂�Ă��邩�`�F�b�N</summary>
		bool __check_not_negative(__m128i v);

		// <summary>�X�J���[�P�ʂœ����l���`�F�b�N</summary>
		bool __check_equals(__m128i a, __m128i b);

	} // namespace util_mm
} // namespace monju

#endif // MONJU_UTIL_MM_H__

