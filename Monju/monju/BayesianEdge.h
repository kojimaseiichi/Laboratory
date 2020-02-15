#pragma once
#ifndef _MONJU_BAYESIAN_EDGE_H__
#define _MONJU_BAYESIAN_EDGE_H__

#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "GridCpt.h"

namespace monju {

	// ���Ԃ̃f�[�^���t�B���j�ۑ�
	// �ێ�����f�[�^
	// �E�x�C�Y�v�Z�I�ȕϐ��iCPT/OOBP�j
	class BayesianEdge : public Synchronizable
	{
	private:
		std::string _id;
		int
			_kNodesX,				// ���̃m�[�h��(X)
			_kUnitsPerNodeX,		// �m�[�h������̃��j�b�g��(X)
			_kNodesY,				// ���̃m�[�h��(Y)
			_kUnitsPerNodeY;		// �m�[�h������̃��j�b�g��(Y)

		std::shared_ptr<MatrixCm<float_t>>
			_lambda,				// �Ɂi�m�[�h��Y * ���j�b�g��Y, �m�[�h��X�j
			_kappa;					// �ȁi�m�[�h��Y * ���j�b�g��X, �m�[�h��X�j

		GridCpt _cpt;				// �d�ݍs��

	public:
		std::string id() const { return _id; }
		int nodesX() const { return _kNodesX; }
		int unitsPerNodeX() const { return _kUnitsPerNodeX; }
		int nodesY() const { return _kNodesY; }
		int unitsPerNodeY() const { return _kUnitsPerNodeY; }
		std::weak_ptr<MatrixCm<float_t>> lambda() { return _lambda; }
		std::weak_ptr<MatrixCm<float_t>> kappa() { return _kappa; }
		GridCpt& cpt() { return _cpt; }

	public:
		BayesianEdge(const BayesianEdge&) = delete;
		BayesianEdge& operator =(const BayesianEdge&) = delete;

		BayesianEdge(
			std::string id,
			int nodes_x,
			int units_per_node_x,
			int nodes_y,
			int units_per_node_y
		)
			: _cpt(id, nodes_x, units_per_node_x, nodes_y, units_per_node_y)
		{
			_id = id;
			_kNodesX = nodes_x;
			_kUnitsPerNodeX = units_per_node_x;
			_kNodesY = nodes_y;
			_kUnitsPerNodeY = units_per_node_y;

			_lambda = std::make_shared<MatrixCm<float_t>>();
			_kappa = std::make_shared<MatrixCm<float>>();

			_lambda->resize(_kNodesY * _kUnitsPerNodeX, _kNodesX);
			_kappa->resize(_kNodesY * _kUnitsPerNodeY, _kNodesX);

			_lambda->setZero();
			_kappa->setZero();
		}
		~BayesianEdge();
		void initRandom();
		void store(std::string dir);
		void load(std::string dir);

	private:
		void _setRandomProb(MatrixCm<float_t>& m, int unitsPerNode, int oppositNodes);
	};
}

#endif // !_MONJU_BAYESIAN_EDGE_H__
