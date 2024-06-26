#include "MatrixLayerStorage.h"
#include "util_file.h"
#include "util_ai.h"


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

/// <summary>
/// 分割表(CROSSTAB)から条件付確率表(CPT)を計算<br>
/// φ値ノードは考慮しない。通常はupdateCptでCPTを更新していくため、分割表から直接計算した結果と一致しない。
/// </summary>
/// <param name="cpt"></param>

void monju::MatrixLayerStorage::calcCpt(std::weak_ptr<MatrixCm<float>> cpt)
{
	auto pCpt = cpt.lock();
	auto gridExtent2d = _gridExtent.flattenCm();
	if (gridExtent2d.rows != pCpt->rows() || gridExtent2d.cols != pCpt->cols())
		throw MonjuException();
	MatrixCm<int32_t> crosstab;
	crosstab.resize(gridExtent2d.rows, gridExtent2d.cols);
	this->readGrid(_CONTINGENCY_TABLE, crosstab);
	*pCpt = crosstab.cast<float>();
	const int matSize = _gridExtent.matrix.size();
	for (int gcol = 0; gcol < gridExtent2d.cols; gcol++)
	{
		for (int grow = 0; grow < gridExtent2d.rows; grow++)
		{
			auto v = pCpt->block(static_cast<int>(grow * matSize), gcol, matSize, 1).reshaped(_gridExtent.matrix.rows, _gridExtent.matrix.cols);
			auto sum = v.sum();
			v.array() /= sum;
		}
	}
}

/// <summary>
/// CPTから相互情報量を計算する。
/// 相互情報量からノード間の
/// </summary>

std::future<void> monju::MatrixLayerStorage::asyncCalcMi()
{
	auto future = _conc.threadPool().submit([&] {
		_updateMi();
		});
	return future;
}

void monju::MatrixLayerStorage::_updateContingencyTableJoiningDiff()
{
	_grid_matrix_t entryCt, entryDif;
	if (!_findGridMatrix(_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
	if (!_findGridMatrix(_INCREMENTAL_DIFF, entryDif)) throw MonjuException();

	MatrixCm<int32_t> ct, dif;
	auto matShape = _gridExtent.matrix;
	ct.resize(matShape.rows, matShape.cols);
	dif.resizeLike(ct);
	for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
	{
		for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
		{
			_cell_data_t cellCt, cellDif;
			_getCellData(entryCt, grow, gcol, cellCt);
			_getCellData(entryDif, grow, gcol, cellDif);
			_readMatrixData(entryCt, cellCt, ct);
			_readMatrixData(entryDif, cellDif, dif);
			ct += dif;
			_writeMatrixData(entryCt, cellCt, ct);
		}
	}
	_setCellDataZeros(entryDif);
}

std::function<int(int, int, int)> monju::MatrixLayerStorage::_cptTopologyTorus()
{
	return [](int win, int col, int cycle) {
		if (win > col)
			std::swap(win, col);
		return std::min(col - win, (win + cycle) - col);
	};
}

void monju::MatrixLayerStorage::_updateContingencyTableAndCpt(const std::function<float(int, int, int)> cptTopology)
{
	_grid_matrix_t entryCt, entryCg, entryCpt;
	if (!_findGridMatrix(_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
	if (!_findGridMatrix(_INCREMENTAL_DIFF, entryCg)) throw MonjuException();
	if (!_findGridMatrix(_CONDITIONAL_PROBABILITY_TABLE, entryCpt)) throw MonjuException();

	MatrixCm<int32_t> ct, dif;
	MatrixCm<float> cpt, delta;
	auto matShape = _gridExtent.matrix;
	ct.resize(matShape.rows, matShape.cols);
	dif.resizeLike(ct);
	cpt.resize(matShape.rows, matShape.cols);
	delta.resizeLike(cpt);
	// ループ(グリッド巡回)
	for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
	{
		for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
		{
			_cell_data_t cellCt, cellCg, cellCpt;
			_getCellData(entryCt, grow, gcol, cellCt);
			_getCellData(entryCg, grow, gcol, cellCg);
			_getCellData(entryCpt, grow, gcol, cellCpt);

			_readMatrixData(entryCt, cellCt, ct);
			_readMatrixData(entryCg, cellCg, dif);
			_readMatrixData(entryCpt, cellCpt, cpt);

			ct += dif;
			_writeMatrixData(entryCt, cellCt, ct);

			delta.setZero();
			// ループ(マトリックス巡回)
			// カウントされた要素をCPTに反映
			int cycle = static_cast<int>(matShape.cols);
			for (auto col = 0; col < matShape.cols; col++)
			{
				for (auto row = 0; row < matShape.rows; row++)
				{
					auto count = dif(row, col);
					if (count == 0) continue;
					// 小さい値の係数をかける必要がある(勝ち数をかけて学習率を調整)
					float eta = 1.f / static_cast<float>(ct(row, col)) * count;
					util_ai::accumulateSomDeltaTorus(delta, col, row, eta);
				}
			}
			cpt += delta;
			util_ai::normalizeCptWithUpperPhiUnit(cpt);
			_writeMatrixData(entryCpt, cellCpt, cpt);
		}
	}
	_setCellDataZeros(entryCg);
}

void monju::MatrixLayerStorage::_updateMi()
{
	_grid_matrix_t entryCpt, entryMi;
	if (!_findGridMatrix(_CONDITIONAL_PROBABILITY_TABLE, entryCpt)) throw MonjuException();
	if (!_findGridMatrix(_MUTUAL_INFORMATION, entryMi)) throw MonjuException();

	MatrixCm<float> cpt;
	MatrixCm<float> mi;
	cpt.resize(_gridExtent.matrix.rows, _gridExtent.matrix.cols);
	mi.resize(_gridExtent.grid.rows, _gridExtent.grid.cols);

	for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
	{
		for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
		{
			_cell_data_t cellCpt;
			_getCellData(entryCpt, grow, gcol, cellCpt);
			_readMatrixData(entryCpt, cellCpt, cpt);
			// 周辺分布P(x), P(y)
			auto cross = cpt.rowwise().sum().col(0) * cpt.colwise().sum().row(0);
			// 相互情報量
			mi(gcol, grow) = (cpt.array() * (cpt.array() / cross.array()).log().array()).sum();
		}
	}
	_cell_data_t cellMi;
	_getCellData(entryMi, 0, 0, cellMi);
	_writeMatrixData(entryMi, cellMi, mi);
}

std::vector<int32_t>&& monju::MatrixLayerStorage::_toVector(MatrixRm<int32_t>& win) const
{
	std::vector<int32_t> v;
	for (int row = 0; row < win.rows(); row++)
		v.push_back(win(row, 0));
	return std::move(v);
}
