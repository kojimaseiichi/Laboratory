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
			// �Œ�̃p�����[�^
			const int M = 10, N = 10, K = 10, TS = 2;
			std::string source_file = R"(C:\monju\kernels\test\mat_mul.cl)";
			std::string kernel_name = "mat_mul_M${M}_N${N}_K${K}_TS${TS}";
			// �v���b�g�t�H�[�����쐬���ăf�o�C�X���擾
			monju::PlatformContext platformContext;
			platformContext.open(R"(C:\dev\test)");
			monju::Device& device = platformContext.deviceContext().getDevice(0);

			// �v�Z�f�[�^ c��ab
			monju::MatrixCm<float> a, b, c;
			a.resize(M, N);
			b.resize(N, K);
			c.resize(M, K);

			a.setOnes();
			b.setOnes();
			c.setZero();

			// OpenCL�v���O�����̃e���v���[�g�p�����[�^
			std::map<std::string, std::string> params_map;
			params_map["M"] = boost::lexical_cast<std::string>(M);
			params_map["N"] = boost::lexical_cast<std::string>(N);
			params_map["K"] = boost::lexical_cast<std::string>(K);
			params_map["TS"] = boost::lexical_cast<std::string>(TS);

			// �v���O����������
			monju::DeviceProgram program;
			program.create(platformContext.deviceContext(), platformContext.deviceContext().getAllDevices(), source_file, params_map);

			// �J�[�l��������
			monju::DeviceKernel kernel;
			kernel.create(program, kernel_name, params_map);

			// �J�[�l�����s�i���[�N�A�C�e���ݒ�j
			std::vector<size_t> global_work_size;
			global_work_size.push_back(M);
			global_work_size.push_back(K);
			std::vector<size_t> local_work_size;
			local_work_size.push_back(TS);
			local_work_size.push_back(TS);

			// �J�[�l���������ݒ�
			monju::DeviceMemory mem(device);
			mem.addMemory(monju::VariableKind::A, a);
			mem.addMemory(monju::VariableKind::B, b);
			mem.addMemory(monju::VariableKind::C, c);

			// �J�[�l�������ݒ�
			monju::DeviceKernelArguments args(mem);
			args.push(monju::VariableKind::A, false);
			args.push(monju::VariableKind::B, false);
			args.push(monju::VariableKind::C, true);
			args.stackArguments(kernel);

			// �J�[�l�����s
			kernel.compute(device, global_work_size, local_work_size);
			mem.requireRead(args.outputParams());

			// �J�[�l���̎��s���ʂ�ǂݍ���
			mem.flushRead();

			Assert::AreEqual(c(0, 0), 10.f);
		}
	};
}
