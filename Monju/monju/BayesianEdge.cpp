#include "BayesianEdge.h"

monju::BayesianEdge::~BayesianEdge()
{

}

void monju::BayesianEdge::initRandom()
{
	_setRandomProb(_lambda, _kUnitsPerNodeX, _kNodesY);
	_setRandomProb(_kappa, _kUnitsPerNodeY, _kNodesY);
	_cpt.initRandom();
}

void monju::BayesianEdge::store(std::string dir)
{
	std::string extension = "mat2";
	util_eigen::write_binary(dir, _id, "lambda", extension, _lambda);
	util_eigen::write_binary(dir, _id, "kappa", extension, _kappa);
	_cpt.store(dir);
}

void monju::BayesianEdge::load(std::string dir)
{
	std::string extension = "mat2";
	if (util_eigen::read_binary(dir, _id, "lambda", extension, _lambda) == false)
		util_eigen::init_matrix_zero(_lambda, _kNodesY * _kUnitsPerNodeX, _kNodesX);
	if (util_eigen::read_binary(dir, _id, "kappa", extension, _kappa) == false)
		util_eigen::init_matrix_zero(_kappa, _kNodesY * _kUnitsPerNodeY, _kNodesX);
	_cpt.load(dir);
}

void monju::BayesianEdge::_setRandomProb(MatrixCm<float_t>& m, int unitsPerNode, int oppositNodes)
{
	m.setRandom();
	m = m.array().abs();
	for (auto col : m.colwise())
	{
		auto cm = col.reshaped(unitsPerNode, oppositNodes);
		cm.array().rowwise() /= cm.array().colwise().sum();
	}
}
