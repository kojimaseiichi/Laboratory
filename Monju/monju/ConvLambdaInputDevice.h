#pragma once
#ifndef _MONJU_CONV_LAMBDA_INPUT_DEVICE_H__
#define _MONJU_CONV_LAMBDA_INPUT_DEVICE_H__

#include "MonjuTypes.h"

namespace monju
{

	class ConvLambdaInputDevice
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClMemory>
			_clInput,
			_clCpt,
			_clLambda;
		std::shared_ptr<MatrixRm<float_t>> _image, _lambda;
		std::shared_ptr<MatrixCm<float_t>> _cpt;
		std::unique_ptr<ClVariableSet> _clVariableSet;

	public:
		ConvLambdaInputDevice(
			std::weak_ptr<ClMachine> clMachine,
			ConvLambdaInput* pInput,
			std::weak_ptr<MatrixRm<float_t>> lambda);
		~ConvLambdaInputDevice();
		ClVariableSet& clVariableSet();

	public:	// コピー禁止・ムーブ禁止
		ConvLambdaInputDevice(const ConvLambdaInputDevice&) = delete;
		ConvLambdaInputDevice(ConvLambdaInputDevice&&) = delete;
		ConvLambdaInputDevice& operator=(const ConvLambdaInputDevice&) = delete;
		ConvLambdaInputDevice& operator=(ConvLambdaInputDevice&&) = delete;
	};
}

#endif // !_MONJU_CONV_LAMBDA_INPUT_DEVICE_H__
