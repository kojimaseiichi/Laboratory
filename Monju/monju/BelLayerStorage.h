#pragma once
#ifndef _MONJU_BEL_LAYER_STORAGE_H__
#define _MONJU_BEL_LAYER_STORAGE_H__

#include <string>
#include "GridMatrixStorage.h"
#include "Extent.h"
#include "ConcurrencyContext.h"
#include "Synchronizable.h"

namespace monju
{
	/// <summary>
	/// BELC[Μi±»f[^
	/// BelLayerNXΕΫ·ιf[^πi±»B
	/// <br/>
	/// </summary>
	class BelLayerStorage : public GridMatrixStorage
	{
#pragma region Private Field
	private:
		Synchronizable _syn;
		ConcurrencyContext _conc;
		LayerShape _shape;
		GridExtent _cross;
		float _coefLateralInhibition;	// €}§ΜW
#pragma endregion
#pragma region Storage Key
		// Xg[WL[
	private:
		// ff[^
		const std::string _VAR_LAMBDA	= "lambda";		// Ι(ήx)
		const std::string _VAR_PI		= "pi";			// Ξ(Oͺz)
		const std::string _VAR_RHO		= "rho";		// Ο(γͺz)
		const std::string _VAR_R		= "r";			// yieB
		const std::string _VAR_R_LI		= "r_li";		// €}§yieB(Win-Rate)
		const std::string _VAR_R_WR		= "r_wr";		// ¦yieB(Lateral Inhibition)
		const std::string _VAR_BEL		= "bel";		// BELIEVE
		const std::string _VAR_WIN		= "win";		// Ώjbg
		// vΚ
		const std::string _STAT_CONTINGENCY_TABLE = "ct";			// ͺ\ SKe\ int32_t
		const std::string _STAT_INCREMENTAL_DIFF = "df";			// ͺ\Μ·ͺ SKe\ int32_t
		const std::string _STAT_MUTUAL_INFORMATION = "mi";			// wΰΜm[hΤέξρΚ sρ float
		// Key-Value
		const std::string _KV_COE_LATERAL_INHIBITION = "cli";		// €}§yieBΜW
#pragma endregion
#pragma region Constructor
	public:
		BelLayerStorage(std::string fileName, LayerShape shape);
		~BelLayerStorage();
#pragma endregion
#pragma region Public Properties
		float	coef_lateral_inhibition();
		void	coef_lateral_inhibition(float v);
#pragma endregion
#pragma region Public Method
	public:
		void PrepareAllStorageKeys();
		void ClearAllStorageKeys();

