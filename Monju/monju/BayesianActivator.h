#pragma once
#ifndef _MONJU_BAYESIAN_ACTIVATOR_H__
#define _MONJU_BAYESIAN_ACTIVATOR_H__

#include "MonjuTypes.h"
#include "Closable.h"
#include "PlatformContext.h"
#include "DeviceProgram.h"
#include "DeviceKernel.h"
#include "DeviceKernelArguments.h"
#include "DeviceMemory.h"
#include <map>
#include <boost/lexical_cast.hpp>

namespace monju {

	class BayesianActivator : Closable
	{
	private:
		std::string
			_kSrcOobpUp1 = "",
			_kSrcOobpUp2 = "",
			_kSrcOobpDown1 = "",
			_kSrcOobpDown2 = "",
			_kKernelOobpUp1 = "",
			_kKernelOobpUp2 = "",
			_kKernelOobpDown1 = "",
			_kKernelOobpDown2 = "";

		const int
			_kNodesX,
			_kNodesY,
			_kUnitsPerNodeX,
			_kUnitsPerNodeY;

		std::weak_ptr<PlatformContext> _platformContext;

		DeviceProgram 
			_oobpUp1, 
			_oobpUp2, 
			_oobpDown1, 
			_oobpDown2;

		DeviceKernel
			_kernelOobpUp1,
			_kernelOobpUp2,
			_kernelOobpDown1,
			_kernelOobpDown2;

	public:
		BayesianActivator(const BayesianActivator&) = delete;
		BayesianActivator& operator =(const BayesianActivator&) = delete;

		BayesianActivator(int nodesX, int nodesY, int unitsPerNodeX, int unitsPerNodeY, std::weak_ptr<PlatformContext> platformContext) :
			_kNodesX(nodesX),
			_kNodesY(nodesY),
			_kUnitsPerNodeX(unitsPerNodeX),
			_kUnitsPerNodeY(unitsPerNodeY)
		{
			_platformContext = platformContext;

			std::map<std::string, std::string> params_map;
			params_map["X"] = boost::lexical_cast<std::string>(_kNodesX);
			params_map["Y"] = boost::lexical_cast<std::string>(_kNodesY);
			params_map["XU"] = boost::lexical_cast<std::string>(_kUnitsPerNodeX);
			params_map["YU"] = boost::lexical_cast<std::string>(_kUnitsPerNodeY);

			if (auto c = _platformContext.lock())
			{
				_oobpUp1.create(c->deviceContext(), c->deviceContext().getAllDevices(), _kSrcOobpUp1, params_map);
				_oobpUp2.create(c->deviceContext(), c->deviceContext().getAllDevices(), _kSrcOobpUp2, params_map);
				_oobpDown1.create(c->deviceContext(), c->deviceContext().getAllDevices(), _kSrcOobpDown1, params_map);
				_oobpDown2.create(c->deviceContext(), c->deviceContext().getAllDevices(), _kSrcOobpDown2, params_map);

				_kernelOobpUp1.create(_oobpUp1, _kKernelOobpUp1, params_map);
				_kernelOobpUp2.create(_oobpUp2, _kKernelOobpUp2, params_map);
				_kernelOobpDown1.create(_oobpDown1, _kKernelOobpDown1, params_map);
				_kernelOobpDown2.create(_oobpDown2, _kKernelOobpDown2, params_map);
			}
		}

		void up(Device& device, DeviceMemory& memx, DeviceMemory& memy, DeviceMemory& memw)
		{
			_up(device, memx, memy, memw);
			memw.flushRead();
			memx.flushRead();
		}

		void down(Device& device, DeviceMemory& memx, DeviceMemory& memy, DeviceMemory& memw)
		{
			_down(device, memx, memy, memw);
			memw.flushRead();
			memy.flushRead();
		}

		void both(Device& device, DeviceMemory& memx, DeviceMemory& memy, DeviceMemory& memw)
		{
			_up(device, memx, memy, memw);
			_down(device, memx, memy, memw);
			memx.flushRead();
			memw.flushRead();
			memy.flushRead();
		}

	private:

		void _up(Device& device, DeviceMemory& memx, DeviceMemory& memy, DeviceMemory& memw)
		{
			DeviceKernelArguments argUp1;
			argUp1.push(memy, monju::VariableKind::lambda, false);
			argUp1.push(memy, monju::VariableKind::pi, false);
			argUp1.push(memw, monju::VariableKind::W, false);
			argUp1.push(memw, monju::VariableKind::kappa, false);
			argUp1.push(memw, monju::VariableKind::lambda, true);
			argUp1.stackArguments(_kernelOobpUp1);

			std::vector<size_t> global_work_size1;
			global_work_size1.push_back(_kNodesY);
			global_work_size1.push_back(_kNodesX);
			std::vector<size_t> local_work_size1;
			local_work_size1.push_back(1);
			local_work_size1.push_back(_kNodesX);

			// (1) É…_YåvéZ
			_kernelOobpUp1.compute(device, global_work_size1, local_work_size1);
			memw.requireRead(argUp1.outputParams());

			monju::DeviceKernelArguments argUp2;
			argUp2.push(memw, monju::VariableKind::lambda, false);
			argUp2.push(memx, monju::VariableKind::R, false);
			argUp2.push(memx, monju::VariableKind::lambda, true);
			argUp2.stackArguments(_kernelOobpUp2);

			std::vector<size_t> global_work_size2;
			global_work_size2.push_back(1);
			global_work_size2.push_back(_kNodesX);

			// (2) É…åvéZ
			_kernelOobpUp2.compute(device, global_work_size2);
			memx.requireRead(argUp2.outputParams());

			//memw.flushRead();
			//memx.flushRead();
		}

		void _down(Device& device, DeviceMemory& memx, DeviceMemory& memy, DeviceMemory& memw)
		{
			monju::DeviceKernelArguments argDown1;
			argDown1.push(memx, monju::VariableKind::rho, false);
			argDown1.push(memw, monju::VariableKind::W, false);
			argDown1.push(memw, monju::VariableKind::lambda, false);
			argDown1.push(memw, monju::VariableKind::kappa, true);
			argDown1.stackArguments(_kernelOobpDown1);

			std::vector<size_t> global_work_size1;
			global_work_size1.push_back(_kNodesY);
			global_work_size1.push_back(_kNodesX);
			std::vector<size_t> local_work_size1;
			local_work_size1.push_back(_kNodesY);
			local_work_size1.push_back(1);

			// É»_X
			_kernelOobpDown1.compute(device, global_work_size1, local_work_size1);
			memw.requireRead(argDown1.outputParams());

			monju::DeviceKernelArguments argDown2;
			argDown2.push(memw, monju::VariableKind::kappa, false);
			argDown2.push(memy, monju::VariableKind::pi, true);
			argDown2.stackArguments(_kernelOobpDown2);

			std::vector<size_t> global_work_size2;
			global_work_size2.push_back(_kNodesY);
			global_work_size2.push_back(1);

			// ÉŒ
			_kernelOobpDown2.compute(device, global_work_size2);
			memy.requireRead(argDown2.outputParams());
		}

	};
}

#endif // !_MONJU_BAYESIAN_ACTIVATOR_H__
