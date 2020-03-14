#pragma once
#ifndef _MONJU_CONV_LAMBDA_INPUT_PORT_DEVICE_H__
#define _MONJU_CONV_LAMBDA_INPUT_PORT_DEVICE_H__

#include "ConvLambdaInputPort.h"
#include "ClMachine.h"
#include "ClDeviceContext.h"
#include "ClMemory.h"
#include "ClVariableSet.h"

namespace monju
{
	class ConvLambdaInputPortDevice
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		ConvLambdaInputPort* _pPort;
		std::shared_ptr<ClMemory>
			_clInput,
			_clCpt,
			_clLambda;
		std::shared_ptr<MatrixRm<float_t>> _lambda;
		ClVariableSet _clVariableSet;

	public:
		ConvLambdaInputPortDevice(
			std::weak_ptr<ClMachine> clMachine,
			ConvLambdaInputPort& port,
			std::weak_ptr<MatrixRm<float_t>> lambda)
		{
			_clMachine = clMachine.lock();
			_pPort = &port;

			auto image = _pPort->image().lock();
			auto cpt = _pPort->cpt().lock();

			_lambda = lambda.lock();

			_clInput = std::make_shared<ClMemory>(_clMachine, image->size() * sizeof(float_t));
			_clCpt = std::make_shared<ClMemory>(_clMachine, cpt->size() * sizeof(float_t));
			_clLambda = std::make_shared<ClMemory>(_clMachine, _lambda->size() * sizeof(float));

			_clVariableSet.add(_pPort->image(), VariableKind::IMAGE, _clInput);
			_clVariableSet.add(_pPort->cpt(), VariableKind::W, _clCpt);
			_clVariableSet.add<MatrixRm<float_t>>(_lambda, VariableKind::lambda, _clLambda);
		}
		~ConvLambdaInputPortDevice()
		{

		}
		ClVariableSet& clVariableSet()
		{
			return _clVariableSet;
		}

	public:	// コピー禁止・ムーブ禁止
		ConvLambdaInputPortDevice(const ConvLambdaInputPortDevice&) = delete;
		ConvLambdaInputPortDevice(ConvLambdaInputPortDevice&&) = delete;
		ConvLambdaInputPortDevice& operator=(const ConvLambdaInputPortDevice&) = delete;
		ConvLambdaInputPortDevice& operator=(ConvLambdaInputPortDevice&&) = delete;
	};
}

#endif // !_MONJU_CONV_LAMBDA_INPUT_PORT_DEVICE_H__
