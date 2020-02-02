#pragma once
#ifndef _MONJU_BAYESIAN_EDGE_DEVICE_H__
#define _MONJU_BAYESIAN_EDGE_DEVICE_H__

#include "BayesianEdge.h"
#include "Device.h"
#include "DeviceMemory.h"
#include "GridCpt.h"

namespace monju {

	class BayesianEdgeDevice
	{
	private:
		Device* _pDevice;
		BayesianEdge* _pEdge;
		GridCpt* _pCpt;
		std::unique_ptr<DeviceMemory> _pMem;

	public:	// コピー禁止・ムーブ禁止
		BayesianEdgeDevice(const BayesianEdgeDevice&) = delete;
		BayesianEdgeDevice(BayesianEdgeDevice&&) = delete;
		BayesianEdgeDevice& operator=(const BayesianEdgeDevice&) = delete;
		BayesianEdgeDevice& operator=(BayesianEdgeDevice&&) = delete;

	public:
		BayesianEdgeDevice(Device& device, BayesianEdge& edge, GridCpt& cpt)
		{
			_pDevice = &device;
			_pEdge = &edge;
			_pCpt = &cpt;
			_pMem = std::make_unique<DeviceMemory>(device);

			_pMem->addMemory(VariableKind::lambda, _pEdge->lambda());
			_pMem->addMemory(VariableKind::kappa, _pEdge->kappa());
			_pMem->addMemory(VariableKind::W, _pCpt->cpt());
		}
		~BayesianEdgeDevice()
		{

		}

		Device& device() const { return *_pDevice; }
		DeviceMemory& mem() const { return *_pMem; }
		//std::weak_ptr<DeviceMemory> ref() const { return _pMem; }
	};
}

#endif // !_MONJU_BAYESIAN_EDGE_DEVICE_H__
