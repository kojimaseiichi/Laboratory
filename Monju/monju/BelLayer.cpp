#include "BelLayer.h"
#include "GridMatrixStorage.h"

// コンストラクタ

monju::BelLayer::BelLayer(std::weak_ptr<Environment> env, std::string id, monju::LayerStruct shape)
{
	_env = env.lock();
	_id = id;
	_shape = shape;

	_initHostMemory();
}

monju::BelLayer::~BelLayer()
{
}

void monju::BelLayer::initVariables()
{
	// λ、ρ、BELを乱数で初期化
	// Rを1で初期化
	// WINを0で初期化
	_setRandomProb(_lambda);
	_setRandomProb(_pi);
	_rho->array() = _lambda->array() * _pi->array();
	_r->setOnes();
	*_bel = _rho->array().colwise() / _rho->array().rowwise().sum();
	_win->setZero();
}

void monju::BelLayer::store()
{
	GridMatrixStorage storage(_dataFileName());
	_prepareStorage(storage);
	storage.writeMatrix("lambda", *_lambda);
	storage.writeMatrix("pi", *_pi);
	storage.writeMatrix("rho", *_rho);
	storage.writeMatrix("r", *_r);
	storage.writeMatrix("bel", *_bel);
	storage.writeMatrix("win", *_win);
}

void monju::BelLayer::load()
{
	GridMatrixStorage storage(_dataFileName());
	_prepareStorage(storage);
	storage.readMatrix("lambda", *_lambda);
	storage.readMatrix("pi", *_pi);
	storage.readMatrix("rho", *_rho);
	storage.readMatrix("r", *_r);
	storage.readMatrix("bel", *_bel);
	storage.readMatrix("win", *_win);
}

void monju::BelLayer::findWinner()
{
	// ノードごとの最大のユニットを特定
	// Eigenにおいて、行ごとのargmaxの計算はloopなしで実現できない
	for (Eigen::Index nRow = 0; nRow < _bel->rows(); nRow++)
	{
		auto row = _bel->row(nRow);
		MatrixRm<float_t>::Index maxarg;
		row.maxCoeff(&maxarg);
		(*_win)(nRow, 0) = static_cast<int32_t>(maxarg);
	}
}

bool monju::BelLayer::containsNan()
{
	bool a = util_eigen::contains_nan<MatrixRm<float_t>>(_lambda);
	bool b = util_eigen::contains_nan<MatrixRm<float_t>>(_pi);
	bool c = util_eigen::contains_nan<MatrixRm<float_t>>(_rho);
	bool d = util_eigen::contains_nan<MatrixRm<float_t>>(_r);
	bool e = util_eigen::contains_nan<MatrixRm<float_t>>(_bel);
	bool f = util_eigen::contains_nan<MatrixRm<int32_t>>(_win);
	return a || b || c || d || e || f;
}

void monju::BelLayer::_setRandomProb(std::shared_ptr<MatrixRm<float_t>> m)
{
	util_eigen::set_stratum_prob_randmom(m.get());
}


// ヘルパ

std::string monju::BelLayer::_dataFileName() const
{
	return util_file::combine(_env->info().workFolder, _id, "dbm");
}

void monju::BelLayer::_prepareStorage(GridMatrixStorage& storage)
{
	Extent extent = _shape.flatten();
	Extent extentWin(extent.rows, 1);

	storage.prepare<float_t>("lambda", extent, kRowMajor);
	storage.prepare<float_t>("pi", extent, kRowMajor);
	storage.prepare<float_t>("rho", extent, kRowMajor);
	storage.prepare<float_t>("r", extent, kRowMajor);
	storage.prepare<float_t>("bel", extent, kRowMajor);
	storage.prepare<int32_t>("win", extentWin, kRowMajor);
}

void monju::BelLayer::_initHostMemory()
{
	// 変数のメモリを確保
	_lambda	= std::make_shared<MatrixRm<float_t>>();
	_pi		= std::make_shared<MatrixRm<float_t>>();
	_rho	= std::make_shared<MatrixRm<float_t>>();
	_r		= std::make_shared<MatrixRm<float_t>>();
	_bel	= std::make_shared<MatrixRm<float_t>>();
	_win	= std::make_shared<MatrixRm<int32_t>>();

	// Eigenの初期化
	Extent e = _shape.flatten();
	_lambda	->resize(e.rows, e.cols);
	_pi		->resize(e.rows, e.cols);
	_rho	->resize(e.rows, e.cols);
	_r		->resize(e.rows, e.cols);
	_bel	->resize(e.rows, e.cols);
	_win	->resize(e.rows, 1);

	// データファイルの使用準備
	{
		GridMatrixStorage storage(_dataFileName());
		_prepareStorage(storage);
	}
}

