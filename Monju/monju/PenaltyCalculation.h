#pragma once
#ifndef _MONJU_PENALTY_CALCULATION_H__
#define _MONJU_PENALTY_CALCULATION_H__

#include "GridMatrixStorage.h"
#include "GridMatrixAccessor.h"

namespace monju {

	// ベイジアンネットの枠組みにおけるモデルの制約を計算します。
	// ・一様な勝率となるようなペナルティ
	// ・ノード同士が独立となるようなペナルティ
	class PenaltyCalculation
	{
		// 定数
	private:
		const int kNumNodes;
		const int kNumUnits;
		const int kNumActiveNodes;
		float _coeWinPenalty;
		float _coeLatPenalty;

	private:
		std::weak_ptr<TriangularGridMatrixStorage<int32_t>> _pStorage;

	public:
		PenaltyCalculation(
			int num_of_nodes,
			int num_of_units,
			int active_num_of_nodes,
			std::weak_ptr<TriangularGridMatrixStorage<int32_t>> pStorage,
			float coefficient_win_penalty,
			float coefficient_lat_penalty);

		bool calcPenalty(
			std::weak_ptr<MatrixRm<float_t>> winRateRestriction,
			std::weak_ptr<MatrixRm<float_t>> lateralInhibition,
			std::weak_ptr<MatrixRm<float_t>> penalty);

	private:

		void _calcExponentialPenalty(
			MatrixRm<int32_t>& mpe_count,
			std::weak_ptr<MatrixRm<float_t>>& winRateRestriction,
			std::weak_ptr<MatrixRm<float_t>>& lateralInhibition,
			std::weak_ptr<MatrixRm<float_t>>& penalty);

		void _loadWinCount(/*out*/MatrixRm<int32_t>& mpe_count);

		void _calcWinRateDistribution(MatrixRm<int32_t>& mpe_count, /*out*/MatrixRm<float_t>& prob);

		void _calcWinKLDistance(
			MatrixRm<int32_t>& mpe_count,
			/*out*/MatrixRm<float_t>& message);

		void _calcLateralMutualInfo(
			/*out*/MatrixRm<float_t>& restriction);

		// ノードU、Vの側抑制ペナルティを計算
		void _sumLateralMutualInfo(int U, int V, MatrixRm<int32_t>& count, /*out*/MatrixRm<float>& restriction);

		// コピー禁止・ムーブ禁止
	public:
		PenaltyCalculation(const PenaltyCalculation&) = delete;
		PenaltyCalculation(PenaltyCalculation&&) = delete;
		PenaltyCalculation& operator =(const PenaltyCalculation&) = delete;
		PenaltyCalculation& operator =(PenaltyCalculation&&) = delete;
	};

} // monju

#endif // !_MONJU_PENALTY_CALCULATION_H__