#pragma once
#ifndef _MONJU_BEL_LAYER_FMC_H__
#define _MONJU_BEL_LAYER_FMC_H__

#include <memory>
#include "ClVariableSet.h"

namespace monju {

	class ClMachine;
	class BelLayer;
	class ClMemory;

	// デバイスのメモリを供給
	class BelLayerFmc
	{
	/*フィールド*/private:
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
	
	/*コンストラクタ*/public:
		BelLayerFmc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node);
		~BelLayerFmc();
	
	/*プロパティ*/public:
		ClVariableSet& clVariableSet();
	
	/*ヘルパ*/private:
		void _capturePointer(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node);
		void _initClMemory();
		void _initVariableSet();

	/*コピー禁止・ムーブ禁止*/public:
		BelLayerFmc(const BelLayerFmc&) = delete;
		BelLayerFmc(BelLayerFmc&&) = delete;
		BelLayerFmc& operator=(const BelLayerFmc&) = delete;
		BelLayerFmc& operator=(BelLayerFmc&&) = delete;
	};
}

#endif // !_MONJU_BEL_LAYER_FMC_H__

