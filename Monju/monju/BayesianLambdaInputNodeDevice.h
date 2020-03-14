#pragma once
#ifndef _MONJU_BAYESIAN_LAMBDA_INPUT_NODE_DEVICE_H__
#define _MONJU_BAYESIAN_LAMBDA_INPUT_NODE_DEVICE_H__

#include "BayesianLambdaInputNode.h"
#include "ClMachine.h"
#include "ClDeviceContext.h"
#include "ClMemory.h"
#include "ClVariableSet.h"

namespace monju
{
	class BayesianLambdaInputNodeDevice
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		BayesianLambdaInputNode* _pNode;
		std::shared_ptr<ClMemory>
			_clLambda;
		ClVariableSet _clVariableSet;

	public:
		BayesianLambdaInputNodeDevice(std::weak_ptr<ClMachine> clMachine, BayesianLambdaInputNode& node)
		{
			_clMachine = clMachine.lock();
			_pNode = &node;

			auto lambda = _pNode->lambda().lock();
			_clLambda = std::make_shared<ClMemory>(_clMachine, lambda->size() * sizeof(float));
			_clVariableSet.add(_pNode->lambda(), VariableKind::lambda, _clLambda);
		}
		~BayesianLambdaInputNodeDevice()
		{

		}
		ClVariableSet& clVariableSet()
		{
			return _clVariableSet;
		}

	public:	// コピー禁止・ムーブ禁止
		BayesianLambdaInputNodeDevice(const BayesianLambdaInputNodeDevice&) = delete;
		BayesianLambdaInputNodeDevice(BayesianLambdaInputNodeDevice&&) = delete;
		BayesianLambdaInputNodeDevice& operator=(const BayesianLambdaInputNodeDevice&) = delete;
		BayesianLambdaInputNodeDevice& operator=(BayesianLambdaInputNodeDevice&&) = delete;

	};
}

#endif // !_MONJU_BAYESIAN_LAMBDA_INPUT_NODE_DEVICE_H__
