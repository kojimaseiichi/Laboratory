#pragma once
#ifndef _MONJU_BAYESIAN_ACTIVATOR_H__
#define _MONJU_BAYESIAN_ACTIVATOR_H__

#include "MonjuTypes.h"
#include <map>
#include <filesystem>
#include <boost/lexical_cast.hpp>
#include "BayesianNodeDevice.h"
#include "BayesianEdgeDevice.h"
#include "util_file.h"
#include "ClMachine.h"
#include "ClFunc.h"
#include "Environment.h"

namespace monju {

	class BayesianInterNodeCompute
	{
	private:
		const std::string
			_kSrcOobpUp1 = "oobp\\oobp3_full_up_1.cl",
			_kSrcOobpUp2 = "oobp\\oobp3_full_up_2.cl",
			_kSrcOobpDown1 = "oobp\\oobp3_full_down_1.cl",
			_kSrcOobpDown2 = "oobp\\oobp3_full_down_2.cl",
			_kKernelOobpUp1 = "oobp3_full_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpUp2 = "oobp3_full_up_2_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown1 = "oobp3_full_down_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown2 = "oobp3_full_down_2_X${X}_Y${Y}_XU${XU}_YU${YU}";

		UniformBasisShape
			_shapeX,
			_shapeY;

		std::shared_ptr<ClMachine> _clMachine;

		std::shared_ptr<ClKernel>
			_kernelOobpUp1,
			_kernelOobpUp2,
			_kernelOobpDown1,
			_kernelOobpDown2;

	public:
		BayesianInterNodeCompute(
			UniformBasisShape shapeX,
			UniformBasisShape shapeY,
			Environment& env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_shapeX = shapeX;
			_shapeY = shapeY;
			_clMachine = clMachine.lock();

			std::map<std::string, std::string> params_map;
			params_map["X"] = boost::lexical_cast<std::string>(shapeX.nodes);
			params_map["Y"] = boost::lexical_cast<std::string>(shapeY.nodes);
			params_map["XU"] = boost::lexical_cast<std::string>(shapeX.units);
			params_map["YU"] = boost::lexical_cast<std::string>(shapeY.units);

			std::filesystem::path kernelPathBase = env.info().kernelFolder;
			_kernelOobpUp1 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpUp1,
				_kKernelOobpUp1,
				params_map);
			_kernelOobpUp2 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpUp2,
				_kKernelOobpUp2,
				params_map);
			_kernelOobpDown1 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpDown1,
				_kKernelOobpDown1,
				params_map);
			_kernelOobpDown2 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpDown2,
				_kKernelOobpDown2,
				params_map);
		}
		~BayesianInterNodeCompute()
		{
		}

		void up(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			_up(clDeviceContext, nodeX, nodeY, edge, pJoin);
			auto p = clDeviceContext.lock();
			p->flush();
		}

		void down(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			_down(clDeviceContext, nodeX, nodeY, edge, pJoin);
			auto p = clDeviceContext.lock();
			p->flush();
		}

		void both(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			_up(clDeviceContext, nodeX, nodeY, edge, pJoin);
			_down(clDeviceContext, nodeX, nodeY, edge, pJoin);
			auto p = clDeviceContext.lock();
			p->flush();
		}

	private:

		void _up(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			_upOobp1(clDeviceContext, nodeX, nodeY, edge, pJoin);
			_upOobp2(clDeviceContext, nodeX, nodeY, edge, pJoin);
		}

		void _upOobp1(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			ClFunc func1(_clMachine, _kernelOobpUp1);

			func1.pushArgument(nodeY.clVariableSet().getClMemory(VariableKind::lambda));
			func1.pushArgument(nodeY.clVariableSet().getClMemory(VariableKind::pi));
			func1.pushArgument(edge.clVariableSet().getClMemory(VariableKind::W));
			func1.pushArgument(edge.clVariableSet().getClMemory(VariableKind::kappa));
			func1.pushArgument(edge.clVariableSet().getClMemory(VariableKind::lambda));

			std::vector<size_t> global_work_size1 = { _shapeY.nodes, _shapeX.nodes };
			std::vector<size_t> local_work_size1 = { 1, _shapeX.nodes };

			func1.execute(clDeviceContext, global_work_size1, local_work_size1, pJoin);
			//edge.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::lambda);
		}

		void _upOobp2(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			ClFunc func2(_clMachine, _kernelOobpUp2);

			func2.pushArgument(edge.clVariableSet().getClMemory(VariableKind::lambda));
			func2.pushArgument(nodeX.clVariableSet().getClMemory(VariableKind::R));
			func2.pushArgument(nodeX.clVariableSet().getClMemory(VariableKind::lambda));

			std::vector<size_t> global_work_size2 = { 1, _shapeX.nodes };

			func2.execute(clDeviceContext, global_work_size2, pJoin);
			//nodeX.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::lambda);
		}

		void _down(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			_downOobp1(clDeviceContext, nodeX, nodeY, edge, pJoin);
			_downOobp2(clDeviceContext, nodeX, nodeY, edge, pJoin);
		}
		void _downOobp1(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			ClFunc func1(_clMachine, _kernelOobpDown1);

			func1.pushArgument(nodeX.clVariableSet().getClMemory(VariableKind::rho));
			func1.pushArgument(edge.clVariableSet().getClMemory(VariableKind::W));
			func1.pushArgument(edge.clVariableSet().getClMemory(VariableKind::lambda));
			func1.pushArgument(edge.clVariableSet().getClMemory(VariableKind::kappa));

			std::vector<size_t> global_work_size1 = { _shapeY.nodes , _shapeX.nodes };
			std::vector<size_t> local_work_size1 = { _shapeY.nodes, 1 };

			func1.execute(clDeviceContext, global_work_size1, local_work_size1, pJoin);
			//edge.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::kappa);
		}
		void _downOobp2(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BayesianNodeDevice& nodeX,
			BayesianNodeDevice& nodeY,
			BayesianEdgeDevice& edge,
			ClEventJoiner* pJoin)
		{
			ClFunc func2(_clMachine, _kernelOobpDown2);

			func2.pushArgument(edge.clVariableSet().getClMemory(VariableKind::kappa));
			func2.pushArgument(nodeY.clVariableSet().getClMemory(VariableKind::pi));

			std::vector<size_t> global_work_size2 = { _shapeY.nodes, 1 };

			func2.execute(clDeviceContext, global_work_size2, pJoin);
			//nodeX.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::pi);
		}


		// コピー禁止・ムーブ禁止
	public:
		BayesianInterNodeCompute(const BayesianInterNodeCompute&) = delete;
		BayesianInterNodeCompute(BayesianInterNodeCompute&&) = delete;
		BayesianInterNodeCompute& operator =(const BayesianInterNodeCompute&) = delete;
		BayesianInterNodeCompute& operator =(BayesianInterNodeCompute&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_ACTIVATOR_H__
