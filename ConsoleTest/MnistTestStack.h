#pragma once

#include <vector>
#include "monju/MonjuTypes.h"
#include "monju/Extent.h"
//#include "monju/ConvLambdaInputFmc.h"

class MnistTestStack
{
private:
	const size_t
		kMnistInputWidth = 28,
		kMnistInputHeight = 28;
	const monju::Extent
		_extentImage = { 28, 28 },
		_extentFilter = { 6, 6 },
		_extentStride = { 2, 2 };
	std::vector<monju::LayerStruct> _network;

public:
	MnistTestStack()
	{
		// 入力層含めて5層のネットワーク
	}
};

