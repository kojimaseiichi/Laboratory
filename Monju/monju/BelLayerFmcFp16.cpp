#include "BelLayerFmcFp16.h"
#include "ClMachine.h"
#include "BelLayer.h"
#include "ClMemory.h"
#include "VariableKind.h"

monju::BelLayerFmcFp16::BelLayerFmcFp16(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node)
{
	_capturePointer(clMachine, node);
	_initClMemory();
	_initVariableSet();
}

monju::BelLayerFmcFp16::~BelLayerFmcFp16()
{

}

monju::ClVariableSet& monju::BelLayerFmcFp16::clVariableSet()
{
	return _clVariableSet;
}

void monju::BelLayerFmcFp16::_capturePointer(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<BelLayer> node)
{
	// 参照する外部のポインタをロック
	_clMachine = clMachine.lock();
	_node = node.lock();
}

void monju::BelLayerFmcFp16::_initClMemory()
{
	// OpenCLのデバイス上のメモリを初期化
	auto lambda = _node->lambda().lock();
	auto pi = _node->pi().lock();
	auto rho = _node->rho().lock();
	auto r = _node->r().lock();
	auto bel = _node->bel().lock();
	auto win = _node->win().lock();

	_clLambda = std::make_shared<ClMemory>(_clMachine, lambda->size() * sizeof(float));
	_clPi = std::make_shared<ClMemory>(_clMachine, pi->size() * sizeof(float));
	_clRho = std::make_shared<ClMemory>(_clMachine, rho->size() * sizeof(float));
	_clR = std::make_shared<ClMemory>(_clMachine, r->size() * sizeof(float));
	_clBel = std::make_shared<ClMemory>(_clMachine, bel->size() * sizeof(float));
	_clWin = std::make_shared<ClMemory>(_clMachine, win->size() * sizeof(int32_t));
}

void monju::BelLayerFmcFp16::_initVariableSet()
{
	// 変数コンテナであるVariableSetを初期化
	_clVariableSet.add(_node->lambda(), VariableKind::lambda, _clLambda);
	_clVariableSet.add(_node->pi(), VariableKind::pi, _clPi);
	_clVariableSet.add(_node->rho(), VariableKind::rho, _clRho);
	_clVariableSet.add(_node->r(), VariableKind::R, _clR);
	_clVariableSet.add(_node->bel(), VariableKind::BEL, _clBel);
	_clVariableSet.add(_node->win(), VariableKind::WIN, _clWin);
}
