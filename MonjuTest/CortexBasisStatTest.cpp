/*
#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalculation.h"
#include "monju/util_math.h"
#include "monju/BayesianNodeStat.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(CortexBasisStatTest)
	{
		TEST_METHOD(Persist1)
		{
			//auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
			//storage->create(R"(C:\dev\test\hoge_tri.gma)", 2, 2, 2);
			//monju::MatrixRm<int32_t> m(2, 2);
			//monju::MatrixRowMajorAccessor a(m);
			//m << 1, 0, 0, 4;
			//storage->writeCell(a, 0, 0);
			//m << 2, 3, 4, 5;
			//storage->writeCell(a, 1, 0);
			//m << 3, 0, 0, 6;
			//storage->writeCell(a, 1, 1);
			//storage->close();

			//monju::BayesianNodeStat stat("hoge", { 2, 2 }, 2.f, 5.f);
			//stat.create(R"(C:\dev\test)");
			//
			//monju::MatrixRm<int32_t> win(2, 1);
			//win << 0, 0;

			////auto f1 = stat.persist(win);
			////f1.wait();

			//auto f2 = stat.calcPenalty();
			//f2.wait();

			//monju::MatrixRm<float_t> penalty = stat.penalty();

			//Assert::AreEqual(monju::util_math::round_n(4.000175171, 3), monju::util_math::round_n(penalty(0, 0), 3));
			//Assert::AreEqual(monju::util_math::round_n(0.816292572f, 3), monju::util_math::round_n(penalty(0, 1), 3));
			//Assert::AreEqual(monju::util_math::round_n(1.454513189f, 3), monju::util_math::round_n(penalty(1, 0), 3));
			//Assert::AreEqual(monju::util_math::round_n(0.898379721f, 3), monju::util_math::round_n(penalty(1, 1), 3));
		}

		TEST_METHOD(Persist2)
		{
			//auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
			//storage->create(R"(C:\dev\test\hoge_tri.gma)", 2, 2, 2);
			//monju::MatrixRm<int32_t> m(2, 2);
			//monju::MatrixRowMajorAccessor a(m);
			//m << 1, 0, 0, 4;
			//storage->writeCell(a, 0, 0);
			//m << 2, 3, 4, 5;
			//storage->writeCell(a, 1, 0);
			//m << 3, 0, 0, 6;
			//storage->writeCell(a, 1, 1);
			//storage->close();

			//monju::BayesianNodeStat stat("hoge", { 2, 2 }, 2.f, 5.f);
			//stat.create(R"(C:\dev\test)");

			//monju::MatrixRm<int32_t> win(2, 1);
			//win << 0, 0;
			//auto f1 = stat.accumulate(win);
			//win << 0, 1;
			//auto f2 = stat.accumulate(win);
			//f1.wait();
			//f2.wait();

			//auto f3 = stat.calcPenalty();
			//f3.wait();

			//monju::MatrixRm<float_t> penalty = stat.penalty();

			//Assert::AreEqual(monju::util_math::round_n(1.111827138f, 4), monju::util_math::round_n(penalty(0, 0), 4));
			//Assert::AreEqual(monju::util_math::round_n(0.938740393f, 4), monju::util_math::round_n(penalty(0, 1), 4));
			//Assert::AreEqual(monju::util_math::round_n(1.255309760f, 4), monju::util_math::round_n(penalty(1, 0), 4));
			//Assert::AreEqual(monju::util_math::round_n(0.927761223f, 4), monju::util_math::round_n(penalty(1, 1), 4));
		}
	};
}
*/