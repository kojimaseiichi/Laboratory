#include "MatrixLayerStorage.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="fileName">�f�[�^�t�@�C����</param>
/// <param name="x">��ʃ��C���[�`��</param>
/// <param name="y">���ʃ��C���[�`��</param>
/// <returns></returns>

monju::MatrixLayerStorage::MatrixLayerStorage(std::string fileName, LayerShape x, LayerShape y)
	: GridMatrixStorage(fileName),
	_conc(1)
{
	_gridExtent.cross(x, y);
	_gridExtentLambda = _gridExtentKappa = _gridExtent;
	_gridExtentLambda.matrix.rows = 1;
	_gridExtentKappa.matrix.cols = 1;
	_prepareStorage();
}

/// <summary>
/// �f�X�g���N�^
/// </summary>

monju::MatrixLayerStorage::~MatrixLayerStorage()
{

}

/// <summary>
/// �Ƀf�[�^�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">��</param>

void monju::MatrixLayerStorage::persistLambda(bool storing, MatrixCm<float_t>& lambda)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_LAMBDA_VARIABLES, lambda);
	else			readGrid<MatrixCm<float_t>>(_LAMBDA_VARIABLES, lambda);
}

/// <summary>
/// �ȃf�[�^�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">��</param>

void monju::MatrixLayerStorage::persistKappa(bool storing, MatrixCm<float_t>& kappa)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_KAPPA_VARIABLES, kappa);
	else			readGrid<MatrixCm<float_t>>(_KAPPA_VARIABLES, kappa);
}

/// <summary>
/// �����\�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">��</param>

void monju::MatrixLayerStorage::persistCrossTab(bool storing, MatrixCm<int32_t>& crosstab)
{
	if (storing)	writeGrid<MatrixCm<int32_t>>(_CONTINGENCY_TABLE, crosstab);
	else			readGrid<MatrixCm<int32_t>>(_CONTINGENCY_TABLE, crosstab);
}

/// <summary>
/// �����\�̍����̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">��</param>

void monju::MatrixLayerStorage::persistCrossTabDiff(bool storing, MatrixCm<int32_t>& diff)
{
	if (storing)	writeGrid<MatrixCm<int32_t>>(_INCREMENTAL_DIFF, diff);
	else			readGrid<MatrixCm<int32_t>>(_INCREMENTAL_DIFF, diff);
}

/// <summary>
/// ���ݏ��ʂ̉i�����^����
/// </summary>
/// <param name="storing"></param>
/// <param name="mi"></param>

void monju::MatrixLayerStorage::persistMi(bool storing, MatrixCm<float>& mi)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_MUTUAL_INFORMATION, mi);
	else			readGrid<MatrixCm<float_t>>(_MUTUAL_INFORMATION, mi);
}

/// <summary>
/// CPT�f�[�^�̉i�����^����
/// </summary>
/// <param name="storing">true:�i���� | false:����</param>
/// <param name="lambda">CPT</param>

void monju::MatrixLayerStorage::persistCpt(bool storing, MatrixCm<float_t>& cpt)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_CONDITIONAL_PROBABILITY_TABLE, cpt);
	else			readGrid<MatrixCm<float_t>>(_CONDITIONAL_PROBABILITY_TABLE, cpt);
}

/// <summary>
/// �����\�̍��������C���[�Ԃ̏��҃��j�b�g�ŉ��Z
/// </summary>
/// <param name="xWinner"></param>
/// <param name="yWinner"></param>
/// <returns></returns>

std::future<void> monju::MatrixLayerStorage::asyncIncrementDiff(std::weak_ptr<MatrixRm<int32_t>> xWinner, std::weak_ptr<MatrixRm<int32_t>> yWinner)
{
	// �񓯊������̏����Ƃ��ď��҃��j�b�g�̃f�[�^���R�s�[
	auto px = xWinner.lock();
	auto py = yWinner.lock();
	auto ax = _toVector(*px);
	auto ay = _toVector(*py);
	auto wrapper = [&] {
		// �L���v�`���[�Fax, ay
		this->coeffOp<int32_t>(
			_INCREMENTAL_DIFF,
			ax,
			ay,
			[](const int32_t v)->int32_t {
				return v + 1;
			});
	};
	auto f = _conc.threadPool().submit(wrapper);
	return f;
}

/// <summary>
/// �����\���X�V
/// </summary>
/// <returns></returns>

std::future<void> monju::MatrixLayerStorage::asyncUpdateContingencyTable()
{
	auto future = _conc.threadPool().submit([&] {
		_updateContingencyTableJoiningDiff();
		});
	return future;
}

/// <summary>
/// �����𕪊��\�ɔ��f����CPT���Čv�Z
/// </summary>
/// <returns></returns>

std::future<void> monju::MatrixLayerStorage::asyncUpdateCpt()
{
	auto topo = _cptTopologyTorus();
	auto future = _conc.threadPool().submit([&] {
		_updateContingencyTableAndCpt(topo);
		});
	return future;
}
