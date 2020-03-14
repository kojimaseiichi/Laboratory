#include "BayesianNode.h"

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

void monju::BayesianNode::_setRandomProb(std::shared_ptr<MatrixRm<float_t>> m)
{
	util_eigen::set_random_prob(*m);
}

