#include "BelLayerUpdaterFmc.h"

/*コンストラクタ*/

monju::BelLayerUpdaterFmc::BelLayerUpdaterFmc(std::weak_ptr<Environment> env, LayerShape shape, std::weak_ptr<ClMachine> clMachine)
{
	_initInstance(shape, env, clMachine);
	_createClKernel();
}

monju::BelLayerUpdaterFmc::~BelLayerUpdaterFmc()
{
}

void monju::BelLayerUpdaterFmc::bel(std::weak_ptr<ClDeviceContext> clDeviceContext, std::weak_ptr<BelLayerFmc> layer, std::weak_ptr<ClEventJoiner> clEventJoiner)
{
	/// デバイスに必要なデータが転送されている前提

	auto p = clDeviceContext.lock();
	// joinはnull可能
	_bel(clDeviceContext, layer, clEventJoiner);
	p->flush(); // コマンドキューをフラッシュ
}

void monju::BelLayerUpdaterFmc::_initInstance(LayerShape& shape, std::weak_ptr<Environment> env, std::weak_ptr<ClMachine> clMachine)
{
	_capturePointers(env, clMachine);
	_shape = shape;
}

void monju::BelLayerUpdaterFmc::_capturePointers(std::weak_ptr<Environment> env, std::weak_ptr<ClMachine> clMachine)
{
	_env = env.lock();
	_clMachine = clMachine.lock();
}

void monju::BelLayerUpdaterFmc::_createClKernel()
{
	param_map param_map;
	param_map["X"] = boost::lexical_cast<std::string>(_shape.nodes.size());
	param_map["XU"] = boost::lexical_cast<std::string>(_shape.units.size());

	std::filesystem::path kernelPathBase = _env->info().kernelFolder;
	_clKernel = std::make_shared<ClKernel>(
		_clMachine,
		(kernelPathBase / _kSrcOobpBel).string(),
		_kKernelOobpBel,
		param_map);
}

void monju::BelLayerUpdaterFmc::_bel(std::weak_ptr<ClDeviceContext> clDeviceContext, std::weak_ptr<BelLayerFmc> layer, std::weak_ptr<ClEventJoiner> clEventJoiner)
{
	auto pNode = layer.lock();

	ClFunc func(_clMachine, _clKernel);
	func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::lambda));
	func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::pi));
	func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::rho));
	func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::BEL));
	func.pushArgument(pNode->clVariableSet().getClMemory(VariableKind::WIN));

	std::vector<size_t> global_work_size = { static_cast<size_t>(_shape.nodes.size()) };

	func.execute(clDeviceContext, global_work_size, clEventJoiner);
}
