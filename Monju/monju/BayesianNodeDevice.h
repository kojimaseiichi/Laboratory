#pragma once
#ifndef _BAYESIAN_NODE_DEVICE_H__
#define _BAYESIAN_NODE_DEVICE_H__

#include "BayesianNode.h"
#include "ClMachine.h"
#include "ClDeviceContext.h"
#include "ClMemory.h"
#include "ClVariableSet.h"

namespace monju {

	class BayesianNodeDevice
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		BayesianNode* _pNode;
		std::shared_ptr<ClMemory>
			_clLambda,
			_clPi,
			_clRho,
			_clR,
			_clBel,
			_clWin;
		ClVariableSet _clVariableSet;

	public:
		BayesianNodeDevice(std::weak_ptr<ClMachine> clMachine, BayesianNode& node)
		{
			_clMachine = clMachine.lock();
			_pNode = &node;

			auto lambda = _pNode->lambda().lock();
			auto pi = _pNode->pi().lock();
			auto rho = _pNode->rho().lock();
			auto r = _pNode->r().lock();
			auto bel = _pNode->bel().lock();
			auto win = _pNode->win().lock();

			_clLambda = std::make_shared<ClMemory>(_clMachine, lambda->size() * sizeof(float));
			_clPi = std::make_shared<ClMemory>(_clMachine, pi->size() * sizeof(float));
			_clRho = std::make_shared<ClMemory>(_clMachine, rho->size() * sizeof(float));
			_clR = std::make_shared<ClMemory>(_clMachine, r->size() * sizeof(float));
			_clBel = std::make_shared<ClMemory>(_clMachine, bel->size() * sizeof(float));
			_clWin = std::make_shared<ClMemory>(_clMachine, win->size() * sizeof(int32_t));

			_clVariableSet.add(_pNode->lambda(), VariableKind::lambda, _clLambda);
			_clVariableSet.add(_pNode->pi(), VariableKind::pi, _clPi);
			_clVariableSet.add(_pNode->rho(), VariableKind::rho, _clRho);
			_clVariableSet.add(_pNode->r(), VariableKind::R, _clR);
			_clVariableSet.add(_pNode->bel(), VariableKind::BEL, _clBel);
			_clVariableSet.add(_pNode->win(), VariableKind::WIN, _clWin);
		}
		~BayesianNodeDevice()
		{

		}
		ClVariableSet& clVariableSet()
		{
			return _clVariableSet;
		}

	public:	// コピー禁止・ムーブ禁止
		BayesianNodeDevice(const BayesianNodeDevice&) = delete;
		BayesianNodeDevice(BayesianNodeDevice&&) = delete;
		BayesianNodeDevice& operator=(const BayesianNodeDevice&) = delete;
		BayesianNodeDevice& operator=(BayesianNodeDevice&&) = delete;
	};
}

#endif // !_BAYESIAN_NODE_DEVICE_H__

