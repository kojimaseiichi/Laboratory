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
#include "monju/BayesianNodeCompute.h"
#include "monju/Environment.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(BayesianModTest)
	{
		TEST_METHOD(Test1)
		{
			std::vector<monju::UniformBasisShape> network{ { 1, 256}, {32, 32}, {32, 32} };
			monju::Environment env(R"(C:\dev\test)");

			// CPUの記憶域確保
			monju::BayesianNode inputLayer("input-layer", network[0]);
			monju::BayesianNode layer1("layer1", network[1]);
			monju::BayesianNode layer2("layer2", network[2]);
			inputLayer.initRandom();
			layer1.initRandom();
			layer2.initRandom();

			monju::BayesianEdge inputEdge("input-edge", network[1], network[0]);
			monju::BayesianEdge edge1("edge1", network[2], network[1]);
			inputEdge.initRandom();
			edge1.initRandom();

			// 統計情報
			monju::BayesianNodeStat nodeStat1("stat1", network[1], 5.0f, 10.f);
			monju::BayesianNodeStat nodeStat2("stat2", network[2], 5.0f, 10.f);

			try
			{
				// GPU使用準備
				std::shared_ptr<monju::ClMachine> clMachine = std::make_shared<monju::ClMachine>(env.info().platformId);

				// GPUの記憶域確保
				monju::BayesianNodeDevice inputLayerDevice(clMachine, inputLayer);
				monju::BayesianNodeDevice layerDevice1(clMachine, layer1);
				monju::BayesianNodeDevice layerDevice2(clMachine, layer2);
				monju::BayesianEdgeDevice inputEdgeDevice(clMachine, inputEdge);
				monju::BayesianEdgeDevice edgeDevice1(clMachine, edge1);

				// GPUの計算資源
				monju::BayesianInterNodeCompute interNodeCmp1(network[1], network[0], env, clMachine);
				monju::BayesianInterNodeCompute interNodeCmp2(network[2], network[1], env, clMachine);
				monju::BayesianNodeCompute nodeCmp1(network[1], env, clMachine);
				monju::BayesianNodeCompute nodeCmp2(network[2], env, clMachine);

				// デバイス選択
				auto deviceId = clMachine->deviceIds().at(0);
				// GPUの割り当て
				std::shared_ptr<monju::ClDeviceContext> dc = std::make_shared<monju::ClDeviceContext>(clMachine, deviceId);

				// 非同期処理
				monju::ClEventJoiner joiner;

				// GPUメモリ転送
				inputLayerDevice.clVariableSet().enqueueWriteAll(dc, nullptr);
				layerDevice1.clVariableSet().enqueueWriteAll(dc, nullptr);
				layerDevice2.clVariableSet().enqueueWriteAll(dc, nullptr);
				inputEdgeDevice.clVariableSet().enqueueWriteAll(dc, nullptr);
				edgeDevice1.clVariableSet().enqueueWriteAll(dc, nullptr);

				for (int n = 0; n < 10000; n++)
				{
					interNodeCmp1.both(dc, layerDevice1, inputLayerDevice, inputEdgeDevice, &joiner);
					interNodeCmp2.both(dc, layerDevice2, layerDevice1, edgeDevice1, &joiner);
					nodeCmp1.bel(dc, layerDevice1, &joiner);
					nodeCmp2.bel(dc, layerDevice2, &joiner);
					joiner.join();
				}
			}
			catch (...) {

			}
		}
	};
}
