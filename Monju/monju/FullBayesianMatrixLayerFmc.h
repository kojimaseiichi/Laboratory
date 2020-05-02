#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__

#include "MonjuTypes.h"
#include "FullBayesianMatrixLayer.h"
#include "ClVariableSet.h"

namespace monju {

	class FullBayesianMatrixLayerFmc
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr< FullBayesianMatrixLayer> _edge;
		std::shared_ptr<ClMemory> 
			_clLambda, 
			_clKappa, 
			_clCpt;
		ClVariableSet _clVariableSet;

	public: // コンストラクタ
		FullBayesianMatrixLayerFmc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge)
		{
			_lockOuterResources(clMachine, edge);
			_allocClMemory();

		}
		~FullBayesianMatrixLayerFmc()
		{
		}

	public: // プロパティ
		ClVariableSet& clVariableSet()
		{
			return _clVariableSet;
		}

	private: // ヘルパー
		void _lockOuterResources(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge)
		{
			_clMachine = clMachine.lock();
			_edge = edge.lock();
		}

		void _allocClMemory()
		{
			auto lambda = _edge->lambda().lock();
			auto kappa = _edge->kappa().lock();
			auto cpt = _edge->cpt().lock();

			_clLambda = std::make_shared<ClMemory>(_clMachine, lambda->size() * sizeof(float_t));
			_clKappa = std::make_shared<ClMemory>(_clMachine, kappa->size() * sizeof(float_t));
			_clCpt = std::make_shared<ClMemory>(_clMachine, cpt->size() * sizeof(float_t));

			_clVariableSet.add(_edge->lambda(), VariableKind::lambda, _clLambda);
			_clVariableSet.add(_edge->kappa(), VariableKind::kappa, _clKappa);
			_clVariableSet.add(_edge->cpt(), VariableKind::W, _clCpt);
		}

	public:	// コピー禁止・ムーブ禁止
		FullBayesianMatrixLayerFmc(const FullBayesianMatrixLayerFmc&) = delete;
		FullBayesianMatrixLayerFmc(FullBayesianMatrixLayerFmc&&) = delete;
		FullBayesianMatrixLayerFmc& operator=(const FullBayesianMatrixLayerFmc&) = delete;
		FullBayesianMatrixLayerFmc& operator=(FullBayesianMatrixLayerFmc&&) = delete;
	};
}

#endif // !_MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__
