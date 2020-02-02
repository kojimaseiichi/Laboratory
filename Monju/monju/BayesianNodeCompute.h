#pragma once
#ifndef _MONJU_BAYESIAN_NODE_COMPUTE_H__
#define _MONJU_BAYESIAN_NODE_COMPUTE_H__

#include "MonjuTypes.h"
#include "PlatformContext.h"
#include "DeviceProgram.h"
#include "DeviceKernel.h"
#include "DeviceKernelArguments.h"
#include "DeviceMemory.h"
#include <map>
#include <boost/lexical_cast.hpp>
#include "BayesianNodeDevice.h"

namespace monju {

	class BayesianNodeCompute
	{
	private:
		const std::string
			_kSrcOobpBel = "oobp\\oobp3_bel.cl",
			_kKernelOobpBel = "oobp3_bel_X${X}_XU${XU}";

		const int
			_kNodes,
			_kUnitsPerNode;

		PlatformContext* _pPlatformContext;

		DeviceProgram _pgmOobpBel;

		DeviceKernel _kernelOobpBel;

	public:
		BayesianNodeCompute(const BayesianNodeCompute&) = delete;
		BayesianNodeCompute& operator =(const BayesianNodeCompute&) = delete;

		BayesianNodeCompute(int nodes, int unitsPerNode, PlatformContext& platformContext)
			: _kNodes(nodes), _kUnitsPerNode(unitsPerNode)
		{
			_pPlatformContext = &platformContext;

			std::map<std::string, std::string> params_map;
			params_map["X"] = boost::lexical_cast<std::string>(_kNodes);
			params_map["XU"] = boost::lexical_cast<std::string>(_kUnitsPerNode);

			_pgmOobpBel.create(_pPlatformContext->deviceContext(), _pPlatformContext->deviceContext().getAllDevices(), util_file::combine(_pPlatformContext->kernelDir(), _kSrcOobpBel), params_map);
			_kernelOobpBel.create(_pgmOobpBel, _kKernelOobpBel, params_map);
		}

		void bel(BayesianNodeDevice& node)
		{
			_bel(node.device(), node.mem());
		}

	private:
		void _bel(Device& device, DeviceMemory& mem)
		{
			DeviceKernelArguments arg;
			arg.push(mem, VariableKind::lambda, false);
			arg.push(mem, VariableKind::pi, false);
			arg.push(mem, VariableKind::rho, true);
			arg.push(mem, VariableKind::BEL, true);
			arg.push(mem, VariableKind::WIN, true);
			arg.stackArguments(_kernelOobpBel);

			std::vector<size_t> global_work_size;
			global_work_size.push_back(_kNodes);

			_kernelOobpBel.compute(device, global_work_size);
			mem.requireRead(arg.outputParams());
		}
	};
}

#endif // !_MONJU_BAYESIAN_NODE_COMPUTE_H__
