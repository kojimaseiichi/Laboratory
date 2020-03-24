#pragma once
#ifndef _MONJU_BAYESIAN_FULL_CONNECT_EDGE_DEVICE_H__
#define _MONJU_BAYESIAN_FULL_CONNECT_EDGE_DEVICE_H__

#include "BayesianFullConnectEdge.h"
#include "FullConnectedGridCpt.h"
#include "ClVariableSet.h"

namespace monju {

	class BayesianFullConnectEdgeDevice
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		BayesianFullConnectEdge* _pEdge;
		std::shared_ptr<ClMemory> _clLambda, _clKappa, _clCpt;
		ClVariableSet _clVariableSet;
	public:
		BayesianFullConnectEdgeDevice(std::weak_ptr<ClMachine> clMachine, BayesianFullConnectEdge& edge)
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
		~BayesianFullConnectEdgeDevice()
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
		BayesianFullConnectEdgeDevice(const BayesianFullConnectEdgeDevice&) = delete;
		BayesianFullConnectEdgeDevice(BayesianFullConnectEdgeDevice&&) = delete;
		BayesianFullConnectEdgeDevice& operator=(const BayesianFullConnectEdgeDevice&) = delete;
		BayesianFullConnectEdgeDevice& operator=(BayesianFullConnectEdgeDevice&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_FULL_CONNECT_EDGE_DEVICE_H__
