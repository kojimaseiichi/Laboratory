#include "BayesianNodeStat.h"

#include "Synchronizable.h"
#include "util_file.h"

monju::BayesianNodeStat::BayesianNodeStat(std::string id, monju::LayerStruct shape, float_t coeWinPenalty, float_t coeLatPenalty)
	: _conc(1)
{
	_id = id;
	_shape = shape;
	_win = std::make_shared<MatrixRm<float_t>>(shape.nodes.size(), shape.units.size());
	_lat = std::make_shared<MatrixRm<float_t>>(shape.nodes.size(), shape.units.size());
	_penalty = std::make_shared<MatrixRm<float_t>>(shape.nodes.size(), shape.units.size());
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
	const auto task = [=](std::weak_ptr<GridMatrixStorage> sto, std::shared_ptr<MatrixRm<int32_t>> w) -> void
	{
		auto inc = [](const int32_t val)->int32_t { return val + 1; };
		if (auto pSto = sto.lock())
		{
			// LOCK -----------------------------
			WriteGuard g(_synch);
			for (int u = 0; u < w->rows(); u++)
			{
				for (int v = 0; v <= u; v++)
					pSto->coeffOp<int32_t>(_kDataCount, u, v, (*w)(u, 0), (*w)(v, 0), inc);
			}
		}
	};
	return _conc.threadPool().submit(task, _storage, std::forward<std::shared_ptr<MatrixRm<int32_t>>>(ptrWinCp));
}

std::future<void> monju::BayesianNodeStat::calcPenalty()
{
	PenaltyCalculation* p = new PenaltyCalculation(_shape.nodes.size(), _shape.units.size(), _shape.units.size(), _storage, _coeWinPenalty, _coeLatPenalty);
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
	_storage = std::make_shared<GridMatrixStorage>(storagePath(dir));
	GridExtent extent = _shape.asGridExtent();
	_storage->prepare<float_t>(_kDataCount, extent, kDensityRectangular, kRowMajor, kRowMajor);
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
