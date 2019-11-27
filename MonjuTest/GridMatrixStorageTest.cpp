#include "pch.h"
#include "CppUnitTest.h"
#include "monju/GridMatrixStorage.h"
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MonjuTest
{
	TEST_CLASS(RectangularGridMatrixStorageTest)
	{
	private:

	public:

		
		TEST_METHOD(CreateAndCloseTest_1_1_1_1)
		{
			std::string file_name = R"(C:\dev\test\RectangularGridMatrixStorage_1_1_1_1)";
			boost::filesystem::remove(file_name);
			{

				monju::RectangularGridMatrixStorage<float> storage;
				storage.create(file_name, 1, 1, 1, 1);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 0.f, L"初期値");
				storage.writeElement(0, 0, 0, 0, 1.f);
				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 1.0f, L"保存");
				storage.addElement(0, 0, 0, 0, 100.f);
				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 101.0f, L"加算");
				storage.close();
			}
			// 行優先でアクセス
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				monju::MatrixRm<float> m(1, 1);
				monju::GridMatrixRowMajorAccessor<float> ar(m);
				storage.load(ar);
				Assert::AreEqual(m(0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み");
				m(0, 0) = 2.f;
				storage.store(ar);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 2.f, L"GridMatrixRowMajorAccessorの書き込み");

				storage.close();
			}
			// 列優先でアクセス
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				monju::MatrixCm<float> mc(1, 1);
				monju::GridMatrixColMajorAccessor<float> ac(mc);
				storage.load(ac);
				Assert::AreEqual(mc(0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixColMajorAccessorの読み込み");
				mc(0, 0) = 2.f;
				storage.store(ac);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 2.f, L"GridMatrixColMajorAccessorの書き込み");

				storage.close();
			}
		}

		TEST_METHOD(CreateAndCloseTest_1_1_2_2)
		{
			std::string file_name = R"(C:\dev\test\RectangularGridMatrixStorage_1_1_2_2)";
			boost::filesystem::remove(file_name);
			{

				monju::RectangularGridMatrixStorage<float> storage;
				storage.create(file_name, 1, 1, 2, 2);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 0.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 0.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 0.f);
				storage.writeElement(0, 0, 0, 0, 1.f);
				storage.writeElement(0, 0, 0, 1, 2.f);
				storage.writeElement(0, 0, 1, 0, 3.f);
				storage.writeElement(0, 0, 1, 1, 4.f);
				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 1.f);
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 2.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 3.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 4.f);
				storage.addElement(0, 0, 0, 0, 100.f);
				storage.addElement(0, 0, 0, 1, 100.f);
				storage.addElement(0, 0, 1, 0, 100.f);
				storage.addElement(0, 0, 1, 1, 100.f);
				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 101.f);
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 102.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 103.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 104.f);
				storage.close();
			}
			// 行優先でアクセス（優先方向がstorageと一致）
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				monju::MatrixRm<float> mr(1, 4);
				monju::GridMatrixRowMajorAccessor<float> ar(1, 1, 2, 2, mr);
				storage.load(ar);
				Assert::AreEqual(mr(0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み");
				Assert::AreEqual(mr(0, 1), storage.readElement(0, 0, 0, 1), L"GridMatrixRowMajorAccessorの読み込み");
				Assert::AreEqual(mr(0, 2), storage.readElement(0, 0, 1, 0), L"GridMatrixRowMajorAccessorの読み込み");
				Assert::AreEqual(mr(0, 3), storage.readElement(0, 0, 1, 1), L"GridMatrixRowMajorAccessorの読み込み");

				mr(0, 0) = 2.f;
				mr(0, 1) = 3.f;
				mr(0, 2) = 4.f;
				mr(0, 3) = 5.f;

				storage.store(ar);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 2.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 3.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 4.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 5.f, L"GridMatrixRowMajorAccessorの書き込み");

				storage.close();
			}
			// 列優先でアクセス
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				monju::MatrixCm<float> mc(4, 1);
				monju::GridMatrixColMajorAccessor<float> ac(1, 1, 2, 2, mc);
				storage.load(ac);
				Assert::AreEqual(mc(0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixColMajorAccessorの読み込み");
				Assert::AreEqual(mc(1, 0), storage.readElement(0, 0, 1, 0), L"GridMatrixColMajorAccessorの読み込み");
				Assert::AreEqual(mc(2, 0), storage.readElement(0, 0, 0, 1), L"GridMatrixColMajorAccessorの読み込み");
				Assert::AreEqual(mc(3, 0), storage.readElement(0, 0, 1, 1), L"GridMatrixColMajorAccessorの読み込み");

				mc(0, 0) = 2.f;
				mc(1, 0) = 3.f;
				mc(2, 0) = 4.f;
				mc(3, 0) = 5.f;

				storage.store(ac);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 2.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 3.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 4.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 5.f, L"GridMatrixColMajorAccessorの書き込み");

				storage.close();
			}
		}

		TEST_METHOD(CreateAndCloseTest_2_2_1_1)
		{
			std::string file_name = R"(C:\dev\test\RectangularGridMatrixStorage_2_2_1_1)";
			boost::filesystem::remove(file_name);
			{

				monju::RectangularGridMatrixStorage<float> storage;
				storage.create(file_name, 2, 2, 1, 1);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 0.f);
				storage.writeElement(0, 0, 0, 0, 1.f);
				storage.writeElement(0, 1, 0, 0, 2.f);
				storage.writeElement(1, 0, 0, 0, 3.f);
				storage.writeElement(1, 1, 0, 0, 4.f);
				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 1.0f);
				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 2.0f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 3.0f);
				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 4.0f);
				storage.addElement(0, 0, 0, 0, 100.f);
				storage.addElement(0, 1, 0, 0, 100.f);
				storage.addElement(1, 0, 0, 0, 100.f);
				storage.addElement(1, 1, 0, 0, 100.f);
				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 101.0f);
				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 102.0f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 103.0f);
				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 104.0f);
				storage.close();
			}
			// 行優先でアクセス
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				monju::MatrixRm<float> mr(2, 2);
				monju::GridMatrixRowMajorAccessor<float> ar(2, 2, 1, 1, mr);
				storage.load(ar);
				Assert::AreEqual(mr(0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み1");
				Assert::AreEqual(mr(0, 1), storage.readElement(0, 1, 0, 0), L"GridMatrixRowMajorAccessorの読み込み2");
				Assert::AreEqual(mr(1, 0), storage.readElement(1, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み3");
				Assert::AreEqual(mr(1, 1), storage.readElement(1, 1, 0, 0), L"GridMatrixRowMajorAccessorの読み込み4");

				mr(0, 0) = 2.f;
				mr(0, 1) = 3.f;
				mr(1, 0) = 4.f;
				mr(1, 1) = 5.f;

				storage.store(ar);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 2.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 3.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 4.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 5.f, L"GridMatrixRowMajorAccessorの書き込み");

				storage.close();
			}
			// 列優先でアクセス
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				monju::MatrixCm<float> mc(2, 2);
				monju::GridMatrixColMajorAccessor<float> ac(2, 2, 1, 1, mc);
				storage.load(ac);
				Assert::AreEqual(mc(0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixColMajorAccessorの読み込み1");
				Assert::AreEqual(mc(0, 1), storage.readElement(0, 1, 0, 0), L"GridMatrixColMajorAccessorの読み込み2");
				Assert::AreEqual(mc(1, 0), storage.readElement(1, 0, 0, 0), L"GridMatrixColMajorAccessorの読み込み3");
				Assert::AreEqual(mc(1, 1), storage.readElement(1, 1, 0, 0), L"GridMatrixColMajorAccessorの読み込み4");

				mc(0, 0) = 12.f;
				mc(0, 1) = 13.f;
				mc(1, 0) = 14.f;
				mc(1, 1) = 15.f;

				storage.store(ac);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 12.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 13.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 14.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 15.f, L"GridMatrixRowMajorAccessorの書き込み");

				storage.close();
			}
		}

		TEST_METHOD(CreateAndCloseTest_2_2_2_2)
		{
			std::string file_name = R"(C:\dev\test\RectangularGridMatrixStorage_2_2_2_2)";
			boost::filesystem::remove(file_name);
			{

				monju::RectangularGridMatrixStorage<float> storage;
				storage.create(file_name, 2, 2, 2, 2);
				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 0.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 0.f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 0.f);

				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(0, 1, 0, 1), 0.f);
				Assert::AreEqual(storage.readElement(0, 1, 1, 0), 0.f);
				Assert::AreEqual(storage.readElement(0, 1, 1, 1), 0.f);

				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 0.f);

				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 0.f);
				Assert::AreEqual(storage.readElement(1, 1, 0, 1), 0.f);
				Assert::AreEqual(storage.readElement(1, 1, 1, 0), 0.f);
				Assert::AreEqual(storage.readElement(1, 1, 1, 1), 0.f);

				storage.writeElement(0, 0, 0, 0, 11.f);
				storage.writeElement(0, 0, 0, 1, 12.f);
				storage.writeElement(0, 0, 1, 0, 13.f);
				storage.writeElement(0, 0, 1, 1, 14.f);

				storage.writeElement(0, 1, 0, 0, 21.f);
				storage.writeElement(0, 1, 0, 1, 22.f);
				storage.writeElement(0, 1, 1, 0, 23.f);
				storage.writeElement(0, 1, 1, 1, 24.f);

				storage.writeElement(1, 0, 0, 0, 31.f);
				storage.writeElement(1, 0, 0, 1, 32.f);
				storage.writeElement(1, 0, 1, 0, 33.f);
				storage.writeElement(1, 0, 1, 1, 34.f);

				storage.writeElement(1, 1, 0, 0, 41.f);
				storage.writeElement(1, 1, 0, 1, 42.f);
				storage.writeElement(1, 1, 1, 0, 43.f);
				storage.writeElement(1, 1, 1, 1, 44.f);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 11.0f);
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 12.0f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 13.0f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 14.0f);

				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 21.0f);
				Assert::AreEqual(storage.readElement(0, 1, 0, 1), 22.0f);
				Assert::AreEqual(storage.readElement(0, 1, 1, 0), 23.0f);
				Assert::AreEqual(storage.readElement(0, 1, 1, 1), 24.0f);

				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 31.0f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 1), 32.0f);
				Assert::AreEqual(storage.readElement(1, 0, 1, 0), 33.0f);
				Assert::AreEqual(storage.readElement(1, 0, 1, 1), 34.0f);

				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 41.0f);
				Assert::AreEqual(storage.readElement(1, 1, 0, 1), 42.0f);
				Assert::AreEqual(storage.readElement(1, 1, 1, 0), 43.0f);
				Assert::AreEqual(storage.readElement(1, 1, 1, 1), 44.0f);

				storage.addElement(0, 0, 0, 0, 100.f);
				storage.addElement(0, 0, 0, 1, 100.f);
				storage.addElement(0, 0, 1, 0, 100.f);
				storage.addElement(0, 0, 1, 1, 100.f);

				storage.addElement(0, 1, 0, 0, 100.f);
				storage.addElement(0, 1, 0, 1, 100.f);
				storage.addElement(0, 1, 1, 0, 100.f);
				storage.addElement(0, 1, 1, 1, 100.f);

				storage.addElement(1, 0, 0, 0, 100.f);
				storage.addElement(1, 0, 0, 1, 100.f);
				storage.addElement(1, 0, 1, 0, 100.f);
				storage.addElement(1, 0, 1, 1, 100.f);

				storage.addElement(1, 1, 0, 0, 100.f);
				storage.addElement(1, 1, 0, 1, 100.f);
				storage.addElement(1, 1, 1, 0, 100.f);
				storage.addElement(1, 1, 1, 1, 100.f);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 111.0f);
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 112.0f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 113.0f);
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 114.0f);

				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 121.0f);
				Assert::AreEqual(storage.readElement(0, 1, 0, 1), 122.0f);
				Assert::AreEqual(storage.readElement(0, 1, 1, 0), 123.0f);
				Assert::AreEqual(storage.readElement(0, 1, 1, 1), 124.0f);

				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 131.0f);
				Assert::AreEqual(storage.readElement(1, 0, 0, 1), 132.0f);
				Assert::AreEqual(storage.readElement(1, 0, 1, 0), 133.0f);
				Assert::AreEqual(storage.readElement(1, 0, 1, 1), 134.0f);

				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 141.0f);
				Assert::AreEqual(storage.readElement(1, 1, 0, 1), 142.0f);
				Assert::AreEqual(storage.readElement(1, 1, 1, 0), 143.0f);
				Assert::AreEqual(storage.readElement(1, 1, 1, 1), 144.0f);

				storage.close();
			}
			// 行優先でアクセス
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				monju::MatrixRm<float> mr(2, 2 * (2 * 2));
				monju::GridMatrixRowMajorAccessor<float> ar(2, 2, 2, 2, mr);
				storage.load(ar);
				Assert::AreEqual(mr(0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み1");
				Assert::AreEqual(mr(0, 0+1), storage.readElement(0, 0, 0, 1), L"GridMatrixRowMajorAccessorの読み込み2");
				Assert::AreEqual(mr(0, 0+2), storage.readElement(0, 0, 1, 0), L"GridMatrixRowMajorAccessorの読み込み3");
				Assert::AreEqual(mr(0, 0+3), storage.readElement(0, 0, 1, 1), L"GridMatrixRowMajorAccessorの読み込み4");

				Assert::AreEqual(mr(0, 4), storage.readElement(0, 1, 0, 0), L"GridMatrixRowMajorAccessorの読み込み5");
				Assert::AreEqual(mr(0, 4+1), storage.readElement(0, 1, 0, 1), L"GridMatrixRowMajorAccessorの読み込み6");
				Assert::AreEqual(mr(0, 4+2), storage.readElement(0, 1, 1, 0), L"GridMatrixRowMajorAccessorの読み込み7");
				Assert::AreEqual(mr(0, 4+3), storage.readElement(0, 1, 1, 1), L"GridMatrixRowMajorAccessorの読み込み8");

				Assert::AreEqual(mr(1, 0), storage.readElement(1, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み9");
				Assert::AreEqual(mr(1, 0+1), storage.readElement(1, 0, 0, 1), L"GridMatrixRowMajorAccessorの読み込み10");
				Assert::AreEqual(mr(1, 0+2), storage.readElement(1, 0, 1, 0), L"GridMatrixRowMajorAccessorの読み込み11");
				Assert::AreEqual(mr(1, 0+3), storage.readElement(1, 0, 1, 1), L"GridMatrixRowMajorAccessorの読み込み12");

				Assert::AreEqual(mr(1, 4), storage.readElement(1, 1, 0, 0), L"GridMatrixRowMajorAccessorの読み込み13");
				Assert::AreEqual(mr(1, 4+1), storage.readElement(1, 1, 0, 1), L"GridMatrixRowMajorAccessorの読み込み14");
				Assert::AreEqual(mr(1, 4+2), storage.readElement(1, 1, 1, 0), L"GridMatrixRowMajorAccessorの読み込み15");
				Assert::AreEqual(mr(1, 4+3), storage.readElement(1, 1, 1, 1), L"GridMatrixRowMajorAccessorの読み込み16");

				mr(0, 0) = 10.f;
				mr(0, 0+1) = 11.f;
				mr(0, 0+2) = 12.f;
				mr(0, 0+3) = 13.f;

				mr(0, 4) = 20.f;
				mr(0, 4+1) = 21.f;
				mr(0, 4+2) = 22.f;
				mr(0, 4+3) = 23.f;

				mr(1, 0) = 30.f;
				mr(1, 0+1) = 31.f;
				mr(1, 0+2) = 32.f;
				mr(1, 0+3) = 33.f;

				mr(1, 4) = 40.f;
				mr(1, 4+1) = 41.f;
				mr(1, 4+2) = 42.f;
				mr(1, 4+3) = 43.f;

				storage.store(ar);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 10.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 11.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 12.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 13.f, L"GridMatrixColMajorAccessorの書き込み");

				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 20.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 0, 1), 21.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 1, 0), 22.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 1, 1), 23.f, L"GridMatrixColMajorAccessorの書き込み");

				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 30.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 0, 1), 31.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 1, 0), 32.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 1, 1), 33.f, L"GridMatrixColMajorAccessorの書き込み");

				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 40.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 0, 1), 41.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 1, 0), 42.f, L"GridMatrixColMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 1, 1), 43.f, L"GridMatrixColMajorAccessorの書き込み");

				storage.close();
			}
			// 列優先でアクセス
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);
				monju::MatrixCm<float> mc(2 * (2 * 2), 2);
				monju::GridMatrixColMajorAccessor<float> ac(2, 2, 2, 2, mc);
				storage.load(ac);
				Assert::AreEqual(mc(0 + 0, 0), storage.readElement(0, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み1");
				Assert::AreEqual(mc(0 + 2, 0), storage.readElement(0, 0, 0, 1), L"GridMatrixRowMajorAccessorの読み込み2");
				Assert::AreEqual(mc(0 + 1, 0), storage.readElement(0, 0, 1, 0), L"GridMatrixRowMajorAccessorの読み込み3");
				Assert::AreEqual(mc(0 + 3, 0), storage.readElement(0, 0, 1, 1), L"GridMatrixRowMajorAccessorの読み込み4");

				Assert::AreEqual(mc(0 + 0, 1), storage.readElement(0, 1, 0, 0), L"GridMatrixRowMajorAccessorの読み込み5");
				Assert::AreEqual(mc(0 + 2, 1), storage.readElement(0, 1, 0, 1), L"GridMatrixRowMajorAccessorの読み込み6");
				Assert::AreEqual(mc(0 + 1, 1), storage.readElement(0, 1, 1, 0), L"GridMatrixRowMajorAccessorの読み込み7");
				Assert::AreEqual(mc(0 + 3, 1), storage.readElement(0, 1, 1, 1), L"GridMatrixRowMajorAccessorの読み込み8");

				Assert::AreEqual(mc(4 + 0, 0), storage.readElement(1, 0, 0, 0), L"GridMatrixRowMajorAccessorの読み込み9");
				Assert::AreEqual(mc(4 + 2, 0), storage.readElement(1, 0, 0, 1), L"GridMatrixRowMajorAccessorの読み込み10");
				Assert::AreEqual(mc(4 + 1, 0), storage.readElement(1, 0, 1, 0), L"GridMatrixRowMajorAccessorの読み込み11");
				Assert::AreEqual(mc(4 + 3, 0), storage.readElement(1, 0, 1, 1), L"GridMatrixRowMajorAccessorの読み込み12");

				Assert::AreEqual(mc(4 + 0, 1), storage.readElement(1, 1, 0, 0), L"GridMatrixRowMajorAccessorの読み込み13");
				Assert::AreEqual(mc(4 + 2, 1), storage.readElement(1, 1, 0, 1), L"GridMatrixRowMajorAccessorの読み込み14");
				Assert::AreEqual(mc(4 + 1, 1), storage.readElement(1, 1, 1, 0), L"GridMatrixRowMajorAccessorの読み込み15");
				Assert::AreEqual(mc(4 + 3, 1), storage.readElement(1, 1, 1, 1), L"GridMatrixRowMajorAccessorの読み込み16");

				mc(0, 0) = 20.f;
				mc(1, 0) = 21.f;
				mc(2, 0) = 22.f;
				mc(3, 0) = 23.f;

				mc(0, 1) = 30.f;
				mc(1, 1) = 31.f;
				mc(2, 1) = 32.f;
				mc(3, 1) = 33.f;

				mc(4, 0) = 40.f;
				mc(5, 0) = 41.f;
				mc(6, 0) = 42.f;
				mc(7, 0) = 43.f;

				mc(4, 1) = 50.f;
				mc(5, 1) = 51.f;
				mc(6, 1) = 52.f;
				mc(7, 1) = 53.f;

				storage.store(ac);

				storage.close();
			}
			{
				monju::RectangularGridMatrixStorage<float> storage;
				storage.open(file_name);

				Assert::AreEqual(storage.readElement(0, 0, 0, 0), 20.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 0, 1), 22.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 0), 21.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 0, 1, 1), 23.f, L"GridMatrixRowMajorAccessorの書き込み");

				Assert::AreEqual(storage.readElement(0, 1, 0, 0), 30.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 0, 1), 32.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 1, 0), 31.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(0, 1, 1, 1), 33.f, L"GridMatrixRowMajorAccessorの書き込み");

				Assert::AreEqual(storage.readElement(1, 0, 0, 0), 40.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 0, 1), 42.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 1, 0), 41.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 0, 1, 1), 43.f, L"GridMatrixRowMajorAccessorの書き込み");

				Assert::AreEqual(storage.readElement(1, 1, 0, 0), 50.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 0, 1), 52.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 1, 0), 51.f, L"GridMatrixRowMajorAccessorの書き込み");
				Assert::AreEqual(storage.readElement(1, 1, 1, 1), 53.f, L"GridMatrixRowMajorAccessorの書き込み");

				storage.close();
			}
		}

	};
}
