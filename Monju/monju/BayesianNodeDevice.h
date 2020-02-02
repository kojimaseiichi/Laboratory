#pragma once
#ifndef _BAYESIAN_NODE_DEVICE_H__
#define _BAYESIAN_NODE_DEVICE_H__

#include "BayesianNode.h"
#include "Device.h"
#include "DeviceMemory.h"

namespace monju {

	class BayesianNodeDevice
	{
	private:
		Device* _pDevice;
		BayesianNode* _pNode;
		std::unique_ptr<DeviceMemory> _mem;

	public:	// コピー禁止・ムーブ禁止
		BayesianNodeDevice(const BayesianNodeDevice&) = delete;
		BayesianNodeDevice(BayesianNodeDevice&&) = delete;
		BayesianNodeDevice& operator=(const BayesianNodeDevice&) = delete;
		BayesianNodeDevice& operator=(BayesianNodeDevice&&) = delete;

	public:
		BayesianNodeDevice(Device& device, BayesianNode& node)
		{
			_pDevice = &device;
			_pNode = &node;

			_mem = std::make_unique<DeviceMemory>(device);

			_mem->addMemory(VariableKind::lambda, _pNode->lambda());
			_mem->addMemory(VariableKind::pi, _pNode->pi());
			_mem->addMemory(VariableKind::rho, _pNode->rho());
			_mem->addMemory(VariableKind::R, _pNode->r());
			_mem->addMemory(VariableKind::BEL, _pNode->bel());
			_mem->addMemory(VariableKind::WIN, _pNode->win());
		}

		~BayesianNodeDevice()
		{

		}

		Device& device() const { return *_pDevice; }
		DeviceMemory& mem() const { return *_mem; }
		//std::weak_ptr<DeviceMemory> ref() const { return _mem; }


	};
}

#endif // !_BAYESIAN_NODE_DEVICE_H__

