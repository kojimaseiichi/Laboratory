// Eigen�������[�e�B���e�B
#pragma once
#ifndef _MONJU_EIGEN_UTIL_H__
#define _MONJU_EIGEN_UTIL_H__

namespace monju {
	namespace util_eigen {

		/// <summary>�s��T�C�Y���w�肵�āA�v�f���O�ŏ�����</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
		}

		/// <summary>�x�N�g���̃T�C�Y���w�肵�āA�v�f���O�ŏ�����</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int dim)
		{
			m.resize(dim);
			m.setZero();
		}

		/// <summary>�s�̗�̃T�C�Y���w�肵�āA�v�f���w�肵���X�J���[�ŏ�����</summary>
		template <typename Matrix>
		void init_matrix_set(typename Matrix::Scalar s, Matrix& m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
			m += s;
		}

		/// <summary>�x�N�g���̃T�C�Y���w�肵�āA�v�f���w�肵���X�J���[�ŏ�����</summary>
		template <typename Matrix>
		void init_matrix_set(typename Matrix::Scalar s, Matrix& m, int dim)
		{
			m.resize(dim);
			m.setZero();
			m += s;
		}
	}
}

#endif // !_MONJU_EIGEN_UTIL_H__
