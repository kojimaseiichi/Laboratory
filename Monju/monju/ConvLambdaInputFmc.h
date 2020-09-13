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
#include "Extent.h"

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
		LayerShape _shape;


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

			param_map params;
			params["W"] = boost::lexical_cast<std::string>(_extentInput.cols);
			params["H"] = boost::lexical_cast<std::string>(_extentInput.rows);
			params["FW"] = boost::lexical_cast<std::string>(_extentFilter.cols);
			params["FH"] = boost::lexical_cast<std::string>(_extentFilter.rows);
			params["SW"] = boost::lexical_cast<std::string>(_extentSlide.cols);
			params["SH"] = boost::lexical_cast<std::string>(_extentSlide.rows);
			params["X"] = boost::lexical_cast<std::string>(_shape.nodes.size());
			params["XU"] = boost::lexical_cast<std::string>(_shape.units.size());

			std::filesystem::path kernelPathBase = env.info().kernelFolder;
			_clKernel = std::make_shared<ClKernel>(
				_clMachine,
				(kernelPathBase / _kSrcFile).string(),
				_kKernel,
				params);

		}
		~ConvLambdaInputFmc()
		{

		}
		void convInputSlideUp(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			ClVariableSet& variables,
			std::weak_ptr<ClEventJoiner> join)
		{
			_convInputSlideUp(clDeviceContext, variables, join);
			auto p = clDeviceContext.lock();
			p->flush();
		}
	private:
		void _convInputSlideUp(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			ClVariableSet& variables,
			std::weak_ptr<ClEventJoiner> join)
		{
			ClFunc func(_clMachine, _clKernel);
			func.push_kernel_argument(variables.get_cl_memory(VariableKind::IMAGE));
			func.push_kernel_argument(variables.get_cl_memory(VariableKind::W));
			func.push_kernel_argument(variables.get_cl_memory(VariableKind::lambda));

			std::vector<size_t> global_work_size = { 
				static_cast<size_t>((_extentInput.rows - _extentFilter.rows) / _extentSlide.rows + 1), 
				static_cast<size_t>((_extentInput.cols - _extentFilter.cols) / _extentSlide.cols + 1)
			};
			func.execute_kernel(clDeviceContext, global_work_size, join);

		}

	public:	// コピー禁止・ムーブ禁止
		ConvLambdaInputFmc(const ConvLambdaInputFmc&) = delete;
		ConvLambdaInputFmc(ConvLambdaInputFmc&&) = delete;
		ConvLambdaInputFmc& operator=(const ConvLambdaInputFmc&) = delete;
		ConvLambdaInputFmc& operator=(ConvLambdaInputFmc&&) = delete;

	};
}

#endif // !_MONJU_CONV_LAMBDA_INPUT_FMC_H__

