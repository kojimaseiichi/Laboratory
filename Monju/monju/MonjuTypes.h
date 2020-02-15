#pragma once
#ifndef _MONJU_MATRIX_H__
#define _MONJU_MATRIX_H__

#include "Eigen/Dense"
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

	/// <summary>nlohmannのJSONクラス</summary>
	using json = nlohmann::json;

	using params = std::map<std::string, std::string>;

	// 破棄予定
	union BasisWorkItemCalcPlan
	{
		int64_t n;
		struct {
			uint32_t adjacent_index; // 各上位基底ノード（列）の隣接する下位基底ノード（行）のインデックス（ID）
			uint32_t index; // ローカルワークアイテムのインデックス
		} plan;
	};
}

#endif // !_MONJU_MATRIX_H__

