#include "BayesianNodeStat.h"

monju::BayesianNodeStat::BayesianNodeStat(std::string id, UniformBasisShape shape, float_t coeWinPenalty, float_t coeLatPenalty)
	: _conc(1)
{
	_id = id;
	_shape = shape;
	_win = std::make_shared<MatrixRm<float_t>>(shape.nodes, shape.units);
	_lat = std::make_shared<MatrixRm<float_t>>(shape.nodes, shape.units);
	_penalty = std::make_shared<MatrixRm<float_t>>(shape.nodes, shape.units);
	_coeWinPenalty = coeWinPenalty;
	_coeLatPenalty = coeLatPenalty;

	//_conc.create();
}

monju::BayesianNodeStat::~BayesianNodeStat()
{
	close();
}

std::future<void> monju::BayesianNodeStat::accumulate(std::weak_ptr<MatrixRm<int32_t>> win)
{
	auto ptrWin = win.lock();
	auto ptrWinCp = std::make_shared<MatrixRm<int32_t>>(*ptrWin);
	const auto task = [=](std::weak_ptr<TStorage> sto, std::shared_ptr<MatrixRm<int32_t>> w) -> void
	{
		if (auto pSto = sto.lock())
		{
			// LOCK -----------------------------
			WriteGuard g(_synch);
			for (int u = 0; u < w->rows(); u++)
			{
				for (int v = 0; v <= u; v++)
					pSto->addElement(u, v, (*w)(u, 0), (*w)(v, 0), 1);
			}
		}
	};
	return _conc.threadPool().submit(task, _storage, std::forward<std::shared_ptr<MatrixRm<int32_t>>>(ptrWinCp));
}

std::future<void> monju::BayesianNodeStat::calcPenalty()
{
	PenaltyCalculation* p = new PenaltyCalculation(_shape.nodes, _shape.units, _shape.units, _storage, _coeWinPenalty, _coeLatPenalty);
	const auto task = [=](PenaltyCalculation* pr, std::weak_ptr<MatrixRm<float_t>> win, std::weak_ptr<MatrixRm<float_t>> lat, std::weak_ptr<MatrixRm<float_t>> penalty) -> void
	{
		// LOCK -----------------------------
		ReadGuard g(_synch);
		std::unique_ptr<PenaltyCalculation> p(pr);
		p->calcPenalty(win, lat, penalty);
	};
	return _conc.threadPool().submit(task, p, _win, _lat, _penalty);

}

void monju::BayesianNodeStat::create(std::string dir)
{
	_storage = std::make_shared<TStorage>();
	_storage->create(storagePath(dir), _shape.nodes, _shape.units, _shape.units);
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
