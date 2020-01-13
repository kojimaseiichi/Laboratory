#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalcTask.h"
#include "monju/util_math.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(TaskPenaltyCalc)
	{
		TEST_METHOD(CalcPenalty)
		{
			auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
			storage->create(R"(C:\dev\test\cout_test.mat)", 2, 2, 2);
			
			monju::MatrixRm<int32_t> m(2, 2);
			monju::MatrixRowMajorAccessor a(m);
			m << 1, 2, 3, 4;
			storage->writeCell(a, 0, 0);
			m << 2, 3, 4, 5;
			storage->writeCell(a, 1, 0);
			m << 3, 4, 5, 6;
			storage->writeCell(a, 1, 1);

			storage->readCell(a, 0, 0);
			storage->readCell(a, 1, 0);
			storage->readCell(a, 1, 1);

			monju::PenaltyCalcTask task(2, 2, 2, storage, 2.f, 5.f);

			auto win = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
			auto lat = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
			auto penalty = std::make_shared <monju::MatrixRm<float_t>>(2, 2);

			win->setZero();
			lat->setZero();
			penalty->setZero();

			task.calcPenalty(win, lat, penalty);

			Assert::AreEqual(monju::util_math::round_n(4.000175171f, 3), monju::util_math::round_n((*penalty)(0, 0), 3));
			Assert::AreEqual(monju::util_math::round_n(0.816292572f, 3), monju::util_math::round_n((*penalty)(0, 1), 3));
			Assert::AreEqual(monju::util_math::round_n(1.454513189f, 3), monju::util_math::round_n((*penalty)(1, 0), 3));
			Assert::AreEqual(monju::util_math::round_n(0.898379721f, 3), monju::util_math::round_n((*penalty)(1, 1), 3));
		}
	};
}
