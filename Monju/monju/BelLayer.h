#pragma once
#ifndef _MONJU_BEL_LAYER_H__
#define _MONJU_BEL_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"
#include "Extent.h"
#include "Environment.h"

namespace monju {

	// ���̃f�[�^���t�@�C���ɕۑ�
	// �ێ�����f�[�^
	// �E�x�C�Y�v�Z�I�̕ϐ��iOOBP�j
	// �E�i�\��j�\���������iPredNet�I�ȁj
	class BelLayer : public Synchronizable
	{
	private: // �X�g���[�W

		std::shared_ptr<Environment> _env;
		std::string _id;
		LayerShape _shape;
		std::shared_ptr<MatrixRm<float_t>>
			_lambda,	// ��_X(x) shape:(nodes, units)
			_pi,		// ��_X(x) shape:(nodes, units)
			_rho,		// ��_X(x) shape:(nodes, units)
			_r,			// �y�i���e�Bc��_X(x)  shape:(nodes, units)
			_bel;		// BEL_X(x)  shape:(nodes, units)
		std::shared_ptr<MatrixRm<int32_t>>
			_win;		// ���҃��j�b�g  shape:(nodes, 1)

	public:	// �R���X�g���N�^
		BelLayer(std::weak_ptr<Environment> env, std::string id, LayerShape shape);
		~BelLayer();

	public: // ���J�����o
		void initVariables();
		void store();
		void load();
		void findWinner();
		bool containsNan();

	private: // �w���p
		std::string _dataFileName() const;
		void _prepareStorage(GridMatrixStorage& storage);
		void _initHostMemory();
		void _setRandomProb(std::shared_ptr<MatrixRm<float_t>> m);

	public: // �v���p�e�B
		std::string id() const { return _id; }
		LayerShape shape() const { return _shape; }
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
