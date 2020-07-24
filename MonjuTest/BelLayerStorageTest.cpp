#include "pch.h"
#include "CppUnitTest.h"
#include "monju/BelLayerStorage.h"
#include <boost/filesystem.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace monju
{
	namespace test
	{
		TEST_CLASS(BelLayerStorageTest)
		{
			TEST_METHOD_INITIALIZE(Init)
			{
			}

			TEST_METHOD(IncrementDiffTest)
			{
				LayerShape shape(2, 2, 2, 2);
				auto win = std::make_shared<MatrixRm<int32_t>>();
				auto flat = shape.flatten();
				win->resize(flat.rows, 1);
				win->coeffRef(0, 0) = 0;
				win->coeffRef(1, 0) = 1;
				win->coeffRef(2, 0) = 2;
				win->coeffRef(3, 0) = 3;
				{
					auto s = BelLayerStorage(R"(C:\monju\test\BelLayerStorage\test.db)", shape);
					s.clearAll();
					auto f = s.asyncIncrementDiff(win);
					f.wait();
				}

				MatrixRm<int32_t> dif;
				GridExtent cross(shape, shape);
				auto crossFlat = cross.flattenRm();
				dif.resize(crossFlat.rows, crossFlat.cols);
				dif.setZero();
				int deg = static_cast<int>(win->rows());
				int matSize = shape.units.size() * shape.units.size();
				for (int row = 0; row < deg; row++)
				{
					for (int col = 0; col < deg; col++)
					{
						auto m = dif.block(row, col * matSize, 1, matSize)
							.reshaped(shape.units.size(), shape.units.size());
						m(win->coeff(row, 0), win->coeff(col, 0)) += 1;
					}
				}

				MatrixRm<int32_t> mir;
				mir.resizeLike(dif);
				mir.setZero();
				{
					auto s = BelLayerStorage(R"(C:\monju\test\BelLayerStorage\test.db)", shape);
					s.persistCrossTabDiff(false, mir);
				}
				for (int grow = 0; grow < deg; grow++)
				{
					for (int gcol = 0; gcol <= grow; gcol++)
					{
						auto b1 = mir.block(grow, gcol * matSize, 1, matSize)
							.reshaped(shape.units.size(), shape.units.size());
						auto b2 = dif.block(grow, gcol * matSize, 1, matSize)
							.reshaped(shape.units.size(), shape.units.size());
						Assert::IsTrue(b1.isApprox(b2, 0));
					}
				}
			}
		};
	}
}