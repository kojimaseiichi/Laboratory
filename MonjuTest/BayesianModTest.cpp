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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(BayesianModTest)
	{
		TEST_METHOD(Test1)
		{
			monju::MatrixRm<int> networkShape(2, 3);
			networkShape <<
				2, 4,	// 2 nodes, 4 units per node
				2, 4,
				2, 4;

			monju::BayesianNode inputLayer("input-layer", networkShape(0, 0), networkShape(0, 1));
			monju::BayesianNode layer1("layer1", networkShape(1, 0), networkShape(1, 1));
			monju::BayesianNode layer2("layer2", networkShape(2, 0), networkShape(2, 1));

			monju::BayesianEdge inputEdge("input-edge", networkShape(1, 0), networkShape(1, 1), networkShape(0, 0), networkShape(0, 1));
			monju::BayesianEdge edge1("edge1", networkShape(2, 0), networkShape(2, 1), networkShape(1, 0), networkShape(1, 1));

			monju::GridCpt cpt2("cpt2", networkShape(1, 0), networkShape(1, 1), networkShape(0, 0), networkShape(0, 1));
			
		}
	};
}
