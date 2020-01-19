#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalcTask.h"
#include "monju/util_math.h"
#include "monju/CortexBasisStat.h"
#include "monju/GridCptUpdater.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(CortexBasisStatTest)
	{
		TEST_METHOD(wtf1)
		{
			monju::GridCptUpdater u(2, 2, 2, 2);
			monju::MatrixRm<int32_t> winX, winY;
			winX.resize(2, 1);
			winY.resize(2, 1);
			winX << 0, 0;
			winY << 0, 0;
			monju::MatrixCm<float_t> lambda;
			lambda.resize(4, 2);
			monju::MatrixCm<float_t> cpt;
			cpt.resize(2 * 2 * 2, 2);
			u.wtaDown(winX, winY, lambda);
			u.addDelta(cpt, 1.0f);

		}
	};
}
