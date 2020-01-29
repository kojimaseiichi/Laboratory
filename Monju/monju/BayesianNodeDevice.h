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
		BayesianNode* _pEdge;
		std::shared_ptr<DeviceMemory> _mem;

	public:
		BayesianNodeDevice(Device& device, BayesianNode& edge)
		{
			_pDevice = &device;
			_pEdge = &edge;

			_mem = std::make_shared<DeviceMemory>(device);

			_mem->addMemory(VariableKind::lambda, _pEdge->lambda());
			_mem->addMemory(VariableKind::pi, _pEdge->pi());
			_mem->addMemory(VariableKind::rho, _pEdge->rho());
			_mem->addMemory(VariableKind::R, _pEdge->r());
			_mem->addMemory(VariableKind::BEL, _pEdge->bel());
		}

		Device& device() const { return *_pDevice; }
		DeviceMemory& mem() const { return *_mem; }
		std::weak_ptr<DeviceMemory> ref() const { return _mem; }


	};
}

#endif // !_BAYESIAN_NODE_DEVICE_H__

