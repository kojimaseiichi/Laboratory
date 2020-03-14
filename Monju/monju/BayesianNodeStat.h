#pragma once
#ifndef _MONJU_BAYESIAN_NODE_STAT_H__
#define _MONJU_BAYESIAN_NODE_STAT_H__

#include "GridMatrixStorage.h"
#include "Closable.h"
#include "ConcurrencyContext.h"
#include "PenaltyCalculation.h"

namespace monju {

	// 基底の統計データをファイルに保存
	class BayesianNodeStat : public Closable
	{
	private:
		using TStorage = TriangularGridMatrixStorage<int32_t>;

		// ID
		std::string _id;
		UniformBasisShape _shape;
		// カウンティング
		std::shared_ptr<TStorage> _storage;
		std::shared_ptr<MatrixRm<float_t>> 
			_win,			// 勝率ペナルティ
			_lat,			// 側抑制ペナルティ
			_penalty;		// ペナルティλ
		float_t
			_coeWinPenalty,	// 勝率ペナルティ係数
			_coeLatPenalty;	// 側抑制ペナルティ係数
		// 同期
		Synchronizable _synch;
		// 同時実行
		ConcurrencyContext _conc;

	public:
		std::weak_ptr<MatrixRm<float_t>> win() const { return _win; }
		std::weak_ptr<MatrixRm<float_t>> lat() const { return _lat; }
		std::weak_ptr<MatrixRm<float_t>> penalty() const { return _penalty; }

	public:
		BayesianNodeStat(
			std::string id,
			UniformBasisShape shape,
			float_t coeWinPenalty,
			float_t coeLatPenalty

		);
		~BayesianNodeStat();
		std::future<void> accumulate(std::weak_ptr<MatrixRm<int32_t>> win);
		std::future<void> calcPenalty();
		void create(std::string dir);
		void close();

	private:
		std::string storagePath(std::string dir) const;

		// コピー禁止・ムーブ禁止
	public:
		BayesianNodeStat(const BayesianNodeStat&) = delete;
		BayesianNodeStat(BayesianNodeStat&&) = delete;
		BayesianNodeStat& operator =(const BayesianNodeStat&) = delete;
		BayesianNodeStat& operator =(BayesianNodeStat&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_NODE_STAT_H__
