#pragma once
#ifndef _MONJU_CORTEX_BASIS_STAT_H__
#define _MONJU_CORTEX_BASIS_STAT_H__

#include "GridMatrixStorage.h"
#include "Closable.h"
#include "ConcurrencyContext.h"
#include "PenaltyCalcTask.h"

namespace monju {

	// äÓíÍÇÃèüó¶Ç…ä÷Ç∑ÇÈìùåvó Çï€ë∂
	class CortexBasisStat : public Closable
	{
	private:
		using TStorage = TriangularGridMatrixStorage<int32_t>;

		std::string _id;
		const int
			_kNodes,
			_kUnitsPerNode;
		std::shared_ptr<TStorage> _storage;
		Synchronizable _synch;
		ConcurrencyContext _conc;
		std::shared_ptr<MatrixRm<float_t>> _win;
		std::shared_ptr<MatrixRm<float_t>> _lat;
		std::shared_ptr<MatrixRm<float_t>> _penalty;

	public:
		CortexBasisStat(const CortexBasisStat&) = delete;
		CortexBasisStat& operator=(const CortexBasisStat&) = delete;

	public:
		MatrixRm<float_t>& win() const { return *_win; }
		MatrixRm<float_t>& lat() const { return *_lat; }
		MatrixRm<float_t>& penalty() const { return *_penalty; }

	public:
		CortexBasisStat(
			std::string id,
			int nodes,
			int units_per_node
		) : _conc(1), _kNodes(nodes), _kUnitsPerNode(units_per_node)
		{
			_id = id;
			_win = std::make_shared<MatrixRm<float_t>>(nodes, units_per_node);
			_lat = std::make_shared<MatrixRm<float_t>>(nodes, units_per_node);
			_penalty = std::make_shared<MatrixRm<float_t>>(nodes, units_per_node);
		}

		~CortexBasisStat()
		{
			close();
		}

		std::future<void> persist(MatrixRm<int32_t>& win)
		{
			MatrixRm<int32_t>* p = new MatrixRm<int32_t>(win);
			const auto task = [=](std::weak_ptr<TStorage> sto, MatrixRm<int32_t>* pr) -> void
			{
				std::unique_ptr<MatrixRm<int32_t>> p(pr);
				if (auto pSto = sto.lock())
				{
					// LOCK -----------------------------
					WriteGuard g(_synch);
					for (int u = 0; u < p->rows(); u++)
					{
						for (int v = 0; v <= u; v++)
							pSto->addElement(u, v, (*p)(u, 0), (*p)(v, 0), 1);
					}
				}
			};
			return _conc.threadPool().submit(task, _storage, p);
		}

		std::future<void> calcPenalty()
		{
			PenaltyCalcTask* p = new PenaltyCalcTask(_kNodes, _kUnitsPerNode, _kUnitsPerNode, _storage, 10.f, 10.f);
			const auto task = [=](PenaltyCalcTask* pr, std::weak_ptr<MatrixRm<float_t>> win, std::weak_ptr<MatrixRm<float_t>> lat, std::weak_ptr<MatrixRm<float_t>> penalty) -> void
			{
				// LOCK -----------------------------
				ReadGuard g(_synch);
				std::unique_ptr<PenaltyCalcTask> p(pr);
				p->calcPenalty(win, lat, penalty);
			};
			return _conc.threadPool().submit(task, p, _win, _lat, _penalty);
			
		}

		void create(std::string dir)
		{
			_storage = std::make_shared<TStorage>();
			_storage->create(storagePath(dir), _kNodes, _kUnitsPerNode, _kUnitsPerNode);
			_conc.create();
		}

		void close()
		{
			_conc.close();
			if (_storage != nullptr)
			{
				_storage->close();
				_storage.reset();
			}
		}

	private:
		std::string storagePath(std::string dir) const
		{
			return util_file::combine(dir, _id + "_tri", "gma");
		}
	};
}

#endif // !_MONJU_CORTEX_BASIS_STAT_H__
