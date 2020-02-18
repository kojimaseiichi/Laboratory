#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalcTask.h"
#include "monju/util_math.h"
#include "monju/BayesianNodeStat.h"
#include "monju/BayesianEdgeStat.h"
#include "monju/BayesianNode.h"
#include "monju//BayesianEdge.h"
#include "monju/GridCpt.h"
#include "monju/BayesianEdgeDevice.h"
#include "monju/BayesianNodeDevice.h"
#include "monju/BayesianInterNodeCompute.h"
#include "monju/BayesianNodeCompute.h"
#include "monju/Environment.h"
#include "monju/VariableKind.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(BayesianModTest)
	{
		TEST_METHOD(Test1)
		{
			std::vector<monju::UniformBasisShape> network{ { 64, 16}, {32, 32}, {32, 32} };
			monju::Environment env(R"(C:\dev\test)");

			// CPU�̋L����m��
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

			// ���v���
			monju::BayesianNodeStat nodeStat1("stat1", network[1], 5.0f, 10.f);
			monju::BayesianNodeStat nodeStat2("stat2", network[2], 5.0f, 10.f);
			//monju::BayesianEdgeStat edgeStat();

			{
				// GPU�g�p����
				std::shared_ptr<monju::ClMachine> clMachine = std::make_shared<monju::ClMachine>(env.info().platformId);

				// GPU�̋L����m��
				monju::BayesianNodeDevice inputLayerDevice(clMachine, inputLayer);
				monju::BayesianNodeDevice layerDevice1(clMachine, layer1);
				monju::BayesianNodeDevice layerDevice2(clMachine, layer2);
				monju::BayesianEdgeDevice inputEdgeDevice(clMachine, inputEdge);
				monju::BayesianEdgeDevice edgeDevice1(clMachine, edge1);

				// GPU�̌v�Z����
				monju::BayesianInterNodeCompute interNodeCmp1(network[1], network[0], env, clMachine);
				monju::BayesianInterNodeCompute interNodeCmp2(network[2], network[1], env, clMachine);
				monju::BayesianNodeCompute nodeCmp1(network[1], env, clMachine);
				monju::BayesianNodeCompute nodeCmp2(network[2], env, clMachine);

				// �f�o�C�X�I��
				auto deviceId = clMachine->deviceIds().at(0);
				// GPU�̊��蓖��
				std::shared_ptr<monju::ClDeviceContext> dc = std::make_shared<monju::ClDeviceContext>(clMachine, deviceId);

				// �񓯊�����
				monju::ClEventJoiner joiner;

				// GPU�������]��
				inputLayerDevice.clVariableSet().enqueueWriteAll(dc, &joiner);
				layerDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);
				layerDevice2.clVariableSet().enqueueWriteAll(dc, &joiner);
				inputEdgeDevice.clVariableSet().enqueueWriteAll(dc, &joiner);
				edgeDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);

				joiner.join();
				for (int n = 0; n < 1000; n++)
				{
					interNodeCmp1.both(dc, layerDevice1, inputLayerDevice, inputEdgeDevice, &joiner);
					interNodeCmp2.both(dc, layerDevice2, layerDevice1, edgeDevice1, &joiner);
					nodeCmp1.bel(dc, layerDevice1, &joiner);
					nodeCmp2.bel(dc, layerDevice2, &joiner);
					
					joiner.join();

					layerDevice1.clVariableSet().enqueueRead(dc, &joiner, monju::VariableKind::WIN);
					layerDevice2.clVariableSet().enqueueRead(dc, &joiner, monju::VariableKind::WIN);

					joiner.join();

					auto win1 = layer1.win();
					auto win2 = layer2.win();
					auto future1 = nodeStat1.accumulate(win1);
					auto future2 = nodeStat2.accumulate(win2);

					if (n % 10 == 0)
					{
						future1.wait();
						future2.wait();
						auto f1 = nodeStat1.calcPenalty();
						auto f2 = nodeStat2.calcPenalty();
						f1.wait();
						f2.wait();


					}
				}
			}
		}
	};
}
 