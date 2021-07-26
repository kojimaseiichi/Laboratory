#include "FullBayesianMatrixLayerUpdaterFmc.h"
#include "ClDeviceContext.h"
#include "VariableKind.h"

// コンストラクタ

monju::FullBayesianMatrixLayerUpdaterFmc::FullBayesianMatrixLayerUpdaterFmc(std::weak_ptr<Environment> env, LayerShape shapeX, LayerShape shapeY, std::weak_ptr<ClMachine> clMachine)
{
	_captureExternalResources(shapeX, shapeY, env, clMachine);
	_create_kernel();
}

monju::FullBayesianMatrixLayerUpdaterFmc::~FullBayesianMatrixLayerUpdaterFmc()
{
}

// 公開メンバ

void monju::FullBayesianMatrixLayerUpdaterFmc::up(std::weak_ptr<ClDeviceContext> clDeviceContext, std::weak_ptr<BelLayerFmc> x, std::weak_ptr<BelLayerFmc> y, std::weak_ptr<FullBayesianMatrixLayerFmc> m, std::weak_ptr<ClEventJoiner> join)
{
	// 全結合逆方向(up)確率伝播(oobp)
	auto layerX = x.lock();
	auto layerY = y.lock();
	auto layerM = m.lock();
	_up(clDeviceContext, *layerX, *layerY, *layerM, join);
	auto pDeviceContext = clDeviceContext.lock();
	pDeviceContext->flush();
}

void monju::FullBayesianMatrixLayerUpdaterFmc::down(std::weak_ptr<ClDeviceContext> clDeviceContext, std::weak_ptr<BelLayerFmc> x, std::weak_ptr<BelLayerFmc> y, std::weak_ptr<FullBayesianMatrixLayerFmc> m, std::weak_ptr<ClEventJoiner> join)
{
	// 全結合順方向(down)確率伝播(oobp)
	auto layerX = x.lock();
	auto layerY = y.lock();
	auto layerM = m.lock();
	_down(clDeviceContext, *layerX, *layerY, *layerM, join);
	auto pDeviceContext = clDeviceContext.lock();
	pDeviceContext->flush();
}

void monju::FullBayesianMatrixLayerUpdaterFmc::both(std::weak_ptr<ClDeviceContext> clDeviceContext, std::weak_ptr<BelLayerFmc> x, std::weak_ptr<BelLayerFmc> y, std::weak_ptr<FullBayesianMatrixLayerFmc> m, std::weak_ptr<ClEventJoiner> join)
{
	// 全結合双方向確率伝播
	auto layerX = x.lock();
	auto layerY = y.lock();
	auto layerM = m.lock();
	_up(clDeviceContext, *layerX, *layerY, *layerM, join);
	_down(clDeviceContext, *layerX, *layerY, *layerM, join);
	auto pDeviceContext = clDeviceContext.lock();
	pDeviceContext->flush();
}

// ヘルパー

void monju::FullBayesianMatrixLayerUpdaterFmc::_captureExternalResources(LayerShape shapeX, LayerShape shapeY, std::weak_ptr<Environment> env, std::weak_ptr<ClMachine> clMachine)
{
	_shapeX = shapeX;
	_shapeY = shapeY;
	_env = env.lock();
	_clMachine = clMachine.lock();
}

void monju::FullBayesianMatrixLayerUpdaterFmc::_create_kernel()
{
	param_map param_map; // カーネルのコンパイル時テンプレート変数
	param_map["X"] = boost::lexical_cast<std::string>(_shapeX.nodes.size());
	param_map["Y"] = boost::lexical_cast<std::string>(_shapeY.nodes.size());
	param_map["XU"] = boost::lexical_cast<std::string>(_shapeX.units.size());
	param_map["YU"] = boost::lexical_cast<std::string>(_shapeY.units.size());

	std::filesystem::path kernelPathBase = _env->info().kernel_folder; // カーネルファイルのフォルダ

	_kernelOobpUp1 = std::make_shared<ClKernel>(
		_clMachine,
		kernelPathBase / _kSrcOobpUp1,
		_kKernelOobpUp1,
		param_map);

	_kernelOobpUp2 = std::make_shared<ClKernel>(
		_clMachine,
		kernelPathBase / _kSrcOobpUp2,
		_kKernelOobpUp2,
		param_map);

	_kernelOobpDown1 = std::make_shared<ClKernel>(
		_clMachine,
		kernelPathBase / _kSrcOobpDown1,
		_kKernelOobpDown1,
		param_map);

	_kernelOobpDown2 = std::make_shared<ClKernel>(
		_clMachine,
		kernelPathBase / _kSrcOobpDown2,
		_kKernelOobpDown2,
		param_map);
}

