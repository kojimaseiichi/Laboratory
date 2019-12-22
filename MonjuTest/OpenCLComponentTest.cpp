#include "pch.h"
#include "CppUnitTest.h"
#include "monju/DeviceMemory.h"

#include "monju/PlatformContext.h"
#include "monju/DeviceContext.h"
#include "monju/DeviceKernel.h"
#include "monju/DeviceMemory.h"
#include "monju/DeviceKernelArguments.h"
#include <map>
#include <iostream>
#include <boost/lexical_cast.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MonjuTest
{
	TEST_CLASS(OpenCLComponentTest)
	{
		TEST_METHOD(GpuDeviceContextTest)
		{
			monju::PlatformContext platformContext;
			platformContext.create(R"(C:\dev\test)", 1);
			monju::DeviceContext& deviceContext = platformContext.deviceContext();
		}

		TEST_METHOD(OpenCLMatMalTest)
		{
			// 固定のパラメータ
			const int M = 10, N = 10, K = 10, TS = 2;
			std::string source_file = R"(C:\monju\kernel\mat_mul.cl)";
			std::string kernel_name = "mat_mul_M${M}_N${N}_K${K}_TS${TS}";
			// プラットフォームを作成してデバイスを取得
			monju::PlatformContext platformContext;
			platformContext.open(R"(C:\dev\test)");
			monju::Device& device = platformContext.deviceContext().getDevice(0);

			// 計算データ c←ab
			monju::MatrixCm<float> a, b, c;
			a.resize(M, N);
			b.resize(N, K);
			c.resize(M, K);

			a.setOnes();
			b.setOnes();
			c.setZero();

			// カーネル初期化
			monju::DeviceKernel kernel;
			monju::DeviceKernel::params_map params_map;
			params_map["M"] = boost::lexical_cast<std::string>(M);
			params_map["N"] = boost::lexical_cast<std::string>(N);
			params_map["K"] = boost::lexical_cast<std::string>(K);
			params_map["TS"] = boost::lexical_cast<std::string>(TS);
			kernel.create(device, source_file, kernel_name, params_map);

			// カーネル実行（ワークアイテム設定）
			std::vector<size_t> global_work_size;
			global_work_size.push_back(M);
			global_work_size.push_back(K);
			std::vector<size_t> local_work_size;
			local_work_size.push_back(TS);
			local_work_size.push_back(TS);

			// カーネルメモリ設定
			monju::DeviceMemory mem(device);
			mem.addMemory(monju::VariableKind::A, a);
			mem.addMemory(monju::VariableKind::B, b);
			mem.addMemory(monju::VariableKind::C, c);

			// カーネル引数設定
			monju::DeviceKernelArguments args(kernel, mem);
			args.push(monju::VariableKind::A);
			args.push(monju::VariableKind::B);
			args.push(monju::VariableKind::C);

			// カーネル実行
			kernel.compute(global_work_size, local_work_size);
			mem.requireRead(monju::VariableKind::C);

			mem.flushRead(); // 問題あり

			Assert::AreEqual(c(0, 0), 10.f);
		}
	};
}
