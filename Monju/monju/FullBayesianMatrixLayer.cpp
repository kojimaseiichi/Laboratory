#include "FullBayesianMatrixLayer.h"

monju::FullBayesianMatrixLayer::FullBayesianMatrixLayer(std::weak_ptr<Environment> env, std::string id, LayerShape shapeX, LayerShape shapeY)
{
	_env = env.lock();
	_id = id;
	_shapeX = shapeX;
	_shapeY = shapeY;
	_gridExtent = GridExtent(shapeX, shapeY);


	_initHostMemory();
	_setRandom();
}

monju::FullBayesianMatrixLayer::~FullBayesianMatrixLayer()
{

}

void monju::FullBayesianMatrixLayer::initVariables()
{
	_setRandom();
}

void monju::FullBayesianMatrixLayer::store()
{
	_storeMatrices(_dataFileName());
}

void monju::FullBayesianMatrixLayer::load()
{
	_loadMatrices(_dataFileName());
}

bool monju::FullBayesianMatrixLayer::containsNan() const
{
	bool a = util_eigen::contains_nan<MatrixCm<float_t>>(_lambda);
	bool b = util_eigen::contains_nan<MatrixCm<float_t>>(_kappa);
	bool c = util_eigen::contains_nan<MatrixCm<float_t>>(_cpt);
	return a || b || c;
}

void monju::FullBayesianMatrixLayer::copyData(const FullBayesianMatrixLayer& o)
{
	if (_gridExtent != o._gridExtent)
		throw MonjuException();
	*_lambda = *(o._lambda);
	*_kappa = *(o._kappa);
	*_cpt = *(o._cpt);
}

void monju::FullBayesianMatrixLayer::performUp(BelLayer& x, BelLayer& y)
{
	GridExtent gextCpt, gextLambda, gextKappa;
	_makeGridExtent(gextCpt, gextLambda, gextKappa);

	auto pi = y.pi().lock();
	auto la = y.lambda().lock();
	for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
	{
		for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
		{
			auto lo_pi = pi->block(grow, 0, 1, _gridExtent.matrix.rows).reshaped(_gridExtent.matrix.rows, 1);
			auto lo_la = la->block(grow, 0, 1, _gridExtent.matrix.rows).reshaped(_gridExtent.matrix.rows, 1);
			auto lo_kappa = _kappa->block(grow * gextKappa.matrix.size(), gcol, gextKappa.matrix.size(), 1).reshaped(gextKappa.matrix.size(), 1);
			auto lo_lambda = _lambda->block(grow * gextLambda.matrix.size(), gcol, gextLambda.matrix.size(), 1).reshaped(1, gextLambda.matrix.size());

			auto lo_cpt = _cpt->block(grow * gextCpt.matrix.size(), gcol, gextCpt.matrix.size(), 1).reshaped(gextCpt.matrix.rows, gextCpt.matrix.cols);
			auto b = (lo_cpt.colwise() + (lo_pi - lo_kappa).col(0)).array().colwise() * lo_la.col(0).array();
			auto c = b.colwise().sum();
			lo_lambda = c.array() / c.sum();
		}
	}
	auto dla = x.lambda().lock();
	auto r = x.r().lock();
	for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
	{
		auto lambda = _lambda->block(0, gcol, gextLambda.matrix.size() * gextLambda.grid.rows, 1).reshaped(gextLambda.matrix.size(), gextLambda.grid.rows).transpose();
		auto a = lambda(0, 0);
		auto b = lambda(1, 0);
		auto c = lambda(2, 0);
		auto d = lambda(3, 0);

		auto u = lambda.array().colwise().prod() * r->row(gcol).array();
		auto e = u(0, 0);
		auto f = u(0, 1);
		dla->row(gcol) = u;
	}
}

void monju::FullBayesianMatrixLayer::performDown(BelLayer& x, BelLayer& y)
{
	GridExtent gextCpt, gextLambda, gextKappa;
	_makeGridExtent(gextCpt, gextLambda, gextKappa);

	auto rho = x.rho().lock();
	for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
	{
		auto lo_rho = rho->row(gcol);
		for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
		{
			auto lo_lambda = _lambda->block(grow * _gridExtent.matrix.cols, gcol, _gridExtent.matrix.cols, 1).reshaped(1, _gridExtent.matrix.cols).row(0);
			auto lo_cpt = _cpt->block(grow * gextCpt.matrix.size(), gcol, gextCpt.matrix.size(), 1).reshaped(gextCpt.matrix.rows, gextCpt.matrix.cols);
			auto lo_kappa = _kappa->block(grow * _gridExtent.matrix.rows, gcol, _gridExtent.matrix.rows, 1);

			auto a = lo_rho.array() / lo_lambda.array();
			auto pi = a.array() / a.sum();
			auto b = lo_cpt.array().rowwise() * pi;
			lo_kappa = b.rowwise().sum();
		}
	}
	auto pi = y.pi().lock();
	decltype(pi)::element_type cp;
	cp.resizeLike(*pi);
	cp.setZero();
	for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
	{
		auto kappa = _kappa->block(grow * gextKappa.matrix.size(), 0, gextKappa.matrix.size(), gextKappa.grid.cols).reshaped(gextKappa.matrix.size(), gextKappa.grid.cols);
		cp.row(grow) = kappa.rowwise().sum();
	}
	*pi = cp;
}

