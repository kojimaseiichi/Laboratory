#pragma once
#ifndef _MONJU_TASK_PENALTY_CALC_H__
#define _MONJU_TASK_PENALTY_CALC_H__

#include "GridMatrixStorage.h"
#include "GridMatrixAccessor.h"

namespace monju {

	/*
	ベイジアンネットの枠組みにおけるモデルの制約を計算します。
	　・一様な勝率となるようなペナルティ
	　・ノード同士が独立となるようなペナルティ
	*/
	class TaskPenaltyCalc
	{
		// 定数
	private:
		const int kNumNodes;
		const int kNumUnits;
		const int kNumActiveNodes;
		const float kCoeWinPenalty;
		const float kCoeLatPenalty;

	private:
		std::weak_ptr<TriangularGridMatrixStorage<int32_t>> _pStorage;

	public:
		TaskPenaltyCalc(
			int num_of_nodes,
			int num_of_units,
			int active_num_of_nodes,
			std::weak_ptr<TriangularGridMatrixStorage<int32_t>> pStorage,
			float coefficient_win_penalty,
			float coefficient_lat_penalty)
			:
			kNumNodes(num_of_nodes),
			kNumUnits(num_of_units),
			kNumActiveNodes(active_num_of_nodes),
			kCoeWinPenalty(coefficient_win_penalty),
			kCoeLatPenalty(coefficient_lat_penalty)
		{
			_pStorage = pStorage;
		}

		TaskPenaltyCalc(const TaskPenaltyCalc&) = delete;
		TaskPenaltyCalc& operator=(const TaskPenaltyCalc&) = delete;

		/// <summary>ペナルティを計算</summary>
		bool calcPenalty(
			std::weak_ptr<MatrixRm<float_t>> winRateRestriction,
			std::weak_ptr<MatrixRm<float_t>> lateralInhibition,
			std::weak_ptr<MatrixRm<float_t>> penalty)
		{
			MatrixRm<int32_t> mpe_count;
			mpe_count.resize(kNumNodes, kNumUnits);

			// MPEカウントをファイルから取得
			_loadWinCount(/*out*/mpe_count);

			// 勝率計算→KL距離計算→勝率ペナルティ計算（ハイパーパラメタ）
			if (auto p = winRateRestriction.lock())
				_calcWinKLDistance(mpe_count, /*out*/*p);
			else
				throw MonjuException(ErrorCode::WeakPointerExpired);

			// 同時勝率計算→相互情報量計算→側抑制ペナルティ計算（ハイパーパラメタ）
			if (auto p = lateralInhibition.lock())
				_calcLateralMutualInfo(/*out*/*p);
			else
				return false;
	
			// ペナルティを計算
			{
				auto
					win = winRateRestriction.lock(),
					lat = lateralInhibition.lock(),
					pe = penalty.lock();
				if (win != nullptr && lat != nullptr && pe != nullptr)
					pe->array() = (-(win->array() * kCoeWinPenalty + lat->array() * kCoeLatPenalty) / (mpe_count.cast<float_t>().array() + 1.f)).exp();
				else
					return false;
			}	
			
			return true;
		}

	private:

		// 対角成分m

		void _loadWinCount(/*out*/MatrixRm<int32_t>& mpe_count)
		{
			if (auto p = _pStorage.lock())
			{
				for (int node = 0; node < kNumNodes; node++)
				{
					for (int unit = 0; unit < kNumUnits; unit++)
						mpe_count.coeffRef(node, unit) = p->readElement(node, node, unit, unit);
				}
			}
			else
				throw MonjuException(ErrorCode::WeakPointerExpired);
		}

		void _calcWinRateDistribution(MatrixRm<int32_t>& mpe_count, /*out*/MatrixRm<float_t>& prob)
		{
			// カウンティングから分布を推定する。
			// mpe_count 行=ノードI、列=ユニットJ
			// Σ_i P(i) = 1
			// P(i,j) = C(i,j) / Σ_i C(i,j))
			prob = mpe_count.cast<float_t>();
			prob.array() += 0.001f;
			prob.array().colwise() /= prob.rowwise().sum().array();
		}

		//void _calcWinKLDistanceWithPhi(
		//	MatrixRm<int32_t>& mpe_count,
		//	MatrixRm<float>& win_prob,
		//	const float coefficient_win_penalty,
		//	/*out*/MatrixRm<float>& message)
		//{
		//	message.resizeLike(win_prob);
		//	message.setZero();

		//	const float Q_fai = _calcWinQDistForFaiUnit();
		//	const float Q = _calcWinQDist();

