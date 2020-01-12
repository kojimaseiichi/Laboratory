#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/TaskPenaltyCalc.h"
#include "monju/util_math.h"
#include "monju/CortexBasisStat.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(CortexBasisStatTest)
	{
		TEST_METHOD(Persist1)
		{
			auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
			storage->create(R"(C:\dev\test\hoge_tri.gma)", 2, 2, 2);
			monju::MatrixRm<int32_t> m(2, 2);
			monju::MatrixRowMajorAccessor a(m);
			m << 1, 0, 0, 4;
			storage->writeCell(a, 0, 0);
			m << 2, 3, 4, 5;
			storage->writeCell(a, 1, 0);
			m << 3, 0, 0, 6;
			storage->writeCell(a, 1, 1);
			storage->close();

			monju::CortexBasisStat stat("hoge", 2, 2);
			stat.create(R"(C:\dev\test)");
			
			monju::MatrixRm<int32_t> win(2, 1);
			win << 0, 0;

			//auto f1 = stat.persist(win);
			//f1.wait();

			auto f2 = stat.calcPenalty();
			f2.wait();

			monju::MatrixRm<float_t> penalty = stat.penalty();

			Assert::AreEqual(monju::util_math::round_n(1.91968E-10f, 3), monju::util_math::round_n(penalty(0, 0), 3));
			Assert::AreEqual(monju::util_math::round_n(2.67588058f, 3), monju::util_math::round_n(penalty(0, 1), 3));
			Assert::AreEqual(monju::util_math::round_n(0.054511065f, 3), monju::util_math::round_n(penalty(1, 0), 3));
			Assert::AreEqual(monju::util_math::round_n(1.746221814f, 3), monju::util_math::round_n(penalty(1, 1), 3));
		}

		TEST_METHOD(Persist2)
		{
			auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
			storage->create(R"(C:\dev\test\hoge_tri.gma)", 2, 2, 2);
			monju::MatrixRm<int32_t> m(2, 2);
			monju::MatrixRowMajorAccessor a(m);
			m << 1, 0, 0, 4;
			storage->writeCell(a, 0, 0);
			m << 2, 3, 4, 5;
			storage->writeCell(a, 1, 0);
			m << 3, 0, 0, 6;
			storage->writeCell(a, 1, 1);
			storage->close();

			monju::CortexBasisStat stat("hoge", 2, 2);
			stat.create(R"(C:\dev\test)");

			monju::MatrixRm<int32_t> win(2, 1);
			win << 0, 0;
			auto f1 = stat.persist(win);
			win << 0, 1;
			auto f2 = stat.persist(win);
			f1.wait();
			f2.wait();

			auto f3 = stat.calcPenalty();
			f3.wait();

			monju::MatrixRm<float_t> penalty = stat.penalty();

			Assert::AreEqual(monju::util_math::round_n(0.406890041f, 4), monju::util_math::round_n(penalty(0, 0), 4));
			Assert::AreEqual(monju::util_math::round_n(1.595775579f, 4), monju::util_math::round_n(penalty(0, 1), 4));
			Assert::AreEqual(monju::util_math::round_n(0.173514255f, 4), monju::util_math::round_n(penalty(1, 0), 4));
			Assert::AreEqual(monju::util_math::round_n(1.605943096f, 4), monju::util_math::round_n(penalty(1, 1), 4));
		}
	};
}