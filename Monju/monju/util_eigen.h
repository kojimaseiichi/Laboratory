// Eigen向けユーティリティ
#pragma once
#ifndef _MONJU_EIGEN_UTIL_H__
#define _MONJU_EIGEN_UTIL_H__

namespace monju {
	namespace util_eigen {

		/// <summary>行列サイズを指定して、要素を０で初期化</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
		}

		/// <summary>ベクトルのサイズを指定して、要素を０で初期化</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int dim)
		{
			m.resize(dim);
			m.setZero();
		}

		/// <summary>行の列のサイズを指定して、要素を指定したスカラーで初期化</summary>
		template <typename Matrix>
		void init_matrix_set(typename Matrix::Scalar s, Matrix& m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
			m += s;
		}

		/// <summary>ベクトルのサイズを指定して、要素を指定したスカラーで初期化</summary>
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
