/*
#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalculation.h"
#include "monju/util_math.h"
#include "monju/BayesianNodeStat.h"
#include "monju/BayesianFullConnectEdgeStat.h"
#include "monju/BayesianNode.h"
#include "monju//BayesianFullConnectEdge.h"
#include "monju/FullConnectedGridCpt.h"
#include "monju/BayesianFullConnectEdgeDevice.h"
#include "monju/BayesianNodeDevice.h"
#include "monju/BayesianFullConnectEdgeFmc.h"
#include "monju/BayesianNodeFmc.h"
#include "monju/Environment.h"
#include "monju/VariableKind.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(BayesianModTest)
	{
		TEST_METHOD(Test2)
		{
			std::vector<monju::UniformBasisShape> network{
				{{32, 1}, 32, 32},
				{{32, 1}, 32, 32}
			};
			monju::Environment env(R"(C:\dev\test)");

			// CPUの記憶域確保
			monju::BayesianNode layer1("layer1", network[0]);
			monju::BayesianNode layer2("layer2", network[1]);
			layer1.initRandom();
			layer2.initRandom();

			monju::BayesianFullConnectEdge edge1("edge1", network[1], network[0], 4.0f, 4.0f);
			edge1.initRandom();

			// 統計情報
			monju::BayesianNodeStat nodeStat1("stat1", network[0], 5.0f, 10.f);
			monju::BayesianNodeStat nodeStat2("stat2", network[1], 5.0f, 10.f);

			{
				// GPU使用準備
				std::shared_ptr<monju::ClMachine> clMachine = std::make_shared<monju::ClMachine>(env.info().platformId);

				// GPUの記憶域確保
				monju::BayesianNodeDevice layerDevice1(clMachine, layer1);
				monju::BayesianNodeDevice layerDevice2(clMachine, layer2);
				monju::BayesianFullConnectEdgeDevice edgeDevice1(clMachine, edge1);

				// GPUの計算資源
				monju::BayesianFullConnectEdgeFmc interNodeCmp1(network[1], network[0], env, clMachine);
				monju::BayesianNodeFmc nodeCmp1(network[0], env, clMachine);
				monju::BayesianNodeFmc nodeCmp2(network[1], env, clMachine);

				// デバイス選択
				auto deviceId = clMachine->deviceIds().at(0);
				// GPUの割り当て
				std::shared_ptr<monju::ClDeviceContext> dc = std::make_shared<monju::ClDeviceContext>(clMachine, deviceId);

				// 非同期処理
				monju::ClEventJoiner joiner;

				// GPUメモリ転送
				layerDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);
				layerDevice2.clVariableSet().enqueueWriteAll(dc, &joiner);
				edgeDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);

				joiner.join();
				for (int n = 0; n < 100; n++)
				{
					interNodeCmp1.both(dc, layerDevice2, layerDevice1, edgeDevice1, &joiner);
					edgeDevice1.clVariableSet().enqueueReadAll(dc, &joiner);
					nodeCmp1.bel(dc, layerDevice1, &joiner);
					layerDevice1.clVariableSet().enqueueReadAll(dc, &joiner);
					nodeCmp2.bel(dc, layerDevice2, &joiner);
					layerDevice2.clVariableSet().enqueueReadAll(dc, &joiner);

					layerDevice1.clVariableSet().enqueueReadAll(dc, &joiner);
					layerDevice2.clVariableSet().enqueueReadAll(dc, &joiner);
					edgeDevice1.clVariableSet().enqueueReadAll(dc, &joiner);

					joiner.join();

					auto win1 = layer1.win();
					auto win2 = layer2.win();
					auto future1 = nodeStat1.accumulate(win1);
					auto future2 = nodeStat2.accumulate(win2);

					//edge1.cpt().train(layer2.win(), layer1.win(), edge1.lambda(), 0.1);	// 学習
				}
				Assert::IsFalse(edge1.containsNan(), L"(edge1)");
				Assert::IsFalse(layer1.containsNan(), L"(layer1)");
				Assert::IsFalse(layer2.containsNan(), L"(layer2)");
			}
		}
		TEST_METHOD(Test1)
		{
			std::vector<monju::UniformBasisShape> network{
				{{32, 1}, 32, 32},
				{{32, 1}, 32, 32},
				{{32, 1}, 32, 32}
			};
			monju::Environment env(R"(C:\dev\test)");

			// CPUの記憶域確保
			monju::BayesianNode inputLayer("input-layer", network[0]);
			monju::BayesianNode layer1("layer1", network[1]);
			monju::BayesianNode layer2("layer2", network[2]);
			inputLayer.initRandom();
			layer1.initRandom();
			layer2.initRandom();

			monju::BayesianFullConnectEdge inputEdge("input-edge", network[1], network[0], 4.0f, 4.0f);
			monju::BayesianFullConnectEdge edge1("edge1", network[2], network[1], 4.0f, 4.0f);
			inputEdge.initRandom();
			edge1.initRandom();

			// 統計情報
			monju::BayesianNodeStat nodeStat1("stat1", network[1], 5.0f, 10.f);
			monju::BayesianNodeStat nodeStat2("stat2", network[2], 5.0f, 10.f);
			//monju::BayesianFullConnectEdgeStat edgeStat();

			{
				// GPU使用準備
				std::shared_ptr<monju::ClMachine> clMachine = std::make_shared<monju::ClMachine>(env.info().platformId);

				// GPUの記憶域確保
				monju::BayesianNodeDevice inputLayerDevice(clMachine, inputLayer);
				monju::BayesianNodeDevice layerDevice1(clMachine, layer1);
				monju::BayesianNodeDevice layerDevice2(clMachine, layer2);
				monju::BayesianFullConnectEdgeDevice inputEdgeDevice(clMachine, inputEdge);
				monju::BayesianFullConnectEdgeDevice edgeDevice1(clMachine, edge1);

				// GPUの計算資源
				monju::BayesianFullConnectEdgeFmc interNodeCmp1(network[1], network[0], env, clMachine);
				monju::BayesianFullConnectEdgeFmc interNodeCmp2(network[2], network[1], env, clMachine);
				monju::BayesianNodeFmc nodeCmp1(network[1], env, clMachine);
				monju::BayesianNodeFmc nodeCmp2(network[2], env, clMachine);

				// デバイス選択
				auto deviceId = clMachine->deviceIds().at(0);
				// GPUの割り当て
				std::shared_ptr<monju::ClDeviceContext> dc = std::make_shared<monju::ClDeviceContext>(clMachine, deviceId);

				// 非同期処理
				monju::ClEventJoiner joiner;

				// GPUメモリ転送
				inputLayerDevice.clVariableSet().enqueueWriteAll(dc, &joiner);
				layerDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);
				layerDevice2.clVariableSet().enqueueWriteAll(dc, &joiner);
				inputEdgeDevice.clVariableSet().enqueueWriteAll(dc, &joiner);
				edgeDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);

				joiner.join();
				for (int n = 0; n < 100; n++)
				{
					interNodeCmp1.both(dc, layerDevice1, inputLayerDevice, inputEdgeDevice, &joiner);
					interNodeCmp2.both(dc, layerDevice2, layerDevice1, edgeDevice1, &joiner);
					nodeCmp1.bel(dc, layerDevice1, &joiner);
					nodeCmp2.bel(dc, layerDevice2, &joiner);
					
					joiner.join();

					layerDevice1.clVariableSet().enqueueRead(dc, &joiner, monju::VariableKind::WIN);
					layerDevice2.clVariableSet().enqueueRead(dc, &joiner, monju::VariableKind::WIN);
					edgeDevice1.clVariableSet().enqueueRead(dc, &joiner, monju::VariableKind::W);

					// 確認
					layerDevice1.clVariableSet().enqueueReadAll(dc, &joiner);
					layerDevice2.clVariableSet().enqueueReadAll(dc, &joiner);
					edgeDevice1.clVariableSet().enqueueReadAll(dc, &joiner);

					joiner.join();

					auto win1 = layer1.win();
					auto win2 = layer2.win();
					auto future1 = nodeStat1.accumulate(win1);
					auto future2 = nodeStat2.accumulate(win2);

					// TODO 入力層と接続するCPTの学習
					bool a = layer1.containsNan();
					bool b = layer2.containsNan();
					bool c = edge1.containsNan();
					edge1.cpt().train(layer2.win(), layer1.win(), edge1.lambda(), 0.1);	// 学習

					if (n % 10 == 0)
					{
						future1.wait();
						future2.wait();
						auto f1 = nodeStat1.calcPenalty();
						auto f2 = nodeStat2.calcPenalty();
						f1.wait();
						f2.wait();
						
						monju::util_eigen::copy(nodeStat1.penalty(), layer1.r());
						monju::util_eigen::copy(nodeStat2.penalty(), layer2.r());

						layerDevice1.clVariableSet().enqueueWrite(dc, &joiner, monju::VariableKind::R);
						layerDevice2.clVariableSet().enqueueWrite(dc, &joiner, monju::VariableKind::R);

						joiner.join();
					}
				}
			}
		}
	};
}
*/