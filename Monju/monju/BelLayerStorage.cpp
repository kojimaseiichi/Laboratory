#include "BelLayerStorage.h"
#include "GridMatrixStorage.h"

monju::BelLayerStorage::BelLayerStorage(std::string fileName, LayerShape shape)
{
	_ps = std::make_unique<GridMatrixStorage>(fileName);
	_shape = shape;
	_prepareStorage();
}

monju::BelLayerStorage::~BelLayerStorage()
{
}

void monju::BelLayerStorage::prepareAll()
{
	_prepareStorage();
}

void monju::BelLayerStorage::persistLambda(bool storing, MatrixRm<float>& lambda)
{
	if (storing)	_ps->readGrid<MatrixRm<float>>(_VAR_LAMBDA, lambda);
	else			_ps->writeGrid<MatrixRm<float>>(_VAR_LAMBDA, lambda);
}

void monju::BelLayerStorage::persistPi(bool storing, MatrixRm<float>& pi)
{
	if (storing)	_ps->readGrid<MatrixRm<float>>(_VAR_PI, pi);
	else			_ps->writeGrid<MatrixRm<float>>(_VAR_PI, pi);
}

void monju::BelLayerStorage::persistRho(bool storing, MatrixRm<float>& rho)
{
	if (storing)	_ps->readGrid<MatrixRm<float>>(_VAR_RHO, rho);
	else			_ps->writeGrid<MatrixRm<float>>(_VAR_RHO, rho);
}

void monju::BelLayerStorage::persistR(bool storing, MatrixRm<float>& r)
{
	if (storing)	_ps->readGrid<MatrixRm<float>>(_VAR_R, r);
	else			_ps->writeGrid<MatrixRm<float>>(_VAR_R, r);
}

void monju::BelLayerStorage::persistBEL(bool storing, MatrixRm<float>& bel)
{
	if (storing)	_ps->readGrid<MatrixRm<float>>(_VAR_BEL, bel);
	else			_ps->writeGrid<MatrixRm<float>>(_VAR_BEL, bel);
}

void monju::BelLayerStorage::persistWin(bool storing, MatrixRm<int32_t>& win)
{
	if (storing)	_ps->readGrid<MatrixRm<int32_t>>(_VAR_BEL, win);
	else			_ps->writeGrid<MatrixRm<int32_t>>(_VAR_BEL, win);
}

void monju::BelLayerStorage::_prepareStorage()
{
	auto ext = _shape.flatten();
	auto extWin = Extent(ext.rows, 1);
	_ps->prepare<float>(_VAR_LAMBDA, ext, kRowMajor);
	_ps->prepare<float>(_VAR_PI, ext, kRowMajor);
	_ps->prepare<float>(_VAR_RHO, ext, kRowMajor);
	_ps->prepare<float>(_VAR_R, ext, kRowMajor);
	_ps->prepare<float>(_VAR_BEL, ext, kRowMajor);
	_ps->prepare<int32_t>(_VAR_WIN, extWin, kRowMajor);
}