		//	for (int node = 0; node < kNumNodes; node++)
		//	{
		//		auto row_prob = win_prob.row(node);
		//		auto row_count = mpe_count.row(node);
		//		auto row_message = message.row(node);
		//		// Φ値ユニット
		//		row_message.coeffRef(0) = _calcWinKLDistance(row_prob.coeff(0), Q_fai) * coefficient_win_penalty / (1.f + row_count.coeff(0));
		//		for (int u = 1; u < kNumNodes; u++)
		//			row_message.coeffRef(u) = _calcWinKLDistance(row_prob.coeff(u), Q) * coefficient_win_penalty / (1.f + row_count.coeff(u));
		//	}
		//}

		void _calcWinKLDistance(
			MatrixRm<int32_t>& mpe_count,
			/*out*/MatrixRm<float_t>& message)
		{
			// カウンティングデータから各ノードのKL距離を計算
			// 基準の分布は全ユニットの勝率が同じ

			// ノードごとに各ユニットの勝率を計算
			MatrixRm<float_t> win_prob;
			_calcWinRateDistribution(mpe_count, /*out*/win_prob);


			// KL距離を計算
			// 目標勝率
			const float_t Q = static_cast<float_t>(kNumActiveNodes) / static_cast<float_t>(kNumNodes * kNumUnits);
			message = win_prob;
			message.array() = Q / message.array();
			message.array() = message.array() * message.array().log() * kNumUnits;

			//message.resizeLike(win_prob); // P
			//message.setConstant(Q);
			//message.array() =  message.array() * message.array().log() * kCoeWinPenalty * static_cast<float>(kNumUnits) / (mpe_count.array().cast<float_t>() + 1.f);
		}

		///// <summary>φ値ユニットの勝率の確率分布</summary>
		//float _calcWinQDistForFaiUnit()
		//{
		//	const float n = static_cast<float>(kNumNodes);
		//	const float a = static_cast<float>(kNumActiveNodes);
		//	return (n - a) / n;
		//}

		///// <summary>非φ値ユニットの勝率の確率分布</summary>
		//float _calcWinQDist()
		//{
		//	const float n = static_cast<float>(kNumNodes);
		//	const float a = static_cast<float>(kNumActiveNodes);
		//	const float s = static_cast<float>(kNumUnits) - 1.0f;
		//	return a / (n * s);
		//}

		///// <summary>勝率ペナルティのカルバックライブラー距離計算</summary>
		//float _calcWinKLDistance(const float P, const float Q)
		//{
		//	const float t = Q / P;
		//	return static_cast<float>(kNumUnits)* t* std::log(t);
		//}

		void _calcLateralMutualInfo(
			/*out*/MatrixRm<float_t>& restriction)
		{
			MatrixRm<int32_t> count(kNumNodes, kNumUnits);
			restriction.resizeLike(count);
			restriction.setZero();

			// ノードのすべての組み合わせに対して相互情報量を計算
			for (int U = 0; U < kNumNodes; U++)
			{
				for (int V = 0; V < U; V++)
					_sumLateralMutualInfo(U, V, count, /*out*/restriction);
			}
			// restriction.array() = coefficient_lat_penalty * restriction.array() / (count.array() + 1).cast<float_t>();
		}

		// ノードU、Vの側抑制ペナルティを計算
		void _sumLateralMutualInfo(int U, int V, MatrixRm<int32_t>& count, /*out*/MatrixRm<float>& restriction)
		{
			// カウンティングデータ取得
			if (auto p = _pStorage.lock())
			{
				MatrixRowMajorAccessor<int32_t> a(count);
				p->readCell(a, U, V);
			}
			else
				throw MonjuException(ErrorCode::WeakPointerExpired);

			// カウンティングデータから同時確率分布を計算
			MatrixRm<float_t> mui = count.cast<float_t>();
			mui = mui / mui.sum();
			// 周辺確率分布を計算
			auto margin_prob_U = mui.rowwise().sum(); // P(u)=Σ_v P(u,v)
			auto margin_prob_V = mui.colwise().sum(); // P(v)=Σ_u P(u,v)
			// 同時確率分布を相互情報量で上書き I(U=u,V=v) の行列を計算
			//   s^2(P(u|v)/P(u))(log(P(u|v)/P(u))
			// = s^2(P(u,v)/P(u)P(v))(log(P(u,v)/P(u)P(v))
			mui.array() /= (margin_prob_U * margin_prob_V).array(); // P(u,v)/P(u)P(v)
			mui.array() *= mui.array().log() * std::pow(static_cast<float>(kNumUnits), 2.f);
			restriction.row(U) += mui.rowwise().sum(); // I(U=u,V)
			restriction.row(V) += mui.colwise().sum(); // I(U,V=v)
		}
	};

} // monju

#endif // !_MONJU_TASK_PENALTY_CALC_H_