#pragma once
#ifndef _MONJU_BEL_LAYER_FMC_FP16_H__
#define _MONJU_BEL_LAYER_FMC_FP16_H__

#include <memory>
#include "eigentypes.h"
#include "ClVariableSet.h"

namespace monju
{
	class ClMachine;
	class ClMemory;
	class BelLayer;

	typedef unsigned __int16        cl_half;
	
		// デバイスのメモリを供給
	class BelLayerFmcFp16
	{
#pragma region Private Field
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

		//

		MatrixRm<cl_half> _lambdaHalf;
		MatrixRm<cl_half> _piHalf;
		MatrixRm<cl_half> _rhoHalf;
		MatrixRm<cl_half> _rHalf;
		MatrixRm<cl_half> _belHalf;
	
#pragma endregion
	/*コンストラクタ*/public:
		BelLayerFmcFp16(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node);
		~BelLayerFmcFp16();

	/*プロパティ*/public:
		ClVariableSet& clVariableSet();

	/*ヘルパ*/private:
		void _capturePointer(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node);
		void _initClMemory();
		void _initVariableSet();

	/*コピー禁止・ムーブ禁止*/public:
		BelLayerFmcFp16(const BelLayerFmcFp16&) = delete;
		BelLayerFmcFp16(BelLayerFmcFp16&&) = delete;
		BelLayerFmcFp16& operator=(const BelLayerFmcFp16&) = delete;
		BelLayerFmcFp16& operator=(BelLayerFmcFp16&&) = delete;
	};

}

#endif // !_MONJU_BEL_LAYER_FMC_FP16_H__

