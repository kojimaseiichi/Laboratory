#include "pch.h"
#include "CppUnitTest.h"
#include "monju/MonjuTypes.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace monju
{
	namespace test
	{
		TEST_CLASS(EigenTest)
		{
			TEST_METHOD(TestHalfConversion)
			{
				MatrixRm<float> a(2, 2), c(2, 2);
				a << 1.f, 2.f, 3.f, 4.f;
				MatrixRm<Eigen::half> h(2, 2);
				h = a.cast<Eigen::half>();
				c = h.cast<float>();
				float prec = 0.01f;
				Assert::IsTrue(a.isApprox(c, prec));
			}
		};
	}
}