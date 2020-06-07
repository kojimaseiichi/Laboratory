#pragma once
#ifndef _MONJU_BAYESIAN_FULL_MATRIX_LAYER_UPDATER_FMC_H__
#define _MONJU_BAYESIAN_FULL_MATRIX_LAYER_UPDATER_FMC_H__

#include "MonjuTypes.h"
#include <boost/lexical_cast.hpp>
#include "BelLayerFmc.h"
#include "FullBayesianMatrixLayerFmc.h"
#include "util_file.h"
#include "ClMachine.h"
#include "ClFunc.h"
#include "Environment.h"
#include "Extent.h"
#include "ClEventJoiner.h"

namespace monju {
	// fast massive compute
	class FullBayesianMatrixLayerUpdaterFmc
	{
	private: // OpenCLのファイルとカーネル名
		const std::string
			_kSrcOobpUp1 = "oobp\\oobp3_full_up_1.cl",
			_kSrcOobpUp2 = "oobp\\oobp3_full_up_2.cl",
			_kSrcOobpDown1 = "oobp\\oobp3_full_down_1.cl",
			_kSrcOobpDown2 = "oobp\\oobp3_full_down_2.cl",
			_kKernelOobpUp1 = "oobp3_full_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpUp2 = "oobp3_full_up_2_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown1 = "oobp3_full_down_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown2 = "oobp3_full_down_2_X${X}_Y${Y}_XU${XU}_YU${YU}";

	private: // ストレージ
		LayerShape
			_shapeX,
			_shapeY;
		std::shared_ptr<Environment> _env;
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel>
			_kernelOobpUp1,
			_kernelOobpUp2,
			_kernelOobpDown1,
			_kernelOobpDown2;

	public: // コンストラクタ
		FullBayesianMatrixLayerUpdaterFmc(
			std::weak_ptr<Environment> env,
			LayerShape shapeX,
			LayerShape shapeY,
			std::weak_ptr<ClMachine> clMachine);
		~FullBayesianMatrixLayerUpdaterFmc();

	public: // 公開メンバ
		void up(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::weak_ptr<BelLayerFmc> x,
			std::weak_ptr<BelLayerFmc> y,
			std::weak_ptr<FullBayesianMatrixLayerFmc> m,
			std::weak_ptr<ClEventJoiner> join);

		void down(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::weak_ptr<BelLayerFmc> x,
			std::weak_ptr<BelLayerFmc> y,
			std::weak_ptr<FullBayesianMatrixLayerFmc> m,
			std::weak_ptr<ClEventJoiner> join);

		void both(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::weak_ptr<BelLayerFmc> x,
			std::weak_ptr<BelLayerFmc> y,
			std::weak_ptr<FullBayesianMatrixLayerFmc> m,
			std::weak_ptr<ClEventJoiner> join);

	private: // ヘルパー
		void _captureExternalResources(
			LayerShape shapeX,
			LayerShape shapeY,
			std::weak_ptr<Environment> env,
			std::weak_ptr<ClMachine> clMachine);
		void _createKernel();
		void _up(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerX,
			BelLayerFmc& layerY,
			FullBayesianMatrixLayerFmc& edge,
			std::weak_ptr<ClEventJoiner> join);

		void _upOobp1(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerY,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join);

		void _upOobp2(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerX,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join);

		void _down(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& nodeX,
			BelLayerFmc& nodeY,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join);
		void _downOobp1(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerX,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join);
		void _downOobp2(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerY,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join);


		// コピー禁止・ムーブ禁止
	public:
		FullBayesianMatrixLayerUpdaterFmc(const FullBayesianMatrixLayerUpdaterFmc&) = delete;
		FullBayesianMatrixLayerUpdaterFmc(FullBayesianMatrixLayerUpdaterFmc&&) = delete;
		FullBayesianMatrixLayerUpdaterFmc& operator =(const FullBayesianMatrixLayerUpdaterFmc&) = delete;
		FullBayesianMatrixLayerUpdaterFmc& operator =(FullBayesianMatrixLayerUpdaterFmc&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_FULL_MATRIX_LAYER_UPDATER_FMC_H__
