#include "BelLayerStorage.h"
#include "GridMatrixStorage.h"

monju::BelLayerStorage::BelLayerStorage(std::string fileName, LayerShape shape)
	: _conc(1), GridMatrixStorage(fileName), _cross(shape, shape)
{
	_shape = shape;
	_prepareStorage();
	_conc.create();
}

monju::BelLayerStorage::~BelLayerStorage()
{
}

/// <summary>
/// ƒXƒgƒŒ[ƒW‚Ìg—p€”õ
/// </summary>

void monju::BelLayerStorage::prepareAll()
{
	_prepareStorage();
	if (this->findKey<float>(_COE_LATERAL_INHIBITION, _coefLateralInhibition) == false)
	{
		_coefLateralInhibition = 1.0f;
		this->setKey<float>(_COE_LATERAL_INHIBITION, _coefLateralInhibition);
	}
}

/// <summary>
/// ƒÉ‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">lambda</param>

void monju::BelLayerStorage::persistLambda(bool storing, MatrixRm<float>& lambda)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_LAMBDA, lambda);
	else			this->readMatrix<MatrixRm<float>>(_VAR_LAMBDA, lambda);
}

/// <summary>
/// ƒÎ‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">pi</param>

void monju::BelLayerStorage::persistPi(bool storing, MatrixRm<float>& pi)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_PI, pi);
	else			this->readMatrix<MatrixRm<float>>(_VAR_PI, pi);
}

/// <summary>
/// ƒÏ‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">rho</param>

void monju::BelLayerStorage::persistRho(bool storing, MatrixRm<float>& rho)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_RHO, rho);
	else			this->readMatrix<MatrixRm<float>>(_VAR_RHO, rho);
}

/// <summary>
/// R‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">R</param>

void monju::BelLayerStorage::persistR(bool storing, MatrixRm<float>& r)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_R, r);
	else			this->readMatrix<MatrixRm<float>>(_VAR_R, r);
}

/// <summary>
/// BEL‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">BEL</param>

void monju::BelLayerStorage::persistBEL(bool storing, MatrixRm<float>& bel)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_BEL, bel);
	else			this->readMatrix<MatrixRm<float>>(_VAR_BEL, bel);
}

/// <summary>
/// WIN‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">WIN</param>

void monju::BelLayerStorage::persistWin(bool storing, MatrixRm<int32_t>& win)
{
	if (storing)	this->writeMatrix<MatrixRm<int32_t>>(_VAR_WIN, win);
	else			this->readMatrix<MatrixRm<int32_t>>(_VAR_WIN, win);
}

/// <summary>
/// •ªŠ„•\‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">crosstab</param>

void monju::BelLayerStorage::persistCrossTab(bool storing, MatrixRm<int32_t>& crosstab)
{
	if (storing)	this->writeGrid<MatrixRm<int32_t>>(_CONTINGENCY_TABLE, crosstab);
	else			this->readGrid<MatrixRm<int32_t>>(_CONTINGENCY_TABLE, crosstab);
}

/// <summary>
/// •ªŠ„•\‚Ì·•ª‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">diff</param>

void monju::BelLayerStorage::persistCrossTabDiff(bool storing, MatrixRm<int32_t>& diff)
{
	if (storing)	this->writeGrid<MatrixRm<int32_t>>(_INCREMENTAL_DIFF, diff);
	else			this->readGrid<MatrixRm<int32_t>>(_INCREMENTAL_DIFF, diff);
}

/// <summary>
/// ‘ŠŒİî•ñ—Ê‚Ì‰i‘±‰»^•œŒ³
/// </summary>
/// <param name="storing">true:‰i‘±‰» | false:•œŒ³</param>
/// <param name="lambda">mi</param>

void monju::BelLayerStorage::persistMi(bool storing, MatrixRm<float>& mi)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_MUTUAL_INFORMATION, mi);
	else			this->readMatrix<MatrixRm<float>>(_MUTUAL_INFORMATION, mi);
}

