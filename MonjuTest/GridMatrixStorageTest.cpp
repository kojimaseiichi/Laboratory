#include "pch.h"
#include "CppUnitTest.h"
#include "monju/GridMatrixStorage.h"
#include <boost/filesystem.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace monju
{
	namespace test
	{
		TEST_CLASS(GridMatrixStorage)
		{

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
				// ‚P‰ñ–Ú‚Ìprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, extentMatrix, kRowMajor);
					storage.readMatrix(name, m2);
					storage.writeMatrix(name, m1);
				}
				// ‚Q‰ñ–Ú‚Ìprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, extentMatrix, kRowMajor);
					storage.readMatrix(name, m2);
				}
				float prec = 0.001f;
				Assert::IsTrue(m1.isApprox(m2, prec));
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
				// ‚P‰ñ–Ú‚Ìprepare
				{
					monju::GridMatrixStorage storage(file);
					storage.prepare<int32_t>(name, grid, kDensityRectangular, kColMajor, kColMajor);
					storage.readGrid(name, m2);
					storage.writeGrid(name, m1);
				}
				// ‚Q‰ñ–Ú‚Ìprepare
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
				// —v‘f’PˆÊ‚Å‘«‚·
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