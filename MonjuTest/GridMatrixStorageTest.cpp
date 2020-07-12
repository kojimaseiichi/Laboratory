#include "pch.h"
#include "CppUnitTest.h"
#include "monju/GridMatrixStorage.h"
#include <boost/filesystem.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace monju
{
	namespace test
	{
		TEST_CLASS(GridMatrixStorageTest)
		{
			TEST_METHOD(KeyValueTest)
			{
				std::string file = "C:\\monju\\test\\GridMatrixStorage\\KeyValueTest.dbm";
				boost::filesystem::remove(file);
				monju::GridMatrixStorage storage(file);
				
				storage.setKey("a", 100);
				int a;
				Assert::IsTrue(storage.findKey("a", a));
				Assert::AreEqual(a, 100);

				storage.setKey("b", 3.14f);
				float b;
				Assert::IsTrue(storage.findKey("b", b));
				Assert::AreEqual(b, 3.14f);

				storage.setKey("c", "hoge");
				std::string c;
				Assert::IsTrue(storage.findKey("c", c));
				Assert::AreEqual(c, std::string("hoge"));

				storage.removeKey("a");
				Assert::IsFalse(storage.findKey("a", a));
				storage.removeKey("b");
				Assert::IsFalse(storage.findKey("b", b));
				storage.removeKey("c");
				Assert::IsFalse(storage.findKey("c", c));
			}
			
			TEST_METHOD(RemoveTest)
			{
				std::string file = "C:\\monju\\test\\GridMatrixStorage\\GridRemoveTest.dbm";
				boost::filesystem::remove(file);
				GridExtent grid(2, 2, 3, 4);
				Extent flatten = grid.flattenCm();
				std::string name = "grid";
				monju::MatrixCm<float> m1;
				m1.resize(flatten.rows, flatten.cols);
				m1 <<
					1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
					2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
					3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15,
					4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16;
				// ÇPâÒñ⁄ÇÃprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, grid, kDensityRectangular, kColMajor, kColMajor);
					storage.writeGrid(name, m1);
				}
				// ÉOÉäÉbÉhçÌèú
				{
					monju::GridMatrixStorage storage(file);
					storage.setZeros(name);
					storage.removeGrid(name);
				}
			}

			TEST_METHOD(ClearTest)
			{
				std::string file = "C:\\monju\\test\\GridMatrixStorage\\GridClearTest.dbm";
				boost::filesystem::remove(file);
				GridExtent grid(2, 2, 3, 4);
				Extent flatten = grid.flattenCm();
				std::string name = "grid";
				monju::MatrixCm<float> m1;
				m1.resize(flatten.rows, flatten.cols);
				m1 <<
					1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
					2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
					3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15,
					4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16;
				// ÇPâÒñ⁄ÇÃprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, grid, kDensityRectangular, kColMajor, kColMajor);
					storage.writeGrid(name, m1);
				}
				// ÉOÉäÉbÉhçÌèú
				{
					monju::GridMatrixStorage storage(file);
					storage.clearGrid(name);
				}
			}

			TEST_METHOD(MatrixReadWrite)
			{
				std::string file = "C:\\monju\\test\\GridMatrixStorage\\OpenAndClose.dbm";
				boost::filesystem::remove(file);
				Extent extentMatrix(2, 3);
				std::string name = "matrix";
				monju::MatrixRm<float> m1, m2;
				m1.resize(extentMatrix.rows, extentMatrix.cols);
				m1 << 1, 2, 3, 4, 5, 6;
				m2.resizeLike(m1);
				// ÇPâÒñ⁄ÇÃprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, extentMatrix, kRowMajor);
					storage.readMatrix(name, m2);
					storage.writeMatrix(name, m1);
				}
				// ÇQâÒñ⁄ÇÃprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, extentMatrix, kRowMajor);
					storage.readMatrix(name, m2);
				}
				float prec = 0.001f;
				Assert::IsTrue(m1.isApprox(m2, prec));
				// ÉOÉäÉbÉhçÌèú
				{

				}
			}

			TEST_METHOD(GridReadWrite)
			{
				std::string file = "C:\\monju\\test\\GridMatrixStorage\\GridReadWrite.dbm";
				boost::filesystem::remove(file);
				GridExtent grid(2, 2, 3, 4);
				Extent flatten = grid.flattenCm();
				std::string name = "grid";
				monju::MatrixCm<float> m1, m2;
				m1.resize(flatten.rows, flatten.cols);
				m1 <<
					1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
					2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
					3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15,
					4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16;
				m2.resizeLike(m1);
				// ÇPâÒñ⁄ÇÃprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, grid, kDensityRectangular, kColMajor, kColMajor);
					storage.readGrid(name, m2);
					storage.writeGrid(name, m1);
				}
				// ÇQâÒñ⁄ÇÃprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, grid, kDensityRectangular, kColMajor, kColMajor);
					storage.readGrid(name, m2);
				}
				float prec = 0.001f;
				Assert::IsTrue(m1.isApprox(m2, prec));
			}

			TEST_METHOD(CoeffReadWrite)
			{
				std::string file = "C:\\monju\\test\\GridMatrixStorage\\CoeffReadWrite.dbm";
				boost::filesystem::remove(file);
				GridExtent grid(2, 2, 2, 2);
				Extent flatten = grid.flattenCm();
				std::string name = "grid";
				monju::MatrixCm<int32_t> m1, m2;
				m1.resize(flatten.rows, flatten.cols);
				m1 <<
					1, 0,
					0, 0,
					0, 0,
					0, 0,
					0, 0,
					1, 0,
					0, 0,
					0, 1;

				m2.resizeLike(m1);
				auto f = [](int32_t v) { return v + 1; };
				// óvëfíPà Ç≈ë´Ç∑
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, grid, kDensityRectangular, kColMajor, kColMajor);
					GridEntry entry;
					storage.coeffOp<int32_t>(name, entry, f);
					entry.grid.row++;
					entry.matrix.row++;
					storage.coeffOp<int32_t>(name, entry, f);
					entry.grid.col++;
					entry.matrix.col++;
					storage.coeffOp<int32_t>(name, entry, f);

					storage.readGrid(name, m2);

					Assert::AreEqual(storage.readCoeff<int32_t>(name, 0, 0, 0, 0), 1);
					Assert::AreEqual(storage.readCoeff<int32_t>(name, 1, 0, 1, 0), 1);
					Assert::AreEqual(storage.readCoeff<int32_t>(name, 1, 1, 1, 1), 1);
				}
				int32_t prec = 0;
				Assert::IsTrue(m1.isApprox(m2, prec));
			}
		};
	}
}