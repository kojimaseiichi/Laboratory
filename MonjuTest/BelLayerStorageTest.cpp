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
			
			void setWinTestData(MatrixRm<int32_t>* p)
			{
				p->coeffRef(0, 0) = 0;
				p->coeffRef(1, 0) = 1;
				p->coeffRef(2, 0) = 2;
				p->coeffRef(3, 0) = 3;
			}

			TEST_METHOD_INITIALIZE(Init)
			{
			}

			/// <summary>
			/// 分割表の更新をテスト
			/// </summary>
			TEST_METHOD(IncrementDiffTest)
			{
				LayerShape shape(2, 2, 2, 2);
				auto flat = shape.flatten();
				// 勝者ユニット
				auto win = std::make_shared<MatrixRm<int32_t>>();
				win->resize(flat.rows, 1);
				setWinTestData(win.get());
				// 勝者データをストレージの分割表に反映
				{
					auto s = BelLayerStorage(R"(C:\monju\test\BelLayerStorage\test.db)", shape);
					s.clearAll();
					auto f = s.asyncIncrementDiff(win);
					f.wait();
				}
				// 分割表を作成
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
				// ストレージから分割表を読み込み
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

			TEST_METHOD(UpdateMiTest)
			{
			}
		};
	}
}