std::string monju::FullBayesianMatrixLayer::_dataFileName() const
{
	return util_file::combine(_env->info().workFolder, _id, "dbm");
}

void monju::FullBayesianMatrixLayer::_makeGridExtent(GridExtent& cpt, GridExtent& lambda, GridExtent& kappa)
{
	cpt = GridExtent(_shapeX, _shapeY);
	lambda = kappa = cpt;
	lambda.matrix.rows = 1;
	kappa.matrix.cols = 1;
}

void monju::FullBayesianMatrixLayer::_initHostMemory()
{
	GridExtent extCpt, extLambda, extKappa;
	_makeGridExtent(extCpt, extLambda, extKappa);

	_lambda = std::make_shared<MatrixCm<float_t>>();
	_kappa = std::make_shared<MatrixCm<float_t>>();
	_cpt = std::make_shared<MatrixCm<float_t>>();


	auto flatLambda = extLambda.flattenCm();
	_lambda->resize(static_cast<Eigen::Index>(flatLambda.rows), static_cast<Eigen::Index>(flatLambda.cols));
	auto flatKappa = extKappa.flattenCm();
	_kappa->resize(static_cast<Eigen::Index>(flatKappa.rows), static_cast<Eigen::Index>(flatKappa.cols));
	auto flatCpt = extCpt.flattenCm();
	_cpt->resize(static_cast<Eigen::Index>(flatCpt.rows), static_cast<Eigen::Index>(flatCpt.cols));

	{
		GridMatrixStorage storage(_dataFileName());
		_prepareStorage(storage, extCpt, extLambda, extKappa);
	}
}

void monju::FullBayesianMatrixLayer::_prepareStorage(GridMatrixStorage& storage, GridExtent cptExtent, GridExtent lambdaExtent, GridExtent kappaExtent)
{
	storage.prepare<float_t>("lambda", lambdaExtent, kDensityRectangular, kColMajor, kColMajor);
	storage.prepare<float_t>("kappa", kappaExtent, kDensityRectangular, kColMajor, kColMajor);
	storage.prepare<float_t>("cpt", cptExtent, kDensityRectangular, kColMajor, kColMajor);
}

void monju::FullBayesianMatrixLayer::_setRandom()
{
	// ランダムに初期化、確率分布として正規化
	const int kCelMatSize = _gridExtent.matrix.size();
	for (int col = 0; col < _gridExtent.grid.cols; col++)
	{
		for (int row = 0; row < _gridExtent.grid.rows; row++)
		{
			auto blockCpt = _cpt->block(row * kCelMatSize, col, kCelMatSize, 1).reshaped(_gridExtent.matrix.rows, _gridExtent.matrix.cols);
			blockCpt.setRandom();
			blockCpt = blockCpt.array().abs();
			blockCpt.array() /= blockCpt.sum();

			auto blockLambda = _lambda->block(row * _gridExtent.matrix.cols, col, _gridExtent.matrix.cols, 1).reshaped(1, _gridExtent.matrix.cols);
			//blockLambda.setRandom();
			//blockLambda.array().abs();
			blockLambda.setOnes();
			blockLambda.array() /= blockLambda.sum();

			auto blockKappa = _kappa->block(row * _gridExtent.matrix.rows, col, _gridExtent.matrix.rows, 1).reshaped(_gridExtent.matrix.rows, 1);
			//blockKappa.setRandom();
			//blockKappa.array().abs();
			//blockKappa.array() /= blockKappa.sum();
			blockKappa.setZero();
		}
	}
}

void monju::FullBayesianMatrixLayer::_storeMatrices(std::string& dir)
{
	GridExtent extCpt, extLambda, extKappa;
	_makeGridExtent(extCpt, extLambda, extKappa);
	GridMatrixStorage storage(_dataFileName());
	_prepareStorage(storage, extCpt, extLambda, extKappa);

	storage.writeGrid("cpt", *_cpt);
	storage.writeGrid("lambda", *_lambda);
	storage.writeGrid("kappa", *_kappa);
}

void monju::FullBayesianMatrixLayer::_loadMatrices(std::string& dir)
{
	GridMatrixStorage storage(_dataFileName());

	storage.readGrid("cpt", *_cpt);
	storage.readGrid("lambda", *_lambda);
	storage.readGrid("kappa", *_kappa);
}
