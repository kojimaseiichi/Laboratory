#include "BayesianNodeStat.h"

monju::BayesianNodeStat::BayesianNodeStat(std::string id, int nodes, int units_per_node, int coeWinPenalty, int coeLatPenalty) : _conc(1), _kNodes(nodes), _kUnitsPerNode(units_per_node)
{
	_id = id;
	_win = std::make_shared<MatrixRm<float_t>>(nodes, units_per_node);
	_lat = std::make_shared<MatrixRm<float_t>>(nodes, units_per_node);
	_penalty = std::make_shared<MatrixRm<float_t>>(nodes, units_per_node);
	_coeWinPenalty = coeWinPenalty;
	_coeLatPenalty = coeLatPenalty;
}

monju::BayesianNodeStat::~BayesianNodeStat()
{
	close();
}

std::future<void> monju::BayesianNodeStat::accumulate(MatrixRm<int32_t>& win)
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

std::future<void> monju::BayesianNodeStat::calcPenalty()
{
	PenaltyCalcTask* p = new PenaltyCalcTask(_kNodes, _kUnitsPerNode, _kUnitsPerNode, _storage, _coeWinPenalty, _coeLatPenalty);
	const auto task = [=](PenaltyCalcTask* pr, std::weak_ptr<MatrixRm<float_t>> win, std::weak_ptr<MatrixRm<float_t>> lat, std::weak_ptr<MatrixRm<float_t>> penalty) -> void
	{
		// LOCK -----------------------------
		ReadGuard g(_synch);
		std::unique_ptr<PenaltyCalcTask> p(pr);
		p->calcPenalty(win, lat, penalty);
	};
	return _conc.threadPool().submit(task, p, _win, _lat, _penalty);

}

void monju::BayesianNodeStat::create(std::string dir)
{
	_storage = std::make_shared<TStorage>();
	_storage->create(storagePath(dir), _kNodes, _kUnitsPerNode, _kUnitsPerNode);
	_conc.create();
}

void monju::BayesianNodeStat::close()
{
	_conc.close();
	if (_storage != nullptr)
	{
		_storage->close();
		_storage.reset();
	}
}

std::string monju::BayesianNodeStat::storagePath(std::string dir) const
{
	return util_file::combine(dir, _id + "_tri", "gma");
}
