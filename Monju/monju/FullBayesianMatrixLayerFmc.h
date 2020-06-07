#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__

#include "MonjuTypes.h"
#include "FullBayesianMatrixLayer.h"
#include "ClVariableSet.h"

namespace monju {

	// マトリックス層のデバイスのメモリを供給
	class FullBayesianMatrixLayerFmc
	{
	/*フィールド*/
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr< FullBayesianMatrixLayer> _edge;
		std::shared_ptr<ClMemory> 
			_clLambda, 
			_clKappa, 
			_clCpt;
		ClVariableSet _clVariableSet;

	/*コンストラクタ*/
	public:
		FullBayesianMatrixLayerFmc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge);
		~FullBayesianMatrixLayerFmc();

	/*プロパティ*/
	public:
		ClVariableSet& clVariableSet();

	/*ヘルパー*/
	private:
		void _lockOuterResources(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge);
		void _allocClMemory();

	/*コピー禁止・ムーブ禁止*/
	public:
		FullBayesianMatrixLayerFmc(const FullBayesianMatrixLayerFmc&) = delete;
		FullBayesianMatrixLayerFmc(FullBayesianMatrixLayerFmc&&) = delete;
		FullBayesianMatrixLayerFmc& operator=(const FullBayesianMatrixLayerFmc&) = delete;
		FullBayesianMatrixLayerFmc& operator=(FullBayesianMatrixLayerFmc&&) = delete;
	};
}

#endif // !_MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__
