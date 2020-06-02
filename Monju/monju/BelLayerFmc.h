#pragma once
#ifndef _MONJU_BEL_LAYER_FMC_H__
#define _MONJU_BEL_LAYER_FMC_H__

#include "MonjuTypes.h"
#include "BelLayer.h"
#include "ClMachine.h"
#include "ClDeviceContext.h"
#include "ClMemory.h"
#include "ClVariableSet.h"

namespace monju {

	class BelLayerFmc
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<BelLayer> _node;
		std::shared_ptr<ClMemory>
			_clLambda,
			_clPi,
			_clRho,
			_clR,
			_clBel,
			_clWin;
		ClVariableSet _clVariableSet;

	// �R���X�g���N�^
	public:
		BelLayerFmc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node)
		{
			_capturePointer(clMachine, node);
			_initClMemory();
			_initVariableSet();
		}
		~BelLayerFmc()
		{

		}

	// �v���p�e�B
	public:

		ClVariableSet& clVariableSet()
		{
			return _clVariableSet;
		}
	
	// �w���p
	private:

		void _capturePointer(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node)
		{
			// �Q�Ƃ���O���̃|�C���^�����b�N
			_clMachine = clMachine.lock();
			_node = node.lock();
		}

		void _initClMemory()
		{
			// OpenCL�̃f�o�C�X��̃�������������
			auto lambda = _node->lambda().lock();
			auto pi = _node->pi().lock();
			auto rho = _node->rho().lock();
			auto r = _node->r().lock();
			auto bel = _node->bel().lock();
			auto win = _node->win().lock();

			_clLambda = std::make_shared<ClMemory>(_clMachine, lambda->size() * sizeof(float));
			_clPi = std::make_shared<ClMemory>(_clMachine, pi->size() * sizeof(float));
			_clRho = std::make_shared<ClMemory>(_clMachine, rho->size() * sizeof(float));
			_clR = std::make_shared<ClMemory>(_clMachine, r->size() * sizeof(float));
			_clBel = std::make_shared<ClMemory>(_clMachine, bel->size() * sizeof(float));
			_clWin = std::make_shared<ClMemory>(_clMachine, win->size() * sizeof(int32_t));
		}

		void _initVariableSet()
		{
			// �ϐ��R���e�i�ł���VariableSet��������
			_clVariableSet.add(_node->lambda(), VariableKind::lambda, _clLambda);
			_clVariableSet.add(_node->pi(), VariableKind::pi, _clPi);
			_clVariableSet.add(_node->rho(), VariableKind::rho, _clRho);
			_clVariableSet.add(_node->r(), VariableKind::R, _clR);
			_clVariableSet.add(_node->bel(), VariableKind::BEL, _clBel);
			_clVariableSet.add(_node->win(), VariableKind::WIN, _clWin);
		}

	public:	// �R�s�[�֎~�E���[�u�֎~
		BelLayerFmc(const BelLayerFmc&) = delete;
		BelLayerFmc(BelLayerFmc&&) = delete;
		BelLayerFmc& operator=(const BelLayerFmc&) = delete;
		BelLayerFmc& operator=(BelLayerFmc&&) = delete;
	};
}

#endif // !_MONJU_BEL_LAYER_FMC_H__

