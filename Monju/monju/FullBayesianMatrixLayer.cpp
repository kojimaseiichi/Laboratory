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
			blockCpt.array().abs();
			blockCpt.array() /= blockCpt.sum();

			auto blockLambda = _lambda->block(row * _gridExtent.matrix.cols, col, _gridExtent.matrix.cols, 1).reshaped(1, _gridExtent.matrix.cols);
			blockLambda.setRandom();
			blockLambda.array().abs();
			blockLambda.array() /= blockLambda.sum();

			auto blockKappa = _kappa->block(row * _gridExtent.matrix.rows, col, _gridExtent.matrix.rows, 1).reshaped(_gridExtent.matrix.rows, 1);
			blockKappa.setRandom();
			blockKappa.array().abs();
			blockKappa.array() /= blockKappa.sum();
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
