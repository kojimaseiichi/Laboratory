#pragma once
#ifndef _MONJU_BEL_LAYER_UPDATE_FMC_H__
#define _MONJU_BEL_LAYER_UPDATE_FMC_H__

#include "MonjuTypes.h"
#include "BelLayerFmc.h"
#include "ClFunc.h"
#include "Environment.h"

namespace monju {

	class BelLayerUpdaterFmc
	{
	private: // ストレージ
		const std::string
			_kSrcOobpBel = "oobp\\oobp3_bel.cl",
			_kKernelOobpBel = "oobp3_bel_X${X}_XU${XU}";

		UniformBasisShape _shape;

		std::shared_ptr<Environment> _env;
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel> _clKernel;
		
	public: // コンストラクタ
		BelLayerUpdaterFmc(
			UniformBasisShape shape,
			std::weak_ptr<Environment> env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_initInstance(shape, env, clMachine);
			_createClKernel();
		}
		~BelLayerUpdaterFmc()
		{
		}
		void bel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::weak_ptr<BelLayerFmc> node,
			std::weak_ptr<ClEventJoiner> clEventJoiner)
		{
			/// デバイスに必要なデータが転送されている前提

			auto p = clDeviceContext.lock();
			// joinはnull可能
			_bel(clDeviceContext, node, clEventJoiner);
			p->flush(); // コマンドキューをフラッシュ
		}

	private: // ヘルパー

		void _initInstance(
			UniformBasisShape& shape,
			std::weak_ptr<Environment> env,
				std::weak_ptr<ClMachine> clMachine)
			{
				_capturePointers(env, clMachine);
			_shape = shape;
		}
		void _capturePointers(
			std::weak_ptr<Environment> env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_env = env.lock();
			_clMachine = clMachine.lock();
		}
		void _createClKernel()
		{
			param_map param_map;
			param_map["X"] = boost::lexical_cast<std::string>(_shape.nodes);
			param_map["XU"] = boost::lexical_cast<std::string>(_shape.units);

			std::filesystem::path kernelPathBase = _env->info().kernelFolder;
			_clKernel = std::make_shared<ClKernel>(
				_clMachine,
				(kernelPathBase / _kSrcOobpBel).string(),
				_kKernelOobpBel,
				param_map);
		}
		void _bel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::weak_ptr<BelLayerFmc> node,
			std::weak_ptr<ClEventJoiner> clEventJoiner)
		{
			auto pNode = node.lock();

			ClFunc func(_clMachine, _clKernel);
			func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::lambda));
			func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::pi));
			func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::rho));
			func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::BEL));
			func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::WIN));

			std::vector<size_t> global_work_size = { _shape.nodes };

			func.execute(clDeviceContext, global_work_size, clEventJoiner);
		}

		// コピー禁止・ムーブ禁止
	public:
		BelLayerUpdaterFmc(const BelLayerUpdaterFmc&) = delete;
		BelLayerUpdaterFmc(BelLayerUpdaterFmc&&) = delete;
		BelLayerUpdaterFmc& operator =(const BelLayerUpdaterFmc&) = delete;
		BelLayerUpdaterFmc& operator =(BelLayerUpdaterFmc&&) = delete;
	};
}

#endif // !_MONJU_BEL_LAYER_UPDATE_FMC_H__