		std::future<void> IncrementDiffMatrixAsync(std::weak_ptr<MatrixRm<int32_t>> winner);
		std::future<void> ComputeMutualInfoOfContingencyTableAsync();
		/// <summary>
		/// ͺ\©η€}§yieBπvZ
		/// </summary>
		/// <returns></returns>
		std::future<void> asyncUpdateRli()
		{
			auto wrapper = [&]() {
				this->_updateRLI();
			};
			auto f = _conc.threadPool().submit(wrapper);
			return f;
		}
#pragma endregion
#pragma region Persistent
	public:
		/// <summary>
		/// ΙΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">lambda</param>
		void persistLambda(bool storing, MatrixRm<float>& lambda);
		/// <summary>
		/// ΞΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">pi</param>
		void persistPi(bool storing, MatrixRm<float>& pi);
		/// <summary>
		/// ΟΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">rho</param>
		void persistRho(bool storing, MatrixRm<float>& rho);
		/// <summary>
		/// RΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">R</param>
		void persistR(bool storing, MatrixRm<float>& r);
		/// <summary>
		/// BELΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">BEL</param>
		void persistBEL(bool storing, MatrixRm<float>& bel);
		/// <summary>
		/// WINΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">WIN</param>
		void persistWin(bool storing, MatrixRm<int32_t>& win);
		/// <summary>
		/// ͺ\Μi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">crosstab</param>
		void persistCrossTab(bool storing, MatrixRm<int32_t>& crosstab);
		/// <summary>
		/// ͺ\Μ·ͺΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">diff</param>
		void persistCrossTabDiff(bool storing, MatrixRm<int32_t>& diff);
		/// <summary>
		/// έξρΚΜi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">rli</param>
		void persistMi(bool storing, MatrixRm<float>& mi);

#pragma endregion
#pragma region Helper Method
	private:
		/// <summary>
		/// ͺ\©ηέξρΚπvZ
		/// </summary>
		void _updateMi()
		{
			_grid_matrix_t entryCt, entryMi;
			if (!_findGridMatrix(_STAT_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
			if (!_findGridMatrix(_STAT_MUTUAL_INFORMATION, entryMi)) throw MonjuException();

			MatrixRm<float> crosstab;
			MatrixRm<float> mi;
			crosstab.resize(_cross.matrix.rows, _cross.matrix.cols);
			mi.resize(_cross.grid.rows, _cross.grid.cols);

			// ObhΜzu@ΊOpsρ
			for (int grow = 0; grow < _cross.grid.rows; grow++)
			{
				for (int gcol = 0; gcol <= grow; gcol++)
				{
					_cell_data_t cellCt;
					_getCellData(entryCt, grow, gcol, cellCt);
					_readMatrixData(entryCt, cellCt, crosstab);
					auto fc = crosstab.cast<float>();
					fc.array() /= fc.sum();
					// όΣͺzP(x), P(y)
					auto cross = fc.rowwise().sum().col(0) * fc.colwise().sum().row(0);
					// έξρΚ
					mi(gcol, grow) = mi(grow, gcol) = (fc.array() * (fc.array() / cross.array()).log().array()).sum();
				}
			}
			_cell_data_t cellMi;
			_getCellData(entryMi, 0, 0, cellMi);
			_writeMatrixData(entryMi, cellMi, mi);
		}
		/// <summary>
		/// €}§yieBπͺ\©ηΔvZ
		/// </summary>
		void _updateRLI()
		{
			_grid_matrix_t entryCt, entryRli;
			if (!_findGridMatrix(_STAT_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
			if (!_findGridMatrix(_VAR_R_LI, entryRli)) throw MonjuException();

			MatrixRm<float> crosstab;
			MatrixRm<float> rli;
			crosstab.resize(_cross.matrix.rows, _cross.matrix.cols);
			auto flat = _shape.flatten();
			rli.resize(flat.rows, flat.cols);
			rli.setZero();
			// W C * s^2
			float coe = static_cast<float>(flat.cols);
			coe *= coe * _coefLateralInhibition;

			// ObhΜzu@ΊOpsρ
			for (int grow = 0; grow < _cross.grid.rows; grow++)
			{
				for (int gcol = 0; gcol < grow; gcol++)
				{
					_cell_data_t cellCt;
					_getCellData(entryCt, grow, gcol, cellCt);
					_readMatrixData(entryCt, cellCt, crosstab);
					auto fc = crosstab.cast<float>();
					fc.array() /= fc.sum();
					// όΣͺzP(x)P(y)
					auto cross = fc.rowwise().sum().col(0) * fc.colwise().sum().row(0);
					// P(x,y) / P(x)P(y)
					auto a = fc.array() / cross.array();
					// exp(°(- 1/t(x,y) * C * s^2 * P(x,y) / P(x)P(y) * log(P(x,y) / P(x)P(y)))
					auto b = coe * a.array() * a.array().log() / fc.array();

					rli.row(grow).array() -= b.rowwise().sum();
					rli.row(gcol).array() -= b.colwise().sum();
				}
			}
			// ΕI»
			rli = rli.array().exp();
			_cell_data_t cellRli;
			_getCellData(entryRli, 0, 0, cellRli);
			_writeMatrixData(entryRli, cellRli, rli);
		}
		void _prepareStorage();
		void _clearStorage();
		std::shared_ptr<std::vector<int32_t>> _toVector(MatrixRm<int32_t>& win) const
		{
			auto v = std::make_shared< std::vector<int32_t>>();
			for (int row = 0; row < win.rows(); row++)
				v->push_back(win(row, 0));
			return std::move(v);
		}

#pragma endregion

	};
}

#endif // !_MONJU_BEL_LAYER_STORAGE_H__
