#include "CortexInterBasisStorage.h"

monju::CortexInterBasisStorage::CortexInterBasisStorage(std::string id, int nodes_x, int units_per_node_x, int nodes_y, int units_per_node_y)
{
	_id = id;
	_nodes_x = nodes_x;
	_units_per_node_x = units_per_node_x;
	_nodes_y = nodes_y;
	_units_per_node_y = units_per_node_y;

	util_eigen::init_matrix_zero(_cpt, _nodes_y * _units_per_node_x * _units_per_node_y, _nodes_x);
	util_eigen::init_matrix_zero(_lambda, _nodes_y * _units_per_node_x, _nodes_x);
	util_eigen::init_matrix_zero(_kappa, _nodes_y * _units_per_node_y, _nodes_x);
}

monju::CortexInterBasisStorage::~CortexInterBasisStorage()
{

}

void monju::CortexInterBasisStorage::store(std::string dir)
{
	std::string extension = "mat2";
	util_eigen::write_binary(dir, _id, "cpt", extension, _cpt);
	util_eigen::write_binary(dir, _id, "lambda", extension, _lambda);
	util_eigen::write_binary(dir, _id, "kappa", extension, _kappa);
}

void monju::CortexInterBasisStorage::load(std::string dir)
{
	std::string extension = "mat2";
	if (util_eigen::read_binary(dir, _id, "cpt", extension, _cpt) == false)
		util_eigen::init_matrix_zero(_cpt, _nodes_y * _units_per_node_x * _units_per_node_y, _nodes_x);
	if (util_eigen::read_binary(dir, _id, "lambda", extension, _lambda) == false)
		util_eigen::init_matrix_zero(_lambda, _nodes_y * _units_per_node_x, _nodes_x);
	if (util_eigen::read_binary(dir, _id, "kappa", extension, _kappa) == false)
		util_eigen::init_matrix_zero(_kappa, _nodes_y * _units_per_node_y, _nodes_x);
}
