
/*
#include "pch.h"
#include "CppUnitTest.h"
#include "monju/DeviceMemory.h"
#include "monju/PlatformContext.h"
#include "monju/DeviceContext.h"
#include "monju/DeviceKernel.h"
#include "monju/DeviceMemory.h"
#include "monju/DeviceKernelArguments.h"
#include "monju/util_math.h"
#include <map>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace MonjuTest
{
	TEST_CLASS(OpenCLOobpTest)
	{
		TEST_METHOD(OobpUpTest)
		{
			// 固定のパラメータ
			const int X = 2, Y = 2, XU = 2, YU = 2;
			std::string source_file1 = R"(C:\monju\kernels\oobp\oobp3_full_up_1.cl)";
			std::string kernel_name1 = "oobp3_full_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}";
			std::string source_file2 = R"(C:\monju\kernels\oobp\oobp3_full_up_2.cl)";
			std::string kernel_name2 = "oobp3_full_up_2_X${X}_Y${Y}_XU${XU}_YU${YU}";
			std::string source_file_bel = R"(C:\monju\kernels\oobp\oobp3_bel.cl)";
			std::string kernel_name_bel = "oobp3_bel_X${X}_XU${XU}";

			// プラットフォームを作成してデバイスを取得
			monju::PlatformContext platformContext;
			platformContext.open(R"(C:\dev\test)");
			monju::Device& device = platformContext.deviceContext().getDevice(0);

			// Y基底データ
			monju::MatrixRm<float> x_lambda, x_pi, x_rho, x_r, x_bel,
				y_lambda, y_pi, y_rho, y_r, y_bel;
			monju::MatrixCm<float> w_lambda, w_kappa, w_cpt;

			x_lambda.resize(X, XU);
			x_pi.resize(X, XU);
			x_rho.resize(X, XU);
			x_r.resize(X, XU);
			x_bel.resize(X, 1);

			y_lambda.resize(Y, YU);
			y_pi.resize(Y, YU);
			y_rho.resize(Y, YU);
			y_r.resize(Y, YU);
			y_bel.resize(Y, 1);

			w_lambda.resize(Y * XU, X);
			w_kappa.resize(Y * YU, X);
			w_cpt.resize(Y * XU * YU, X);

			y_lambda.setConstant(0.5f);
			y_pi.setConstant(0.5f);
			w_kappa.setConstant(0.5f);
			w_cpt.setConstant(0.5f);
			w_lambda.setConstant(0.0f);
			x_lambda.setConstant(0.5f);
			x_pi.setConstant(0.5f);
			x_r.setConstant(1.f);

			w_cpt(0, 0) = 0.2f;

			// OpenCLプログラムのテンプレートパラメータ
			std::map<std::string, std::string> params_map;
			params_map["X"] = boost::lexical_cast<std::string>(X);
			params_map["Y"] = boost::lexical_cast<std::string>(Y);
			params_map["XU"] = boost::lexical_cast<std::string>(XU);
			params_map["YU"] = boost::lexical_cast<std::string>(YU);

			// プログラム初期化
			monju::DeviceProgram program1;
			program1.create(platformContext.deviceContext(), platformContext.deviceContext().getAllDevices(), source_file1, params_map);

			monju::DeviceProgram program2;
			program2.create(platformContext.deviceContext(), platformContext.deviceContext().getAllDevices(), source_file2, params_map);

			monju::DeviceProgram programBel;
			programBel.create(platformContext.deviceContext(), platformContext.deviceContext().getAllDevices(), source_file_bel, params_map);

			// カーネル初期化
			monju::DeviceKernel kernel1;
			kernel1.create(program1, kernel_name1, params_map);

			monju::DeviceKernel kernel2;
			kernel2.create(program2, kernel_name2, params_map);

			monju::DeviceKernel kernelBel;
			kernelBel.create(programBel, kernel_name_bel, params_map);

			// カーネル実行（ワークアイテム設定）
			std::vector<size_t> global_work_size1;
			global_work_size1.push_back(Y);
			global_work_size1.push_back(X);
			std::vector<size_t> local_work_size1;
			local_work_size1.push_back(1);
			local_work_size1.push_back(X);

			std::vector<size_t> global_work_size2;
			global_work_size2.push_back(1);
			global_work_size2.push_back(X);

			std::vector<size_t> global_work_size_bel;
			global_work_size_bel.push_back(X);

			// カーネルメモリ設定
			monju::DeviceMemory memx(device);
			memx.addMemory(monju::VariableKind::lambda, x_lambda);
			memx.addMemory(monju::VariableKind::pi, x_pi);
			memx.addMemory(monju::VariableKind::rho, x_rho);
			memx.addMemory(monju::VariableKind::R, x_r);
			memx.addMemory(monju::VariableKind::BEL, x_bel);

			monju::DeviceMemory memy(device);
			memy.addMemory(monju::VariableKind::lambda, y_lambda);
			memy.addMemory(monju::VariableKind::pi, y_pi);
			memy.addMemory(monju::VariableKind::rho, y_rho);
			memy.addMemory(monju::VariableKind::R, y_r);
			memy.addMemory(monju::VariableKind::BEL, y_bel);

			monju::DeviceMemory memw(device);
			memw.addMemory(monju::VariableKind::lambda, w_lambda);
			memw.addMemory(monju::VariableKind::kappa, w_kappa);
			memw.addMemory(monju::VariableKind::W, w_cpt);

			// カーネル引数設定
			monju::DeviceKernelArguments args1;
			args1.push(memy, monju::VariableKind::lambda, false);
			args1.push(memy, monju::VariableKind::pi, false);
			args1.push(memw, monju::VariableKind::W, false);
			args1.push(memw, monju::VariableKind::kappa, false);
			args1.push(memw, monju::VariableKind::lambda, true);
			args1.stackArguments(kernel1);

			monju::DeviceKernelArguments args2;
			args2.push(memw, monju::VariableKind::lambda, false);
			args2.push(memx, monju::VariableKind::R, false);
			args2.push(memx, monju::VariableKind::lambda, true);
			args2.stackArguments(kernel2);

			monju::DeviceKernelArguments args_bel;
			args_bel.push(memx, monju::VariableKind::lambda, false);
			args_bel.push(memx, monju::VariableKind::pi, false);
			args_bel.push(memx, monju::VariableKind::rho, true);
			args_bel.push(memx, monju::VariableKind::BEL, true);
			args_bel.stackArguments(kernelBel);


			// カーネル実行
			kernel1.compute(device, global_work_size1, local_work_size1);
			memw.requireRead(args1.outputParams());

			kernel2.compute(device, global_work_size2);
			memx.requireRead(args2.outputParams());

			kernelBel.compute(device, global_work_size_bel);
			memx.requireRead(args_bel.outputParams());

			// カーネルの実行結果を読み込み
			memw.flushRead();
			memx.flushRead();

			Assert::AreEqual(monju::util_math::round_n(0.411764706f, 6), monju::util_math::round_n(w_lambda(0, 0), 6));
			Assert::AreEqual(monju::util_math::round_n(0.588235294f, 6), monju::util_math::round_n(w_lambda(1, 0), 6));
			Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(2, 0), 6));
			Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(3, 0), 6));
			Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(0, 1), 6));
			Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(1, 1), 6));
			Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(2, 1), 6));
			Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(3, 1), 6));

			Assert::AreEqual(monju::util_math::round_n(0.205882353f, 6), monju::util_math::round_n(x_lambda(0, 0), 6));
			Assert::AreEqual(monju::util_math::round_n(0.294117647f, 6), monju::util_math::round_n(x_lambda(0, 1), 6));
			Assert::AreEqual(monju::util_math::round_n(0.25f, 2), monju::util_math::round_n(x_lambda(1, 0), 2));
			Assert::AreEqual(monju::util_math::round_n(0.25f, 2), monju::util_math::round_n(x_lambda(1, 1), 2));
		}

		TEST_METHOD(OobpDownTest)
		{
			// 固定のパラメータ
			const int X = 2, Y = 2, XU = 2, YU = 2;
			std::string source_file1 = R"(C:\monju\kernels\oobp\oobp3_full_down_1.cl)";
			std::string kernel_name1 = "oobp3_full_down_1_X${X}_Y${Y}_XU${XU}_YU${YU}";
			std::string source_file2 = R"(C:\monju\kernels\oobp\oobp3_full_down_2.cl)";
			std::string kernel_name2 = "oobp3_full_down_2_X${X}_Y${Y}_XU${XU}_YU${YU}";
			// プラットフォームを作成してデバイスを取得
			monju::PlatformContext platformContext;
			platformContext.open(R"(C:\dev\test)");
			monju::Device& device = platformContext.deviceContext().getDevice(0);

			// 基底データ
			monju::MatrixRm<float> x_lambda, x_pi, x_rho,
				y_lambda, y_pi, y_rho;
			// 基底間データ
			monju::MatrixCm<float> w_lambda, w_kappa, w_cpt;

			// 代数的形状設定
			x_lambda.resize(X, XU);
			x_pi.resize(X, XU);
			x_rho.resize(X, XU);
			y_lambda.resize(Y, YU);
			y_pi.resize(Y, YU);
			y_rho.resize(Y, YU);

			w_lambda.resize(Y * XU, X);
			w_kappa.resize(Y * YU, X);
			w_cpt.resize(Y * XU * YU, X);

			// 初期値設定
			x_rho.setConstant(0.5f);
			w_cpt.setConstant(0.5f);
			w_lambda.setConstant(0.5f);
			w_kappa.setConstant(0.0f);

			w_cpt(0, 0) = 0.2f;

			// OpenCLプログラムのテンプレートパラメータ
			std::map<std::string, std::string> params_map;
			params_map["X"] = boost::lexical_cast<std::string>(X);
			params_map["Y"] = boost::lexical_cast<std::string>(Y);
			params_map["XU"] = boost::lexical_cast<std::string>(XU);
			params_map["YU"] = boost::lexical_cast<std::string>(YU);

			// プログラム初期化
			monju::DeviceProgram program1;
			program1.create(platformContext.deviceContext(), platformContext.deviceContext().getAllDevices(), source_file1, params_map);

			monju::DeviceProgram program2;
			program2.create(platformContext.deviceContext(), platformContext.deviceContext().getAllDevices(), source_file2, params_map);

			// カーネル初期化
			monju::DeviceKernel kernel1;
			kernel1.create(program1, kernel_name1, params_map);
			monju::DeviceKernel kernel2;
			kernel2.create(program2, kernel_name2, params_map);

			// カーネル実行（ワークアイテム設定）
			std::vector<size_t> global_work_size1;
			global_work_size1.push_back(Y);
			global_work_size1.push_back(X);
			std::vector<size_t> local_work_size1;
			local_work_size1.push_back(Y);
			local_work_size1.push_back(1);

			std::vector<size_t> global_work_size2;
			global_work_size2.push_back(Y);
			global_work_size2.push_back(1);

			// カーネルメモリ設定
			monju::DeviceMemory memx(device);
			memx.addMemory(monju::VariableKind::lambda, x_lambda);
			memx.addMemory(monju::VariableKind::pi, x_pi);
			memx.addMemory(monju::VariableKind::rho, x_rho);

			monju::DeviceMemory memy(device);
			memy.addMemory(monju::VariableKind::lambda, y_lambda);
			memy.addMemory(monju::VariableKind::pi, y_pi);
			memy.addMemory(monju::VariableKind::rho, y_rho);

			monju::DeviceMemory memw(device);
			memw.addMemory(monju::VariableKind::lambda, w_lambda);
			memw.addMemory(monju::VariableKind::kappa, w_kappa);
			memw.addMemory(monju::VariableKind::W, w_cpt);

			// カーネル引数設定
			monju::DeviceKernelArguments args1;
			args1.push(memx, monju::VariableKind::rho, false);
			args1.push(memw, monju::VariableKind::W, false);
			args1.push(memw, monju::VariableKind::lambda, false);
			args1.push(memw, monju::VariableKind::kappa, true);
			args1.stackArguments(kernel1);

			monju::DeviceKernelArguments args2;
			args2.push(memw, monju::VariableKind::kappa, false);
			args2.push(memy, monju::VariableKind::pi, true);
			args2.stackArguments(kernel2);

			// カーネル実行
			kernel1.compute(device, global_work_size1, local_work_size1);
			memw.requireRead(args1.outputParams());

			kernel2.compute(device, global_work_size2);
			memy.requireRead(args2.outputParams());

			// カーネルの実行結果を読み込み
			memw.flushRead();
			memy.flushRead();

			//Assert::AreEqual(monju::util_math::round_n(0.411764706f, 6), monju::util_math::round_n(w_lambda(0, 0), 6));
			//Assert::AreEqual(monju::util_math::round_n(0.588235294f, 6), monju::util_math::round_n(w_lambda(1, 0), 6));
			//Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(2, 0), 6));
			//Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(3, 0), 6));
			//Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(0, 1), 6));
			//Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(1, 1), 6));
			//Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(2, 1), 6));
			//Assert::AreEqual(monju::util_math::round_n(0.5f, 6), monju::util_math::round_n(w_lambda(3, 1), 6));

			Assert::AreEqual(monju::util_math::round_n(0.85f, 6), monju::util_math::round_n(y_pi(0, 0), 6));
			Assert::AreEqual(monju::util_math::round_n(1.f, 6), monju::util_math::round_n(y_pi(0, 1), 6));
			Assert::AreEqual(monju::util_math::round_n(1.f, 6), monju::util_math::round_n(y_pi(1, 0), 6));
			Assert::AreEqual(monju::util_math::round_n(1.f, 6), monju::util_math::round_n(y_pi(1, 1), 6));
		}
	};
}
*/
