#pragma once
#ifndef _MONJU_BAYESIAN_NODE_H__
#define _MONJU_BAYESIAN_NODE_H__
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"

namespace monju {

	// ���̃f�[�^���t�@�C���ɕۑ�
	// �ێ�����f�[�^
	// �E�x�C�Y�v�Z�I�̕ϐ��iOOBP�j
	// �E�i�\��j�\���������iPredNet�I�ȁj
	class BayesianNode : public Synchronizable
	{
	private:

		std::string _id;
		int 
			_nodes,				// ���̃m�[�h��
			_units_per_node;	// �m�[�h������̃��j�b�g��
		MatrixRm<float_t>
			_lambda,	// ��_X(x)
			_pi,		// ��_X(x)
			_rho,		// ��_X(x)
			_r,			// �y�i���e�Bc��_X(x)
			_bel;		// BEL_X(x)
		MatrixRm<int32_t> 
			_win;		// ���҃��j�b�g

	public:
		std::string id() const { return _id; }
		int nodes() const { return _nodes; }
		int unitsPerNode() const { return _units_per_node; }
		MatrixRm<float_t> lambda() const { return _lambda; }
		MatrixRm<float_t> pi() const { return _pi; };
		MatrixRm<float_t> rho() const { return _rho; }
		MatrixRm<float_t> r() const { return _r; }
		MatrixRm<float_t> bel() const { return _bel; }
		MatrixRm<int32_t> win() const { return _win; }

	public:
		BayesianNode(const BayesianNode&) = delete;
		BayesianNode& operator =(const BayesianNode&) = delete;

		BayesianNode(std::string id, int nodes, int units_per_node);
		~BayesianNode();

		void store(std::string dir);

		void load(std::string dir);

	};

}

#endif // !_MONJU_BAYESIAN_NODE_H__