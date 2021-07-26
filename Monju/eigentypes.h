#pragma once
#ifndef _MONJU_EIGEN_TYPES_H__
#define _MONJU_EIGEN_TYPES_H__


#include "Eigen/Dense"

namespace monju
{
	/// <summary>�s�x�N�g���i�������j</summary>
	template <typename T>
	using VectorRm = Eigen::Matrix<T, 1, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>��x�N�g���i�c�����j</summary>
	template <typename T>
	using VectorCm = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;

	/// <summary>�s�D��̍s��</summary>
	template <typename T>
	using MatrixRm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>��D��̍s��</summary>
	template <typename T>
	using MatrixCm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;

	// �s��@���
	const int kDensityRectangular = 1;	// �s��
	const int kDensityLowerTriangular = 2;	// ���O�p�s��i�����s��j

	// �s��@�D�����
	const int kRowMajor = 1;
	const int kColMajor = 2;

}

#endif // !_MONJU_EIGEN_TYPES_H__