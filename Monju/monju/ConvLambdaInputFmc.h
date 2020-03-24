#pragma once
#ifndef _MONJU_CONV_LAMBDA_INPUT_FMC_H__
#define _MONJU_CONV_LAMBDA_INPUT_FMC_H__

#include "MonjuTypes.h"
#include <map>
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include "ClMachine.h"
#include "ClDeviceContext.h"
#include "ClMemory.h"
#include "ClVariableSet.h"
#include "ClFunc.h"
#include "Environment.h"
#include "ConvLambdaInput.h"
#include "ConvLambdaInputDevice.h"

namespace monju
{
	class ConvLambdaInputFmc
	{
	private:
		const std::string
			_kSrcFile = "oobp\\conv_input_slide_up.cl",
			_kKernel = "conv_input_slide_up_W${W}_H${H}_FW${FW}_FH${FH}_SW${SW}_SH${SH}_X${X}_XU${XU}";

		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel> _clKernel;
		std::shared_ptr<ClMemory>
			_clInput,
			_clCpt,
			_clLambda;
		std::shared_ptr<MatrixRm<float_t>> _image, _lambda;
		std::shared_ptr<MatrixCm<float_t>> _oneCpt;
		ClVariableSet _clVariableSet;

		Extent _extentInput, _extentFilter, _extentSlide;
		UniformBasisShape _shape;


	public:
		ConvLambdaInputFmc(
			ConvLambdaInput* p,	// 形状を取得する、データは取得しない
			Environment& env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_extentInput = p->extImage();
			_extentFilter = p->extFilter();
			_extentSlide = p->extStride();
			_shape = p->shape();
			_clMachine = clMachine.lock();

			std::map<std::string, std::string> params_map;
			params_map["W"] = boost::lexical_cast<std::string>(_extentInput.cols);
			params_map["H"] = boost::lexical_cast<std::string>(_extentInput.rows);
			params_map["FW"] = boost::lexical_cast<std::string>(_extentFilter.cols);
			params_map["FH"] = boost::lexical_cast<std::string>(_extentFilter.rows);
			params_map["SW"] = boost::lexical_cast<std::string>(_extentSlide.cols);
			params_map["SH"] = boost::lexical_cast<std::string>(_extentSlide.rows);
			params_map["X"] = boost::lexical_cast<std::string>(_shape.nodes);
			params_map["XU"] = boost::lexical_cast<std::string>(_shape.units);

			std::filesystem::path kernelPathBase = env.info().kernelFolder;
			_clKernel = std::make_shared<ClKernel>(
				_clMachine,
				(kernelPathBase / _kSrcFile).string(),
				_kKernel,
				params_map);

		}
		~ConvLambdaInputFmc()
		{

		}
		void convInputSlideUp(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			ClVariableSet& variables,
			ClEventJoiner* pJoin)
		{
			_convInputSlideUp(clDeviceContext, variables, pJoin);
			auto p = clDeviceContext.lock();
			p->flush();
		}
	private:
		void _convInputSlideUp(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			ClVariableSet& variables,
			ClEventJoiner* pJoin)
		{
			ClFunc func(_clMachine, _clKernel);
			func.pushArgument(variables.getClMemory(VariableKind::IMAGE));
			func.pushArgument(variables.getClMemory(VariableKind::W));
			func.pushArgument(variables.getClMemory(VariableKind::lambda));

			std::vector<size_t> global_work_size = { 
				static_cast<size_t>(_shape.extent.rows), 
				static_cast<size_t>(_shape.extent.cols)
			};
			func.execute(clDeviceContext, global_work_size, pJoin);

		}

	public:	// コピー禁止・ムーブ禁止
		ConvLambdaInputFmc(const ConvLambdaInputFmc&) = delete;
		ConvLambdaInputFmc(ConvLambdaInputFmc&&) = delete;
		ConvLambdaInputFmc& operator=(const ConvLambdaInputFmc&) = delete;
		ConvLambdaInputFmc& operator=(ConvLambdaInputFmc&&) = delete;

	};
}

#endif // !_MONJU_CONV_LAMBDA_INPUT_FMC_H__

