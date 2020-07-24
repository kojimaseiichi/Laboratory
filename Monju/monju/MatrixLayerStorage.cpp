#include "MatrixLayerStorage.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="fileName">データファイル名</param>
/// <param name="x">上位レイヤー形状</param>
/// <param name="y">下位レイヤー形状</param>
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
/// デストラクタ
/// </summary>

monju::MatrixLayerStorage::~MatrixLayerStorage()
{

}

/// <summary>
/// λデータの永続化／復元
/// </summary>
/// <param name="storing">true:永続化 | false:復元</param>
/// <param name="lambda">λ</param>

void monju::MatrixLayerStorage::persistLambda(bool storing, MatrixCm<float_t>& lambda)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_LAMBDA_VARIABLES, lambda);
	else			readGrid<MatrixCm<float_t>>(_LAMBDA_VARIABLES, lambda);
}

/// <summary>
/// κデータの永続化／復元
/// </summary>
/// <param name="storing">true:永続化 | false:復元</param>
/// <param name="lambda">κ</param>

void monju::MatrixLayerStorage::persistKappa(bool storing, MatrixCm<float_t>& kappa)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_KAPPA_VARIABLES, kappa);
	else			readGrid<MatrixCm<float_t>>(_KAPPA_VARIABLES, kappa);
}

/// <summary>
/// 分割表の永続化／復元
/// </summary>
/// <param name="storing">true:永続化 | false:復元</param>
/// <param name="lambda">κ</param>

void monju::MatrixLayerStorage::persistCrossTab(bool storing, MatrixCm<int32_t>& crosstab)
{
	if (storing)	writeGrid<MatrixCm<int32_t>>(_CONTINGENCY_TABLE, crosstab);
	else			readGrid<MatrixCm<int32_t>>(_CONTINGENCY_TABLE, crosstab);
}

/// <summary>
/// 分割表の差分の永続化／復元
/// </summary>
/// <param name="storing">true:永続化 | false:復元</param>
/// <param name="lambda">κ</param>

void monju::MatrixLayerStorage::persistCrossTabDiff(bool storing, MatrixCm<int32_t>& diff)
{
	if (storing)	writeGrid<MatrixCm<int32_t>>(_INCREMENTAL_DIFF, diff);
	else			readGrid<MatrixCm<int32_t>>(_INCREMENTAL_DIFF, diff);
}

/// <summary>
/// 相互情報量の永続化／復元
/// </summary>
/// <param name="storing"></param>
/// <param name="mi"></param>

void monju::MatrixLayerStorage::persistMi(bool storing, MatrixCm<float>& mi)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_MUTUAL_INFORMATION, mi);
	else			readGrid<MatrixCm<float_t>>(_MUTUAL_INFORMATION, mi);
}

/// <summary>
/// CPTデータの永続化／復元
/// </summary>
/// <param name="storing">true:永続化 | false:復元</param>
/// <param name="lambda">CPT</param>

void monju::MatrixLayerStorage::persistCpt(bool storing, MatrixCm<float_t>& cpt)
{
	if (storing)	writeGrid<MatrixCm<float_t>>(_CONDITIONAL_PROBABILITY_TABLE, cpt);
	else			readGrid<MatrixCm<float_t>>(_CONDITIONAL_PROBABILITY_TABLE, cpt);
}

/// <summary>
/// 分割表の差分をレイヤー間の勝者ユニットで加算
/// </summary>
/// <param name="xWinner"></param>
/// <param name="yWinner"></param>
/// <returns></returns>

std::future<void> monju::MatrixLayerStorage::asyncIncrementDiff(std::weak_ptr<MatrixRm<int32_t>> xWinner, std::weak_ptr<MatrixRm<int32_t>> yWinner)
{
	// 非同期処理の準備として勝者ユニットのデータをコピー
	auto px = xWinner.lock();
	auto py = yWinner.lock();
	auto ax = _toVector(*px);
	auto ay = _toVector(*py);
	auto wrapper = [&] {
		// キャプチャー：ax, ay
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
/// 分割表を更新
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
/// 差分を分割表に反映してCPTを再計算
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
