#pragma once
#ifndef _MONJU_MATRIX_H__
#define _MONJU_MATRIX_H__

#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "nlohmann/json.hpp"
#include <map>

namespace monju {

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

	/// <summary>�s�D��̃X�p�[�X�s��</summary>
	template <typename T>
	using SparseMatrixRm = Eigen::SparseMatrix<T, Eigen::RowMajor>;

	/// <summary>��D��̃X�p�[�X�s��</summary>
	template <typename T>
	using SparseMatrixCm = Eigen::SparseMatrix<T, Eigen::ColMajor>;

	/// <summary>nlohmann��JSON�N���X</summary>
	using json = nlohmann::json;

	using params = std::map<std::string, std::string>;

	struct Extent
	{
		int32_t rows;
		int32_t cols;

		bool operator ==(const Extent& o)
		{
			return rows == o.rows && cols == o.cols;
		}
		int32_t size() const
		{
			return rows * cols;
		}
		int32_t linearId(int32_t row, int32_t col) const
		{
			return row * cols + col;
		}
	};

	struct UniformBasisShape
	{
		Extent extent;
		uint32_t nodes;
		uint32_t units;

		bool checkExtent()
		{
			return nodes == extent.size();
		}
	};

	// �O���錾
	class ClMachine;
	class ClMemory;
	class ClVariableSet;

	class ConvLambdaInput;
}

#endif // !_MONJU_MATRIX_H__

