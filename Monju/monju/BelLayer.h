#pragma once
#ifndef _MONJU_BEL_LAYER_H__
#define _MONJU_BEL_LAYER_H__

#include "MonjuTypes.h"
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
	class BelLayer : public Synchronizable
	{
	private: // �X�g���[�W

		std::string _id;
		UniformBasisShape _shape;
		std::shared_ptr<MatrixRm<float_t>>
			_lambda,	// ��_X(x)
			_pi,		// ��_X(x)
			_rho,		// ��_X(x)
			_r,			// �y�i���e�Bc��_X(x)
			_bel;		// BEL_X(x)
		std::shared_ptr<MatrixRm<int32_t>>
			_win;		// ���҃��j�b�g

	public:	// �R���X�g���N�^
		BelLayer(std::string id, UniformBasisShape shape);
		~BelLayer();

	public: // ���J�����o
		void initVariables();
		void store(std::string dir);
		void load(std::string dir);
		void findWinner();
		bool containsNan();

	private: // �w���p
		void _initHostMemory();
		void _setRandomProb(std::shared_ptr<MatrixRm<float_t>> m);

	public: // �v���p�e�B
		std::string id() const { return _id; }
		UniformBasisShape shape() const { return _shape; }
		std::weak_ptr<MatrixRm<float_t>> lambda() const { return _lambda; }
		std::weak_ptr<MatrixRm<float_t>> pi() const { return _pi; };
		std::weak_ptr<MatrixRm<float_t>> rho() const { return _rho; }
		std::weak_ptr<MatrixRm<float_t>> r() const { return _r; }
		std::weak_ptr<MatrixRm<float_t>> bel() const { return _bel; }
		std::weak_ptr<MatrixRm<int32_t>> win() const { return _win; }

		// �R�s�[�֎~�E���[�u�֎~
	public:
		BelLayer(const BelLayer&) = delete;
		BelLayer(BelLayer&&) = delete;
		BelLayer& operator =(const BelLayer&) = delete;
		BelLayer& operator =(BelLayer&&) = delete;
	};

}

#endif // !_MONJU_BEL_LAYER_H__
