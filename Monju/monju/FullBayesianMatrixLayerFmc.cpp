#include "FullBayesianMatrixLayerFmc.h"

monju::FullBayesianMatrixLayerFmc::FullBayesianMatrixLayerFmc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge)
{
	_lockOuterResources(clMachine, edge);
	_allocClMemory();

}

monju::FullBayesianMatrixLayerFmc::~FullBayesianMatrixLayerFmc()
{
}

monju::ClVariableSet& monju::FullBayesianMatrixLayerFmc::clVariableSet()
{
	return _clVariableSet;
}

void monju::FullBayesianMatrixLayerFmc::_lockOuterResources(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge)
{
	_clMachine = clMachine.lock();
	_edge = edge.lock();
}

void monju::FullBayesianMatrixLayerFmc::_allocClMemory()
{
	auto lambda = _edge->lambda().lock();
	auto kappa = _edge->kappa().lock();
	auto cpt = _edge->cpt().lock();

	_clLambda = std::make_shared<ClMemory>(_clMachine, lambda->size() * sizeof(float_t));
	_clKappa = std::make_shared<ClMemory>(_clMachine, kappa->size() * sizeof(float_t));
	_clCpt = std::make_shared<ClMemory>(_clMachine, cpt->size() * sizeof(float_t));

	_clVariableSet.add(_edge->lambda(), VariableKind::lambda, _clLambda);
	_clVariableSet.add(_edge->kappa(), VariableKind::kappa, _clKappa);
	_clVariableSet.add(_edge->cpt(), VariableKind::W, _clCpt);
}
