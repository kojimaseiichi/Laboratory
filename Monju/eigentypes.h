#pragma once
#ifndef _MONJU_EIGEN_TYPES_H__
#define _MONJU_EIGEN_TYPES_H__


#include "Eigen/Dense"

namespace monju
{
	/// <summary>行ベクトル（横方向）</summary>
	template <typename T>
	using VectorRm = Eigen::Matrix<T, 1, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>列ベクトル（縦方向）</summary>
	template <typename T>
	using VectorCm = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;

	/// <summary>行優先の行列</summary>
	template <typename T>
	using MatrixRm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>列優先の行列</summary>
	template <typename T>
	using MatrixCm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;

	// 行列　種類
	const int kDensityRectangular = 1;	// 行列
	const int kDensityLowerTriangular = 2;	// 下三角行列（正方行列）

	// 行列　優先方向
	const int kRowMajor = 1;
	const int kColMajor = 2;

}

#endif // !_MONJU_EIGEN_TYPES_H__