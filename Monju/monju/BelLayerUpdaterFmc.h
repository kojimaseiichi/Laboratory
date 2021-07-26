#pragma once
#ifndef _MONJU_BEL_LAYER_UPDATE_FMC_H__
#define _MONJU_BEL_LAYER_UPDATE_FMC_H__

#include <memory>
#include "Extent.h"

namespace monju {

	class ClDeviceContext;
	class Environment;
	class ClMachine;
	class ClKernel;
	class BelLayerFmc;
	class ClEventJoiner;

	class BelLayerUpdaterFmc
	{
	private:/*フィール*/
		const std::string
			_kSrcOobpBel = "oobp\\oobp3_bel.cl",
			_kKernelOobpBel = "oobp3_bel_X${X}_XU${XU}";
		LayerShape _shape;
		std::shared_ptr<Environment> _env;
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel> _clKernel;
		
	public:/*コンストラクタ*/
		BelLayerUpdaterFmc(
			std::weak_ptr<Environment> env,
			LayerShape shape,
			std::weak_ptr<ClMachine> clMachine);
		~BelLayerUpdaterFmc();
		void bel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::weak_ptr<BelLayerFmc> layer,
			std::weak_ptr<ClEventJoiner> clEventJoiner);

	private:/*ヘルパー*/
		void _initInstance(
			LayerShape& shape,
			std::weak_ptr<Environment> env,
			std::weak_ptr<ClMachine> clMachine);
		void _capturePointers(
			std::weak_ptr<Environment> env,
			std::weak_ptr<ClMachine> clMachine);
		void _createClKernel();
		void _bel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::weak_ptr<BelLayerFmc> layer,
			std::weak_ptr<ClEventJoiner> clEventJoiner);

	public: /*コピー禁止・ムーブ禁止*/
		BelLayerUpdaterFmc(const BelLayerUpdaterFmc&) = delete;
		BelLayerUpdaterFmc(BelLayerUpdaterFmc&&) = delete;
		BelLayerUpdaterFmc& operator =(const BelLayerUpdaterFmc&) = delete;
		BelLayerUpdaterFmc& operator =(BelLayerUpdaterFmc&&) = delete;
	};
}

#endif // !_MONJU_BEL_LAYER_UPDATE_FMC_H__
