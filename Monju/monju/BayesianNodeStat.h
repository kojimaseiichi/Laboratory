#pragma once
#ifndef _MONJU_BAYESIAN_NODE_STAT_H__
#define _MONJU_BAYESIAN_NODE_STAT_H__

#include "GridMatrixStorage.h"
#include "Closable.h"
#include "ConcurrencyContext.h"
#include "PenaltyCalcTask.h"

namespace monju {

	// ���̓��v�f�[�^���t�@�C���ɕۑ�
	class BayesianNodeStat : public Closable
	{
	private:
		using TStorage = TriangularGridMatrixStorage<int32_t>;

		// ID
		std::string _id;
		const int
			_kNodes,			// ��ꓖ����̃m�[�h��
			_kUnitsPerNode;		// �m�[�h������̃��j�b�g��
		// �J�E���e�B���O
		std::shared_ptr<TStorage> _storage;
		std::shared_ptr<MatrixRm<float_t>> 
			_win,			// �����y�i���e�B
			_lat,			// ���}���y�i���e�B
			_penalty;		// �y�i���e�B��
		float_t
			_coeWinPenalty,	// �����y�i���e�B�W��
			_coeLatPenalty;	// ���}���y�i���e�B�W��
		// ����
		Synchronizable _synch;
		// �������s
		ConcurrencyContext _conc;

	public:
		BayesianNodeStat(const BayesianNodeStat&) = delete;
		BayesianNodeStat& operator=(const BayesianNodeStat&) = delete;

	public:
		MatrixRm<float_t>& win() const { return *_win; }
		MatrixRm<float_t>& lat() const { return *_lat; }
		MatrixRm<float_t>& penalty() const { return *_penalty; }

	public:
		BayesianNodeStat(
			std::string id,
			int nodes,
			int unitsPerNode,
			int coeWinPenalty,
			int coeLatPenalty

		);
		~BayesianNodeStat();
		std::future<void> accumulate(MatrixRm<int32_t>& win);
		std::future<void> calcPenalty();
		void create(std::string dir);
		void close();

	private:
		std::string storagePath(std::string dir) const;
	};
}

#endif // !_MONJU_BAYESIAN_NODE_STAT_H__
