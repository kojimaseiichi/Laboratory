#include "BelLayer.h"
#include "BelLayerStorage.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"
#include "Extent.h"
#include "Environment.h"

// コンストラクタ

monju::BelLayer::BelLayer(std::weak_ptr<Environment> env, std::string id, monju::LayerShape shape)
{
	_env = env.lock();
	_id = id;

	_shape = shape;

	_InitHostMemory();
}

monju::BelLayer::~BelLayer()
{
}

// 公開プロパティ

std::string monju::BelLayer::id() const { return _id; }

monju::LayerShape monju::BelLayer::shape() const { return _shape; }

std::weak_ptr<monju::MatrixRm<float>> monju::BelLayer::lambda() const { return _lambda; }

std::weak_ptr<monju::MatrixRm<float>> monju::BelLayer::pi() const { return _pi; }

std::weak_ptr<monju::MatrixRm<float>> monju::BelLayer::rho() const { return _rho; }

std::weak_ptr<monju::MatrixRm<float>> monju::BelLayer::r() const { return _r; }

std::weak_ptr<monju::MatrixRm<float>> monju::BelLayer::bel() const { return _bel; }

std::weak_ptr<monju::MatrixRm<int32_t>> monju::BelLayer::win() const { return _win; }

void monju::BelLayer::InitializeVariables()
{
	// λ、ρ、BELを乱数で初期化
	// Rを1で初期化
	// WINを0で初期化
	_SetRandomProb(_lambda);
	_SetRandomProb(_pi);
	_rho->array() = _lambda->array() * _pi->array();
	_r->setOnes();
	*_bel = _rho->array().colwise() / _rho->array().rowwise().sum();
	_win->setZero();
}

void monju::BelLayer::store()
{
	_PersistStorage(true);
}

void monju::BelLayer::load()
{
	_PersistStorage(false);
}

void monju::BelLayer::ComputeToFindWinners()
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

bool monju::BelLayer::TestContainsNan()
{
	bool a = util_eigen::contains_nan<MatrixRm<float>>(_lambda);
	bool b = util_eigen::contains_nan<MatrixRm<float>>(_pi);
	bool c = util_eigen::contains_nan<MatrixRm<float>>(_rho);
	bool d = util_eigen::contains_nan<MatrixRm<float>>(_r);
	bool e = util_eigen::contains_nan<MatrixRm<float>>(_bel);
	bool f = util_eigen::contains_nan<MatrixRm<int32_t>>(_win);
	return a || b || c || d || e || f;
}

void monju::BelLayer::CopyFrom(const BelLayer& o)
{
	*_lambda = *o._lambda;
	*_pi = *o._pi;
	*_rho = *o._rho;
	*_r = *o._r;
	*_bel = *o._bel;
	*_win = *o._win;
}

void monju::BelLayer::ComputeBel()
{
	/* π、λ、制約ノード（R)からBELを計算
	*/
	*_bel = *_rho = _pi->array() * _lambda->array() * _r->array();
	_bel->array().colwise() /= _rho->rowwise().sum().array();
	ComputeToFindWinners();
}

void monju::BelLayer::_SetRandomProb(std::shared_ptr<MatrixRm<float>> m)
{
	util_eigen::set_stratum_prob_randmom(m.get());
}


// ヘルパ

std::string monju::BelLayer::_data_file_name() const
{
	return util_file::combine(_env->info().work_folder, _id, "dbm");
}

void monju::BelLayer::_PersistStorage(bool storing)
{
	BelLayerStorage storage(_data_file_name(), _shape);
	storage.PrepareAllStorageKeys();
	storage.persistLambda(storing, *_lambda);
	storage.persistPi(storing, *_pi);
	storage.persistRho(storing, *_rho);
	storage.persistR(storing, *_r);
	storage.persistBEL(storing, *_bel);
	storage.persistWin(storing, *_win);
}

void monju::BelLayer::_InitHostMemory()
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
		BelLayerStorage storage(_data_file_name(), _shape);
		storage.PrepareAllStorageKeys();
	}
}

