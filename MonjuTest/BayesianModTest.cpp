#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalcTask.h"
#include "monju/util_math.h"
#include "monju/BayesianNodeStat.h"
#include "monju/BayesianNode.h"
#include "monju//BayesianEdge.h"
#include "monju/GridCpt.h"
#include "monju/BayesianEdgeDevice.h"
#include "monju/BayesianNodeDevice.h"
#include "monju/BayesianInterNodeCompute.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(BayesianModTest)
	{
		TEST_METHOD(Test1)
		{
			monju::MatrixRm<int> networkShape(3, 2);
			networkShape <<
				2, 4,	// 2 nodes, 4 units per node
				2, 4,
				2, 4;

			// CPUの記憶域確保
			monju::BayesianNode inputLayer("input-layer", networkShape(0, 0), networkShape(0, 1));
			monju::BayesianNode layer1("layer1", networkShape(1, 0), networkShape(1, 1));
			monju::BayesianNode layer2("layer2", networkShape(2, 0), networkShape(2, 1));

			monju::BayesianEdge inputEdge("input-edge", networkShape(1, 0), networkShape(1, 1), networkShape(0, 0), networkShape(0, 1));
			monju::BayesianEdge edge1("edge1", networkShape(2, 0), networkShape(2, 1), networkShape(1, 0), networkShape(1, 1));

			monju::GridCpt cpt1("cpt1", networkShape(1, 0), networkShape(1, 1), networkShape(0, 0), networkShape(0, 1));
			monju::GridCpt cpt2("cpt2", networkShape(2, 0), networkShape(2, 1), networkShape(1, 0), networkShape(1, 1));

			// GPU使用準備
			std::shared_ptr<monju::PlatformContext> ctx = std::make_shared<monju::PlatformContext>();
			ctx->open(R"(C:\dev\test)");
			auto& device = ctx->deviceContext().getDevice(0);

			// GPUの記憶域確保
			monju::BayesianNodeDevice inputLayerDevice(device, inputLayer);
			monju::BayesianNodeDevice layerDevice1(device, layer1);
			monju::BayesianNodeDevice layerDevice2(device, layer2);
			monju::BayesianEdgeDevice inputEdgeDevice(device, inputEdge, cpt1);
			monju::BayesianEdgeDevice edgeDevice1(device, edge1, cpt2);

			// GPUの計算資源
			monju::BayesianInterNodeCompute interNodeCmp1(networkShape(1,0), networkShape(0,0), networkShape(1,1), networkShape(0,1), ctx);
			monju::BayesianInterNodeCompute interNodeCmp2(networkShape(2, 0), networkShape(1, 0), networkShape(2, 1), networkShape(1, 1), ctx);

			// 計算の実行
			interNodeCmp1.both(layerDevice1, inputLayerDevice, inputEdgeDevice);
			interNodeCmp2.both(layerDevice2, layerDevice1, edgeDevice1);
		}
	};
}
