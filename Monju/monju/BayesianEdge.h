#pragma once
#ifndef _MONJU_BAYESIAN_EDGE_H__
#define _MONJU_BAYESIAN_EDGE_H__
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"


namespace monju {

	// ���Ԃ̃f�[�^���t�B���j�ۑ�
	// �ێ�����f�[�^
	// �E�x�C�Y�v�Z�I�ȕϐ��iCPT/OOBP�j
	class BayesianEdge : public Synchronizable
	{
	private:
		std::string _id;
		int
			_nodes_x,				// ���̃m�[�h��(X)
			_units_per_node_x,		// �m�[�h������̃��j�b�g��(X)
			_nodes_y,				// ���̃m�[�h��(Y)
			_units_per_node_y;		// �m�[�h������̃��j�b�g��(Y)

		MatrixCm<float_t>
			_lambda,				// �Ɂi�m�[�h��Y * ���j�b�g��Y, �m�[�h��X�j
			_kappa;					// �ȁi�m�[�h��Y * ���j�b�g��X, �m�[�h��X�j

	public:
		std::string id() const { return _id; }
		int nodesX() const { return _nodes_x; }
		int unitsPerNodeX() const { return _units_per_node_x; }
		int nodesY() const { return _nodes_y; }
		int unitsPerNodeY() const { return _units_per_node_y; }
		MatrixCm<float_t> lambda() const { return _lambda; }
		MatrixCm<float_t> kappa() const { return _kappa; }


	public:
		BayesianEdge(const BayesianEdge&) = delete;
		BayesianEdge& operator =(const BayesianEdge&) = delete;

		BayesianEdge(
			std::string id,
			int nodes_x,
			int units_per_node_x,
			int nodes_y,
			int units_per_node_y
		);
		~BayesianEdge();
		void store(std::string dir);
		void load(std::string dir);

	};
}

#endif // !_MONJU_BAYESIAN_EDGE_H__
