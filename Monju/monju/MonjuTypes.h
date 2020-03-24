#pragma once
#ifndef _MONJU_MATRIX_H__
#define _MONJU_MATRIX_H__

#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "nlohmann/json.hpp"
#include <map>

namespace monju {

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

	/// <summary>行優先のスパース行列</summary>
	template <typename T>
	using SparseMatrixRm = Eigen::SparseMatrix<T, Eigen::RowMajor>;

	/// <summary>列優先のスパース行列</summary>
	template <typename T>
	using SparseMatrixCm = Eigen::SparseMatrix<T, Eigen::ColMajor>;

	/// <summary>nlohmannのJSONクラス</summary>
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

	// 前方宣言
	class ClMachine;
	class ClMemory;
	class ClVariableSet;

	class ConvLambdaInput;
}

#endif // !_MONJU_MATRIX_H__

