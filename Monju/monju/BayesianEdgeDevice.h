#pragma once
#ifndef _MONJU_BAYESIAN_EDGE_DEVICE_H__
#define _MONJU_BAYESIAN_EDGE_DEVICE_H__

#include "BayesianEdge.h"
#include "FullConnectedGridCpt.h"
#include "ClVariableSet.h"

namespace monju {

	class BayesianEdgeDevice
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		BayesianEdge* _pEdge;
		std::shared_ptr<ClMemory> _clLambda, _clKappa, _clCpt;
		ClVariableSet _clVariableSet;
	public:
		BayesianEdgeDevice(std::weak_ptr<ClMachine> clMachine, BayesianEdge& edge)
		{
			_clMachine = clMachine.lock();
			_pEdge = &edge;

			auto lambda = _pEdge->lambda().lock();
			auto kappa = _pEdge->kappa().lock();
			auto cpt = _pEdge->cpt().cpt().lock();

			_clLambda = std::make_shared<ClMemory>(_clMachine, lambda->size() * sizeof(float));
			_clKappa = std::make_shared<ClMemory>(_clMachine, kappa->size() * sizeof(float));
			_clCpt = std::make_shared<ClMemory>(_clMachine, cpt->size() * sizeof(float));

			_clVariableSet.add(_pEdge->lambda(), VariableKind::lambda, _clLambda);
			_clVariableSet.add(_pEdge->kappa(), VariableKind::kappa, _clKappa);
			_clVariableSet.add(_pEdge->cpt().cpt(), VariableKind::W, _clCpt);
		}
		~BayesianEdgeDevice()
		{
			_clLambda.reset();
			_clKappa.reset();
			_clCpt.reset();
		}
		ClVariableSet& clVariableSet()
		{
			return _clVariableSet;
		}

	public:	// コピー禁止・ムーブ禁止
		BayesianEdgeDevice(const BayesianEdgeDevice&) = delete;
		BayesianEdgeDevice(BayesianEdgeDevice&&) = delete;
		BayesianEdgeDevice& operator=(const BayesianEdgeDevice&) = delete;
		BayesianEdgeDevice& operator=(BayesianEdgeDevice&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_EDGE_DEVICE_H__
