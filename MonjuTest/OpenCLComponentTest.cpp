#include "pch.h"
#include "CppUnitTest.h"
#include "monju/DeviceMemory.h"

#include "monju/DeviceContext.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MonjuTest
{
	TEST_CLASS(OpenCLComponentTest)
	{
		TEST_METHOD(GpuDeviceContextTest)
		{
			monju::DeviceContext c;
			c.create(1);
		}
	};
}
