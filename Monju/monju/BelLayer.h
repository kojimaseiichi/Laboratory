#pragma once
#ifndef _MONJU_BEL_LAYER_H__
#define _MONJU_BEL_LAYER_H__

#include <memory>
#include <string>
#include <stdint.h>
#include "eigentypes.h"
#include "Synchronizable.h"
#include "Extent.h"


namespace monju
{
	// �O���錾
	class Environment;

	// BEL���C���[�̃z�X�g��������
	class BelLayer : public Synchronizable
	{
	private: // �X�g���[�W

		std::shared_ptr<Environment> _env;
		std::string _id;
		LayerShape _shape;
		std::shared_ptr<MatrixRm<float>>
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

	public: // ���J�v���p�e�B
		std::string							id() const;
		LayerShape							shape() const;
		std::weak_ptr<MatrixRm<float>>		lambda() const;
		std::weak_ptr<MatrixRm<float>>		pi() const;;
		std::weak_ptr<MatrixRm<float>>		rho() const;
		std::weak_ptr<MatrixRm<float>>		r() const;
		std::weak_ptr<MatrixRm<float>>		bel() const;
		std::weak_ptr<MatrixRm<int32_t>>	win() const;

	public: // ���J�����o
		void InitializeVariables();
		void ComputeBel();
		void ComputeToFindWinners();
		void store();
		void load();
		void CopyFrom(const BelLayer& o);
		bool TestContainsNan();

	private: // �v���C�x�[�g�v���p�e�B
		std::string _data_file_name() const;

	private: // �w���p
		void _PersistStorage(bool storing);
		void _InitHostMemory();
		void _SetRandomProb(std::shared_ptr<MatrixRm<float>> m);

		// �R�s�[�֎~�E���[�u�֎~
	public:
		BelLayer(const BelLayer&) = delete;
		BelLayer(BelLayer&&) = delete;
		BelLayer& operator =(const BelLayer&) = delete;
		BelLayer& operator =(BelLayer&&) = delete;
	};

}

#endif // !_MONJU_BEL_LAYER_H__
