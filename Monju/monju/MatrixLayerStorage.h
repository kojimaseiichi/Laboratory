#pragma once
#ifndef _MONJU_MATRIX_LAYER_STORAGE_H__
#define _MONJU_MATRIX_LAYER_STORAGE_H__

#include <vector>

#include "GridMatrixStorage.h"

#include "Closable.h"
#include "Extent.h"
#include "ConcurrencyContext.h"
#include "Synchronizable.h"

namespace monju {
	

	/// <summary>
	/// 二つの層間の条件付確率表(CPT)、分割表(CT)およびその他変数をファイルに保持する。
	/// ファイル入出力に関係する学習アルゴリズムの一部を実装する。
	/// </summary>
	class MatrixLayerStorage : public GridMatrixStorage
	{
#pragma region Private Field
	private:
		ConcurrencyContext _conc;		// 同時実行コンテキスト
		Synchronizable _syn;			// 同期オブジェクト
		GridExtent _gridExtent;			// CPT形状
		GridExtent _gridExtentLambda;	// λ形状
		GridExtent _gridExtentKappa;	// κ形状
#pragma endregion
#pragma region Private Constant
	private:
		const std::string _CONTINGENCY_TABLE = "ct";				// 分割表 ４階テンソル int32_t
		const std::string _INCREMENTAL_DIFF= "df";					// 分割表の差分 ４階テンソル int32_t
		const std::string _MUTUAL_INFORMATION = "mi";				// ノード間相互情報量 行列 float
		const std::string _CONDITIONAL_PROBABILITY_TABLE = "cpt";	// 条件付確率表 ４階テンソル float
		const std::string _LAMBDA_VARIABLES = "lambda";				// λ ３階テンソル
		const std::string _KAPPA_VARIABLES = "kappa";				// κ ３階テンソル
#pragma endregion
#pragma region Constructor
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="fileName">データファイル名</param>
		/// <param name="x">上位レイヤー形状</param>
		/// <param name="y">下位レイヤー形状</param>
		/// <returns></returns>
		MatrixLayerStorage(std::string fileName, LayerShape x, LayerShape y);
		/// <summary>
		/// デストラクタ
		/// </summary>
		~MatrixLayerStorage();
#pragma endregion
#pragma region Persistent
	public:
		/// <summary>
		/// λデータの永続化／復元
		/// </summary>
		/// <param name="storing">true:永続化 | false:復元</param>
		/// <param name="lambda">λ</param>
		void persistLambda(bool storing, MatrixCm<float>& lambda);
		/// <summary>
		/// κデータの永続化／復元
		/// </summary>
		/// <param name="storing">true:永続化 | false:復元</param>
		/// <param name="lambda">κ</param>
		void persistKappa(bool storing, MatrixCm<float>& kappa);
		/// <summary>
		/// 分割表の永続化／復元
		/// </summary>
		/// <param name="storing">true:永続化 | false:復元</param>
		/// <param name="lambda">κ</param>
		void persistCrossTab(bool storing, MatrixCm<int32_t>& crosstab);
		/// <summary>
		/// 分割表の差分の永続化／復元
		/// </summary>
		/// <param name="storing">true:永続化 | false:復元</param>
		/// <param name="lambda">κ</param>
		void persistCrossTabDiff(bool storing, MatrixCm<int32_t>& diff);
		/// <summary>
		/// 相互情報量の永続化／復元
		/// </summary>
		/// <param name="storing">true:永続化 | false:復元</param>
		/// <param name="lambda">κ</param>
		void persistMi(bool storing, MatrixCm<float>& mi);
		/// <summary>
		/// CPTデータの永続化／復元
		/// </summary>
		/// <param name="storing">true:永続化 | false:復元</param>
		/// <param name="lambda">CPT</param>
		void persistCpt(bool storing, MatrixCm<float>& cpt);
#pragma endregion
#pragma region Public Method
		/// <summary>
		/// データファイルに必要なエントリを作成
		/// </summary>
		void prepareAll()
		{
			_prepareStorage();
		}
		/// <summary>
		/// 分割表の差分をレイヤー間の勝者ユニットで加算
		/// </summary>
		/// <param name="xWinner"></param>
		/// <param name="yWinner"></param>
		/// <returns></returns>
		std::future<void> asyncIncrementDiff(std::weak_ptr<MatrixRm<int32_t>> xWinner, std::weak_ptr<MatrixRm<int32_t>> yWinner);
		/// <summary>
		/// 差分を分割表に反映して更新
		/// </summary>
		/// <returns></returns>
		std::future<void> asyncUpdateContingencyTable();
		/// <summary>
		/// 差分を分割表に反映してCPTを再計算(多分使わない)
		/// </summary>
		/// <returns></returns>
		std::future<void> asyncUpdateCpt();
		/// <summary>
		/// 分割表(CROSSTAB)から条件付確率表(CPT)を計算<br>
		/// φ値ノードは考慮しない。通常はupdateCptでCPTを更新していくため、分割表から直接計算した結果と一致しない。
		/// </summary>
		/// <param name="cpt"></param>
		void calcCpt(std::weak_ptr<MatrixCm<float>> cpt);
		/// <summary>
		/// CPTから相互情報量を計算する。
		/// 相互情報量からノード間の
		/// </summary>
		std::future<void> asyncCalcMi();
#pragma endregion
#pragma region Helper
	private:/*ヘルパ*/
		void _prepareStorage()
		{
			this->prepare<int32_t>(_CONTINGENCY_TABLE, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<int32_t>(_INCREMENTAL_DIFF, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float>(_MUTUAL_INFORMATION, _gridExtent.matrix, kColMajor);
			this->prepare<float>(_CONDITIONAL_PROBABILITY_TABLE, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float>(_LAMBDA_VARIABLES, _gridExtentLambda, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float>(_KAPPA_VARIABLES, _gridExtentKappa, kDensityRectangular, kColMajor, kColMajor);
		}
		void _updateContingencyTableJoiningDiff();
		std::function<int(int, int, int)> _cptTopologyTorus();
		void _updateContingencyTableAndCpt(const std::function<float(int, int, int)> cptTopology);
		void _updateMi();
		std::vector<int32_t>&& _toVector(MatrixRm<int32_t>& win) const;
#pragma endregion
	};
}

#endif // !_MONJU_MATRIX_LAYER_STORAGE_H__