void monju::FullBayesianMatrixLayerUpdaterFmc::_up(std::weak_ptr<ClDeviceContext> clDeviceContext, BelLayerFmc& layerX, BelLayerFmc& layerY, FullBayesianMatrixLayerFmc& edge, std::weak_ptr<ClEventJoiner> join)
{
	_upOobp1(clDeviceContext, layerY, edge, join);
	_upOobp2(clDeviceContext, layerX, edge, join);
}

void monju::FullBayesianMatrixLayerUpdaterFmc::_upOobp1(std::weak_ptr<ClDeviceContext> clDeviceContext, BelLayerFmc& layerY, FullBayesianMatrixLayerFmc& matrixLayer, std::weak_ptr<ClEventJoiner> join)
{
	ClFunc func1(_clMachine, _kernelOobpUp1);

	func1.push_kernel_argument(layerY.clVariableSet().get_cl_memory(VariableKind::lambda));
	func1.push_kernel_argument(layerY.clVariableSet().get_cl_memory(VariableKind::pi));
	func1.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::W));
	func1.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::kappa));
	func1.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::lambda));

	std::vector<size_t> global_work_size1 = { static_cast<size_t>(_shapeY.nodes.size()), static_cast<size_t>(_shapeX.nodes.size()) };
	std::vector<size_t> local_work_size1 = { 1, static_cast<size_t>(_shapeX.nodes.size()) };

	func1.execute_kernel(clDeviceContext, global_work_size1, local_work_size1, join);
	//matrixLayer.clVariableSet().enqueueRead(clDeviceContext, join, VariableKind::lambda);
}

void monju::FullBayesianMatrixLayerUpdaterFmc::_upOobp2(std::weak_ptr<ClDeviceContext> clDeviceContext, BelLayerFmc& layerX, FullBayesianMatrixLayerFmc& matrixLayer, std::weak_ptr<ClEventJoiner> join)
{
	ClFunc func2(_clMachine, _kernelOobpUp2);

	func2.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::lambda));
	func2.push_kernel_argument(layerX.clVariableSet().get_cl_memory(VariableKind::R));
	func2.push_kernel_argument(layerX.clVariableSet().get_cl_memory(VariableKind::lambda));

	std::vector<size_t> global_work_size2 = { 1, static_cast<size_t>(_shapeX.nodes.size()) };

	func2.execute_kernel(clDeviceContext, global_work_size2, join);
	//layerX.clVariableSet().enqueueRead(clDeviceContext, join, VariableKind::lambda);
}

void monju::FullBayesianMatrixLayerUpdaterFmc::_down(std::weak_ptr<ClDeviceContext> clDeviceContext, BelLayerFmc& nodeX, BelLayerFmc& nodeY, FullBayesianMatrixLayerFmc& matrixLayer, std::weak_ptr<ClEventJoiner> join)
{
	_downOobp1(clDeviceContext, nodeX, matrixLayer, join);
	_downOobp2(clDeviceContext, nodeY, matrixLayer, join);
}

void monju::FullBayesianMatrixLayerUpdaterFmc::_downOobp1(std::weak_ptr<ClDeviceContext> clDeviceContext, BelLayerFmc& layerX, FullBayesianMatrixLayerFmc& matrixLayer, std::weak_ptr<ClEventJoiner> join)
{
	ClFunc func1(_clMachine, _kernelOobpDown1);

	func1.push_kernel_argument(layerX.clVariableSet().get_cl_memory(VariableKind::rho));
	func1.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::W));
	func1.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::lambda));
	func1.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::kappa));

	std::vector<size_t> global_work_size1 = { static_cast<size_t>(_shapeY.nodes.size()) , static_cast<size_t>(_shapeX.nodes.size()) };
	std::vector<size_t> local_work_size1 = { static_cast<size_t>(_shapeY.nodes.size()), 1 };

	func1.execute_kernel(clDeviceContext, global_work_size1, local_work_size1, join);
	//edge.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::kappa);
}

void monju::FullBayesianMatrixLayerUpdaterFmc::_downOobp2(std::weak_ptr<ClDeviceContext> clDeviceContext, BelLayerFmc& layerY, FullBayesianMatrixLayerFmc& matrixLayer, std::weak_ptr<ClEventJoiner> join)
{
	ClFunc func2(_clMachine, _kernelOobpDown2);

	func2.push_kernel_argument(matrixLayer.clVariableSet().get_cl_memory(VariableKind::kappa));
	func2.push_kernel_argument(layerY.clVariableSet().get_cl_memory(VariableKind::pi));

	std::vector<size_t> global_work_size2 = { static_cast<size_t>(_shapeY.nodes.size()), 1 };

	func2.execute_kernel(clDeviceContext, global_work_size2, join);
	//nodeX.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::pi);
}
