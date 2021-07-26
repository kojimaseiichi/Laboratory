#pragma once
#ifndef _MONJU_BEL_LAYER_FMC_H__
#define _MONJU_BEL_LAYER_FMC_H__

#include <memory>
#include "ClVariableSet.h"

namespace monju {

	class ClMachine;
	class BelLayer;
	class ClMemory;

	// �f�o�C�X�̃�����������
	class BelLayerFmc
	{
	/*�t�B�[���h*/private:
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
	
	/*�R���X�g���N�^*/public:
		BelLayerFmc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node);
		~BelLayerFmc();
	
	/*�v���p�e�B*/public:
		ClVariableSet& clVariableSet();
	
	/*�w���p*/private:
		void _capturePointer(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node);
		void _initClMemory();
		void _initVariableSet();

	/*�R�s�[�֎~�E���[�u�֎~*/public:
		BelLayerFmc(const BelLayerFmc&) = delete;
		BelLayerFmc(BelLayerFmc&&) = delete;
		BelLayerFmc& operator=(const BelLayerFmc&) = delete;
		BelLayerFmc& operator=(BelLayerFmc&&) = delete;
	};
}

#endif // !_MONJU_BEL_LAYER_FMC_H__

