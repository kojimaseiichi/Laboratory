#include "BelLayer.h"


// �R���X�g���N�^

monju::BelLayer::BelLayer(std::string id, UniformBasisShape shape)
{
	_id = id;
	_shape = shape;

	_initHostMemory();
}

monju::BelLayer::~BelLayer()
{
}

void monju::BelLayer::initVariables()
{
	// �ɁA�ρABEL�𗐐��ŏ�����
	// R��1�ŏ�����
	// WIN��0�ŏ�����
	_setRandomProb(_lambda);
	_setRandomProb(_pi);
	_rho->array() = _lambda->array() * _pi->array();
	_r->setOnes();
	*_bel = _rho->array().colwise() / _rho->array().rowwise().sum();
	_win->setZero();
}

void monju::BelLayer::store(std::string dir)
{
	std::string extension = "mat2";
	util_eigen::write_binary(dir, _id, "lambda", extension, *_lambda);
	util_eigen::write_binary(dir, _id, "pi", extension, *_pi);
	util_eigen::write_binary(dir, _id, "rho", extension, *_rho);
	util_eigen::write_binary(dir, _id, "r", extension, *_r);
	util_eigen::write_binary(dir, _id, "bel", extension, *_bel);

	util_eigen::write_binary(dir, _id, "win", extension, *_win);
}

void monju::BelLayer::load(std::string dir)
{
	std::string extension = "mat2";
	if (util_eigen::read_binary(dir, _id, "lambda", extension, *_lambda) == false)
		util_eigen::init_matrix_zero(*_lambda, _shape.nodes, _shape.units);
	if (util_eigen::read_binary(dir, _id, "pi", extension, *_pi) == false)
		util_eigen::init_matrix_zero(*_pi, _shape.nodes, _shape.units);
	if (util_eigen::read_binary(dir, _id, "rho", extension, *_rho) == false)
		util_eigen::init_matrix_zero(*_rho, _shape.nodes, _shape.units);
	if (util_eigen::read_binary(dir, _id, "r", extension, *_r) == false)
		util_eigen::init_matrix_zero(*_r, _shape.nodes, _shape.units);
	if (util_eigen::read_binary(dir, _id, "bel", extension, *_bel) == false)
		util_eigen::init_matrix_zero(*_bel, _shape.nodes, _shape.units);

	if (util_eigen::read_binary(dir, _id, "win", extension, *_win) == false)
		util_eigen::init_matrix_zero(*_win, _shape.nodes, 1);

}

void monju::BelLayer::findWinner()
{
	// �m�[�h���Ƃ̍ő�̃��j�b�g�����
	// Eigen�ɂ����āA�s���Ƃ�argmax�̌v�Z��loop�Ȃ��Ŏ����ł��Ȃ�
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
	util_eigen::set_random_prob(*m);
}

void monju::BelLayer::_initHostMemory()
{
	// �ϐ��̃��������m��
	_lambda = std::make_shared<MatrixRm<float_t>>();
	_pi = std::make_shared<MatrixRm<float_t>>();
	_rho = std::make_shared<MatrixRm<float_t>>();
	_r = std::make_shared<MatrixRm<float_t>>();
	_bel = std::make_shared<MatrixRm<float_t>>();
	_win = std::make_shared<MatrixRm<int32_t>>();

	_lambda->resize(_shape.nodes, _shape.units);
	_pi->resize(_shape.nodes, _shape.units);
	_rho->resize(_shape.nodes, _shape.units);
	_r->resize(_shape.nodes, _shape.units);
	_bel->resize(_shape.nodes, _shape.units);
	_win->resize(_shape.nodes, 1);
}

