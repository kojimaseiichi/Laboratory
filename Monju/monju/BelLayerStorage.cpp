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

float monju::BelLayerStorage::coef_lateral_inhibition()
{
	return _coefLateralInhibition;
}

void monju::BelLayerStorage::coef_lateral_inhibition(float v)
{
	_coefLateralInhibition = v;
	this->setKey<float>(_KV_COE_LATERAL_INHIBITION, v);
}

/// <summary>
/// �X�g���[�W�̎g�p����
/// </summary>

void monju::BelLayerStorage::PrepareAllStorageKeys()
{
	_prepareStorage();
	if (this->findKey<float>(_KV_COE_LATERAL_INHIBITION, _coefLateralInhibition) == false)
	{
		_coefLateralInhibition = 1.0f;
		this->setKey<float>(_KV_COE_LATERAL_INHIBITION, _coefLateralInhibition);
	}
}

void monju::BelLayerStorage::ClearAllStorageKeys()
{
	_clearStorage();
}

/// <summary>
/// ���҃��j�b�g�𕪊��\�ɔ��f
/// </summary>
/// <param name="winner"></param>
/// <returns></returns>

std::future<void> monju::BelLayerStorage::IncrementDiffMatrixAsync(std::weak_ptr<MatrixRm<int32_t>> winner)
{
	// �񓯊������̏����Ƃ��ď��҃��j�b�g�̃f�[�^���R�s�[
	auto px = winner.lock();
	auto a = _toVector(*px);
	auto wrapper = [&](std::shared_ptr<std::vector<int32_t>> p) {
		// �L���v�`���[�Fax, ay
		this->coeffOp<int32_t>(
			_STAT_INCREMENTAL_DIFF,
			*p,
			*p,
			[](const int32_t v)->int32_t {
				return v + 1;
			});
	};
	auto f = _conc.threadPool().submit(wrapper, std::move(a));
	return f;
}

std::future<void> monju::BelLayerStorage::ComputeMutualInfoOfContingencyTableAsync()
{
	auto wrapper = [&]() {
		this->_updateMi();
	};
	auto f = _conc.threadPool().submit(wrapper);
	return f;
}

/// <summary>
/// �ɂ̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">lambda</param>

void monju::BelLayerStorage::persistLambda(bool storing, MatrixRm<float>& lambda)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_LAMBDA, lambda);
	else			this->readMatrix<MatrixRm<float>>(_VAR_LAMBDA, lambda);
}

/// <summary>
/// �΂̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">pi</param>

void monju::BelLayerStorage::persistPi(bool storing, MatrixRm<float>& pi)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_PI, pi);
	else			this->readMatrix<MatrixRm<float>>(_VAR_PI, pi);
}

/// <summary>
/// �ς̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">rho</param>

void monju::BelLayerStorage::persistRho(bool storing, MatrixRm<float>& rho)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_RHO, rho);
	else			this->readMatrix<MatrixRm<float>>(_VAR_RHO, rho);
}

/// <summary>
/// R�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">R</param>

void monju::BelLayerStorage::persistR(bool storing, MatrixRm<float>& r)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_R, r);
	else			this->readMatrix<MatrixRm<float>>(_VAR_R, r);
}

/// <summary>
/// BEL�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">BEL</param>

void monju::BelLayerStorage::persistBEL(bool storing, MatrixRm<float>& bel)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_VAR_BEL, bel);
	else			this->readMatrix<MatrixRm<float>>(_VAR_BEL, bel);
}

/// <summary>
/// WIN�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">WIN</param>

void monju::BelLayerStorage::persistWin(bool storing, MatrixRm<int32_t>& win)
{
	if (storing)	this->writeMatrix<MatrixRm<int32_t>>(_VAR_WIN, win);
	else			this->readMatrix<MatrixRm<int32_t>>(_VAR_WIN, win);
}

/// <summary>
/// �����\�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">crosstab</param>

void monju::BelLayerStorage::persistCrossTab(bool storing, MatrixRm<int32_t>& crosstab)
{
	if (storing)	this->writeGrid<MatrixRm<int32_t>>(_STAT_CONTINGENCY_TABLE, crosstab);
	else			this->readGrid<MatrixRm<int32_t>>(_STAT_CONTINGENCY_TABLE, crosstab);
}

/// <summary>
/// �����\�̍����̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">diff</param>

void monju::BelLayerStorage::persistCrossTabDiff(bool storing, MatrixRm<int32_t>& diff)
{
	if (storing)	this->writeGrid<MatrixRm<int32_t>>(_STAT_INCREMENTAL_DIFF, diff);
	else			this->readGrid<MatrixRm<int32_t>>(_STAT_INCREMENTAL_DIFF, diff);
}

/// <summary>
/// ���ݏ��ʂ̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">mi</param>

void monju::BelLayerStorage::persistMi(bool storing, MatrixRm<float>& mi)
{
	if (storing)	this->writeMatrix<MatrixRm<float>>(_STAT_MUTUAL_INFORMATION, mi);
	else			this->readMatrix<MatrixRm<float>>(_STAT_MUTUAL_INFORMATION, mi);
}

void monju::BelLayerStorage::_prepareStorage()
{
	auto ext = _shape.flatten();
	auto extWin = Extent(ext.rows, 1);
	this->prepare<float>(_VAR_LAMBDA, ext, kRowMajor);
	this->prepare<float>(_VAR_PI, ext, kRowMajor);
	this->prepare<float>(_VAR_RHO, ext, kRowMajor);
	this->prepare<float>(_VAR_R, ext, kRowMajor);
	this->prepare<float>(_VAR_R_LI, ext, kRowMajor);
	this->prepare<float>(_VAR_R_WR, ext, kRowMajor);
	this->prepare<float>(_VAR_BEL, ext, kRowMajor);
	this->prepare<int32_t>(_VAR_WIN, extWin, kRowMajor);
	this->prepare<int32_t>(_STAT_CONTINGENCY_TABLE, _cross, kDensityLowerTriangular, kRowMajor, kRowMajor);
	this->prepare<int32_t>(_STAT_INCREMENTAL_DIFF, _cross, kDensityLowerTriangular, kRowMajor, kRowMajor);
	this->prepare<float>(_STAT_MUTUAL_INFORMATION, _cross.grid, kRowMajor);
}

void monju::BelLayerStorage::_clearStorage()
{
	this->clearGrid(_VAR_LAMBDA);
	this->clearGrid(_VAR_PI);
	this->clearGrid(_VAR_RHO);
	this->clearGrid(_VAR_R);
	this->clearGrid(_VAR_R_LI);
	this->clearGrid(_VAR_R_WR);
	this->clearGrid(_VAR_BEL);
	this->clearGrid(_VAR_WIN);
	this->clearGrid(_STAT_CONTINGENCY_TABLE);
	this->clearGrid(_STAT_INCREMENTAL_DIFF);
	this->clearGrid(_STAT_MUTUAL_INFORMATION);
}

