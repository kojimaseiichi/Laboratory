#pragma once
#ifndef _MONJU_BAYESIAN_NODE_FMC_H__
#define _MONJU_BAYESIAN_NODE_FMC_H__

#include "MonjuTypes.h"
#include <map>
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include "BayesianNodeDevice.h"
#include "ClFunc.h"
#include "Environment.h"

namespace monju {

	class BayesianNodeFmc
	{
	private:
		const std::string
			_kSrcOobpBel = "oobp\\oobp3_bel.cl",
			_kKernelOobpBel = "oobp3_bel_X${X}_XU${XU}";

		UniformBasisShape _shape;

		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel> _clKernel;
		
	public:
		BayesianNodeFmc(
			UniformBasisShape shape,
			Environment& env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_clMachine = clMachine.lock();
			_shape = shape;

			param_map param_map;
			param_map["X"] = boost::lexical_cast<std::string>(shape.nodes);
			param_map["XU"] = boost::lexical_cast<std::string>(shape.units);

			std::filesystem::path kernelPathBase = env.info().kernelFolder;
			_clKernel = std::make_shared<ClKernel>(
				_clMachine,
				(kernelPathBase / _kSrcOobpBel).string(),
				_kKernelOobpBel,
				param_map);
		}
		~BayesianNodeFmc()
		{
		}
		void bel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& node,
			ClEventJoiner* pJoin)
		{
			_bel(clDeviceContext, node, pJoin);
			auto p = clDeviceContext.lock();
			p->flush();
		}

	private:
		void _bel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& node,
			ClEventJoiner* pJoin)
		{
			ClFunc func(_clMachine, _clKernel);
			func.pushArgument(node.clVariableSet().getClMemory(VariableKind::lambda));
			func.pushArgument(node.clVariableSet().getClMemory(VariableKind::pi));
			func.pushArgument(node.clVariableSet().getClMemory(VariableKind::rho));
			func.pushArgument(node.clVariableSet().getClMemory(VariableKind::BEL));
			func.pushArgument(node.clVariableSet().getClMemory(VariableKind::WIN));

			std::vector<size_t> global_work_size = { _shape.nodes };

			func.execute(clDeviceContext, global_work_size, pJoin);
			//node.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::rho);
			//node.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::BEL);
			//node.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::WIN);
		}

		// コピー禁止・ムーブ禁止
	public:
		BayesianNodeFmc(const BayesianNodeFmc&) = delete;
		BayesianNodeFmc(BayesianNodeFmc&&) = delete;
		BayesianNodeFmc& operator =(const BayesianNodeFmc&) = delete;
		BayesianNodeFmc& operator =(BayesianNodeFmc&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_NODE_FMC_H__
