#include "BelLayer.h"
#include "BelLayerStorage.h"

// コンストラクタ

monju::BelLayer::BelLayer(std::weak_ptr<Environment> env, std::string id, monju::LayerShape shape)
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
	_persistStorage(true);
}

void monju::BelLayer::load()
{
	_persistStorage(false);
}

void monju::BelLayer::findWinner()
{
	// ノードごとの最大のユニットを特定
	// Eigenにおいて、行ごとのargmaxの計算はloopなしで実現できない
	MatrixRm<float>::Index maxarg;
	for (Eigen::Index row = 0; row < _bel->rows(); row++)
	{
		_bel->row(row).maxCoeff(&maxarg);
		(*_win)(row, 0) = static_cast<int32_t>(maxarg);
	}
}

bool monju::BelLayer::containsNan()
{
	bool a = util_eigen::contains_nan<MatrixRm<float>>(_lambda);
	bool b = util_eigen::contains_nan<MatrixRm<float>>(_pi);
	bool c = util_eigen::contains_nan<MatrixRm<float>>(_rho);
	bool d = util_eigen::contains_nan<MatrixRm<float>>(_r);
	bool e = util_eigen::contains_nan<MatrixRm<float>>(_bel);
	bool f = util_eigen::contains_nan<MatrixRm<int32_t>>(_win);
	return a || b || c || d || e || f;
}

void monju::BelLayer::copyData(const BelLayer& o)
{
	*_lambda = *o._lambda;
	*_pi = *o._pi;
	*_rho = *o._rho;
	*_r = *o._r;
	*_bel = *o._bel;
	*_win = *o._win;
}

void monju::BelLayer::performBel()
{
	*_bel = *_rho = _pi->array() * _lambda->array() * _r->array();
	_bel->array().colwise() /= _rho->rowwise().sum().array();
	findWinner();
}

void monju::BelLayer::_setRandomProb(std::shared_ptr<MatrixRm<float>> m)
{
	util_eigen::set_stratum_prob_randmom(m.get());
}


// ヘルパ

std::string monju::BelLayer::_dataFileName() const
{
	return util_file::combine(_env->info().workFolder, _id, "dbm");
}

void monju::BelLayer::_persistStorage(bool storing)
{
	BelLayerStorage storage(_dataFileName(), _shape);
	storage.prepareAll();
	storage.persistLambda(storing, *_lambda);
	storage.persistPi(storing, *_pi);
	storage.persistRho(storing, *_rho);
	storage.persistR(storing, *_r);
	storage.persistBEL(storing, *_bel);
	storage.persistWin(storing, *_win);
}

void monju::BelLayer::_initHostMemory()
{
	// 変数のメモリを確保
	_lambda	= std::make_shared<MatrixRm<float>>();
	_pi		= std::make_shared<MatrixRm<float>>();
	_rho	= std::make_shared<MatrixRm<float>>();
	_r		= std::make_shared<MatrixRm<float>>();
	_bel	= std::make_shared<MatrixRm<float>>();
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
		BelLayerStorage storage(_dataFileName(), _shape);
		storage.prepareAll();
	}
}

