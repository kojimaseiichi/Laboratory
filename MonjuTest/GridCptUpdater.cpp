#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalcTask.h"
#include "monju/util_math.h"
#include "monju/BayesianNodeStat.h"
#include "monju/FullConnectedGridCpt.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(GridCptUpdater)
	{
		TEST_METHOD(wtf1)
		{
			monju::FullConnectedGridCpt u("grid-cpt-test", { 2, 2 }, { 2, 2 }, 4.0f, 4.0f);
			monju::MatrixRm<int32_t> winX, winY;
			winX.resize(2, 1);
			winY.resize(2, 1);
			winX << 0, 0;
			winY << 0, 0;
			monju::MatrixCm<float_t> lambda;
			lambda.resize(4, 2);
			monju::MatrixCm<float_t> cpt;
			cpt.resize(2 * 2 * 2, 2);
			u.addDelta();

		}
	};
}
