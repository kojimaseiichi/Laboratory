#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/TaskPenaltyCalc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(TaskPenaltyCalc)
	{
		TEST_METHOD(Test1)
		{
			auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
			storage->create(R"(C:\dev\test\cout_test.mat)", 2, 2, 2);
			
			monju::TaskPenaltyCalc task(2, 2, 2, storage, 100.f, 100.f);

			auto win = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
			auto lat = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
			auto penalty = std::make_shared <monju::MatrixRm<float_t>>(2, 2);

			task.calcPenalty(win, lat, penalty);
		}
	};
}
