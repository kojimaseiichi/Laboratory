#include "BayesianNode.h"

void monju::BayesianNode::initRandom()
{
	_setRandomProb(*_lambda);
	_setRandomProb(*_pi);
	_rho->array() = _lambda->array() * _pi->array();
	_r->setOnes();
	*_bel = _rho->array().colwise() / _rho->array().rowwise().sum();
	_win->setOnes();
}

void monju::BayesianNode::store(std::string dir)
{
	std::string extension = "mat2";
	util_eigen::write_binary(dir, _id, "lambda", extension, *_lambda);
	util_eigen::write_binary(dir, _id, "pi", extension, *_pi);
	util_eigen::write_binary(dir, _id, "rho", extension, *_rho);
	util_eigen::write_binary(dir, _id, "r", extension, *_r);
	util_eigen::write_binary(dir, _id, "bel", extension, *_bel);

	util_eigen::write_binary(dir, _id, "win", extension, *_win);
}

void monju::BayesianNode::load(std::string dir)
{
	std::string extension = "mat2";
	if (util_eigen::read_binary(dir, _id, "lambda", extension, *_lambda) == false)
		util_eigen::init_matrix_zero(*_lambda, _nodes, _units_per_node);
	if (util_eigen::read_binary(dir, _id, "pi", extension, *_pi) == false)
		util_eigen::init_matrix_zero(*_pi, _nodes, _units_per_node);
	if (util_eigen::read_binary(dir, _id, "rho", extension, *_rho) == false)
		util_eigen::init_matrix_zero(*_rho, _nodes, _units_per_node);
	if (util_eigen::read_binary(dir, _id, "r", extension, *_r) == false)
		util_eigen::init_matrix_zero(*_r, _nodes, _units_per_node);
	if (util_eigen::read_binary(dir, _id, "bel", extension, *_bel) == false)
		util_eigen::init_matrix_zero(*_bel, _nodes, _units_per_node);

	if (util_eigen::read_binary(dir, _id, "win", extension, *_win) == false)
		util_eigen::init_matrix_zero(*_win, _nodes, 1);

}

void monju::BayesianNode::_setRandomProb(MatrixRm<float_t>& m)
{
	m.setRandom();
	m = m.array().abs();
	m.array().colwise() /= m.array().rowwise().sum();
}
