#pragma once
#ifndef _MONJU_CONV_LAMBDA_INPUT_PORT_COMPUTE_H__
#define _MONJU_CONV_LAMBDA_INPUT_PORT_COMPUTE_H__

#include "MonjuTypes.h"
#include <map>
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include "ConvLambdaInputPortDevice.h"
#include "ClFunc.h"
#include "Environment.h"

namespace monju
{
	class ConvLambdaInputPortCompute
	{
	private:
		const std::string
			_kSrcOobpBel = "oobp\\conv_input_slide_up.cl",
			_kKernelOobpBel = "conv_input_slide_up_W${W}_H${H}_FW${FW}_FH${FH}_SW${SW}_SH${SH}_X${X}_XU${XU}";

		Extent _extentInput, _extentFilter;
		UniformBasisShape _shape;

		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel> _clKernel;

	public:
		ConvLambdaInputPortCompute(
			Extent extentInput,
			UniformBasisShape shape,
			Environment& env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_extentInput = extentInput;
			_shape = shape;
			_clMachine = clMachine.lock();

			std::map<std::string, std::string> params_map;
			params_map["W"] = boost::lexical_cast<std::string>(_extentInput.cols);
			params_map["H"] = boost::lexical_cast<std::string>(_extentInput.rows);
			params_map["XU"] = boost::lexical_cast<std::string>(shape.units);


		}

	};
}

#endif // !_MONJU_CONV_LAMBDA_INPUT_PORT_COMPUTE_H__

