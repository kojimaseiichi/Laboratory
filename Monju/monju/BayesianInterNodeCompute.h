#pragma once
#ifndef _MONJU_BAYESIAN_ACTIVATOR_H__
#define _MONJU_BAYESIAN_ACTIVATOR_H__

#include "MonjuTypes.h"
#include "PlatformContext.h"
#include "DeviceProgram.h"
#include "DeviceKernel.h"
#include "DeviceKernelArguments.h"
#include "DeviceMemory.h"
#include <map>
#include <boost/lexical_cast.hpp>
#include "BayesianNodeDevice.h"
#include "BayesianEdgeDevice.h"
#include "util_file.h"

namespace monju {

	class BayesianInterNodeCompute
	{
	private:
		const std::string
			_kSrcOobpUp1 = "oobp\\oobp3_full_up_1.cl",
			_kSrcOobpUp2 = "oobp\\oobp3_full_up_2.cl",
			_kSrcOobpDown1 = "oobp\\oobp3_full_down_1.cl",
			_kSrcOobpDown2 = "oobp\\oobp3_full_down_2.cl",
			_kKernelOobpUp1 = "oobp3_full_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpUp2 = "oobp3_full_up_2_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown1 = "oobp3_full_down_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown2 = "oobp3_full_down_2_X${X}_Y${Y}_XU${XU}_YU${YU}";

		const int
			_kNodesX,
			_kNodesY,
			_kUnitsPerNodeX,
			_kUnitsPerNodeY;

		PlatformContext* _pPlatformContext;

		DeviceProgram 
			_pgmOobpUp1, 
			_pgmOobpUp2, 
			_pgmOobpDown1, 
			_pgmOobpDown2;

		DeviceKernel
			_kernelOobpUp1,
			_kernelOobpUp2,
			_kernelOobpDown1,
			_kernelOobpDown2;

		// コピー禁止・ムーブ禁止
	public:
		BayesianInterNodeCompute(const BayesianInterNodeCompute&) = delete;
		BayesianInterNodeCompute(BayesianInterNodeCompute&&) = delete;
		BayesianInterNodeCompute& operator =(const BayesianInterNodeCompute&) = delete;
		BayesianInterNodeCompute& operator =(BayesianInterNodeCompute&&) = delete;

	public:
		BayesianInterNodeCompute(int nodesX, int nodesY, int unitsPerNodeX, int unitsPerNodeY, PlatformContext& platformContext) :
			_kNodesX(nodesX),
			_kNodesY(nodesY),
			_kUnitsPerNodeX(unitsPerNodeX),
			_kUnitsPerNodeY(unitsPerNodeY)
		{
			_pPlatformContext = &platformContext;

			std::map<std::string, std::string> params_map;
			params_map["X"] = boost::lexical_cast<std::string>(_kNodesX);
			params_map["Y"] = boost::lexical_cast<std::string>(_kNodesY);
			params_map["XU"] = boost::lexical_cast<std::string>(_kUnitsPerNodeX);
			params_map["YU"] = boost::lexical_cast<std::string>(_kUnitsPerNodeY);

			_pgmOobpUp1.create(_pPlatformContext->deviceContext(), _pPlatformContext->deviceContext().getAllDevices(), util_file::combine(_pPlatformContext->kernelDir(), _kSrcOobpUp1), params_map);
			_pgmOobpUp2.create(_pPlatformContext->deviceContext(), _pPlatformContext->deviceContext().getAllDevices(), util_file::combine(_pPlatformContext->kernelDir(), _kSrcOobpUp2), params_map);
			_pgmOobpDown1.create(_pPlatformContext->deviceContext(), _pPlatformContext->deviceContext().getAllDevices(), util_file::combine(_pPlatformContext->kernelDir(), _kSrcOobpDown1), params_map);
			_pgmOobpDown2.create(_pPlatformContext->deviceContext(), _pPlatformContext->deviceContext().getAllDevices(), util_file::combine(_pPlatformContext->kernelDir(), _kSrcOobpDown2), params_map);

			_kernelOobpUp1.create(_pgmOobpUp1, _kKernelOobpUp1, params_map);
			_kernelOobpUp2.create(_pgmOobpUp2, _kKernelOobpUp2, params_map);
			_kernelOobpDown1.create(_pgmOobpDown1, _kKernelOobpDown1, params_map);
			_kernelOobpDown2.create(_pgmOobpDown2, _kKernelOobpDown2, params_map);
		}
		~BayesianInterNodeCompute()
		{
			_release();
		}

		void up(BayesianNodeDevice& nodeX, BayesianNodeDevice& nodeY, BayesianEdgeDevice& edge)
		{
			if (nodeX.device().getClDeviceId() != nodeY.device().getClDeviceId() ||
				nodeX.device().getClDeviceId() != edge.device().getClDeviceId())
				throw MonjuException("異なるデバイス");

			_up(nodeX.device(), nodeX.mem(), nodeY.mem(), edge.mem());
		}

		void down(BayesianNodeDevice& nodeX, BayesianNodeDevice& nodeY, BayesianEdgeDevice& edge)
		{
			if (nodeX.device().getClDeviceId() != nodeY.device().getClDeviceId() ||
				nodeX.device().getClDeviceId() != edge.device().getClDeviceId())
				throw MonjuException("異なるデバイス");

			_down(nodeX.device(), nodeX.mem(), nodeY.mem(), edge.mem());
		}

		void both(BayesianNodeDevice& nodeX, BayesianNodeDevice& nodeY, BayesianEdgeDevice& edge)
		{
			if (nodeX.device().getClDeviceId() != nodeY.device().getClDeviceId() ||
				nodeX.device().getClDeviceId() != edge.device().getClDeviceId())
				throw MonjuException("異なるデバイス");

			_up(nodeX.device(), nodeX.mem(), nodeY.mem(), edge.mem());
			_down(nodeX.device(), nodeX.mem(), nodeY.mem(), edge.mem());
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

			// (1) λ_Y計算
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

			// (2) λ計算
			_kernelOobpUp2.compute(device, global_work_size2);
			memx.requireRead(argUp2.outputParams());
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

			// κ_X
			_kernelOobpDown1.compute(device, global_work_size1, local_work_size1);
			memw.requireRead(argDown1.outputParams());

			monju::DeviceKernelArguments argDown2;
			argDown2.push(memw, monju::VariableKind::kappa, false);
			argDown2.push(memy, monju::VariableKind::pi, true);
			argDown2.stackArguments(_kernelOobpDown2);

			std::vector<size_t> global_work_size2;
			global_work_size2.push_back(_kNodesY);
			global_work_size2.push_back(1);

			// π
			_kernelOobpDown2.compute(device, global_work_size2);
			memy.requireRead(argDown2.outputParams());
		}

		void _release()
		{
			_kernelOobpUp1.release();
			_kernelOobpUp2.release();
			_kernelOobpDown1.release();
			_kernelOobpDown2.release();

			_pgmOobpUp1.release();
			_pgmOobpUp2.release();
			_pgmOobpDown1.release();
			_pgmOobpDown2.release();

		}
	};
}

#endif // !_MONJU_BAYESIAN_ACTIVATOR_H__
