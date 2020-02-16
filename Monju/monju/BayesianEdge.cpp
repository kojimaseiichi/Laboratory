#include "BayesianEdge.h"

monju::BayesianEdge::BayesianEdge(std::string id, UniformBasisShape shapeX, UniformBasisShape shapeY)
	: _cpt(id, shapeX, shapeY)
{
	_id = id;
	_shapeX = shapeX;
	_shapeY = shapeY;

	_lambda = std::make_shared<MatrixCm<float_t>>();
	_kappa = std::make_shared<MatrixCm<float>>();

	_lambda->resize(_shapeY.nodes * _shapeX.units, shapeX.nodes);
	_kappa->resize(_shapeY.nodes * _shapeY.units, shapeX.nodes);

	_lambda->setZero();
	_kappa->setZero();
}

monju::BayesianEdge::~BayesianEdge()
{

}

void monju::BayesianEdge::initRandom()
{
	_setRandomProb(_lambda, _shapeX.units, _shapeY.nodes);
	_setRandomProb(_kappa, _shapeY.units, _shapeY.nodes);
	_cpt.initRandom();
}

void monju::BayesianEdge::store(std::string dir)
{
	std::string extension = "mat2";
	util_eigen::write_binary(dir, _id, "lambda", extension, *_lambda);
	util_eigen::write_binary(dir, _id, "kappa", extension, *_kappa);
	_cpt.store(dir);
}

void monju::BayesianEdge::load(std::string dir)
{
	std::string extension = "mat2";
	if (util_eigen::read_binary(dir, _id, "lambda", extension, *_lambda) == false)
		util_eigen::init_matrix_zero(*_lambda, _shapeY.nodes * _shapeX.units, _shapeX.nodes);
	if (util_eigen::read_binary(dir, _id, "kappa", extension, *_kappa) == false)
		util_eigen::init_matrix_zero(*_kappa, _shapeY.nodes * _shapeY.units, _shapeX.nodes);
	_cpt.load(dir);
}

void monju::BayesianEdge::_setRandomProb(std::shared_ptr<MatrixCm<float_t>> m, int unitsPerNode, int oppositNodes)
{
	m->setRandom();
	*m = m->array().abs();
	for (auto col : m->colwise())
	{
		auto cm = col.reshaped(unitsPerNode, oppositNodes);
		cm.array().rowwise() /= cm.array().colwise().sum();
	}
}

