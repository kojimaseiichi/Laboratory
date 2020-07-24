#pragma once
#ifndef _MONJU_BEL_LAYER_STORAGE_H__
#define _MONJU_BEL_LAYER_STORAGE_H__

#include "Extent.h"
#include "GridMatrixStorage.h"
#include "ConcurrencyContext.h"
#include "Synchronizable.h"

namespace monju
{
	class BelLayerStorage : public GridMatrixStorage
	{
#pragma region Private Field
	private:
		ConcurrencyContext _conc;		// ¯ÀsReLXg
		Synchronizable _syn;			// ¯úIuWFNg
		LayerShape _shape;
		GridExtent _cross;
		/// <summary>
		/// /¤}§ÌW
		/// </summary>
		float _coefLateralInhibition;
#pragma endregion
#pragma region Private Constant
	private:
		const std::string _VAR_LAMBDA = "lambda";					// É(Þx)
		const std::string _VAR_PI = "pi";							// Î(Oªz)
		const std::string _VAR_RHO = "rho";							// Ï(ãªz)
		const std::string _VAR_R = "r";								// yieB
		const std::string _VAR_R_LI = "r_li";						// ¤}§yieB(Win-Rate)
		const std::string _VAR_R_WR = "r_wr";						// ¦yieB(Lateral Inhibition)
		const std::string _VAR_BEL = "bel";							// BELIEVE
		const std::string _VAR_WIN = "win";							// ¿jbg
		const std::string _CONTINGENCY_TABLE = "ct";				// ª\ SKe\ int32_t
		const std::string _INCREMENTAL_DIFF = "df";					// ª\Ì·ª SKe\ int32_t
		const std::string _MUTUAL_INFORMATION = "mi";				// wàÌm[hÔÝîñÊ sñ float
		const std::string _COE_LATERAL_INHIBITION = "cli";			// ¤}§yieBÌW
#pragma endregion
#pragma region Constructor
	public:
		BelLayerStorage(std::string fileName, LayerShape shape);
		~BelLayerStorage();
#pragma endregion
#pragma region Public Properties
		float getCoefLateralInhibition()
		{
			return _coefLateralInhibition;
		}
		void setCoefLateralInhibition(float v)
		{
			_coefLateralInhibition = v;
			this->setKey<float>(_COE_LATERAL_INHIBITION, v);
		}
#pragma endregion
#pragma region Public Method
	public:
		/// <summary>
		/// Xg[WÌgpõ
		/// </summary>
		void prepareAll();
		void clearAll()
		{
			_clearStorage();
		}
		/// <summary>
		/// Òjbgðª\É½f
		/// </summary>
		/// <param name="winner"></param>
		/// <returns></returns>
		std::future<void> asyncIncrementDiff(std::weak_ptr<MatrixRm<int32_t>> winner)
		{
			// ñ¯úÌõÆµÄÒjbgÌf[^ðRs[
			auto px = winner.lock();
			auto a = _toVector(*px);
			auto wrapper = [&] (std::shared_ptr<std::vector<int32_t>> p){
				// Lv`[Fax, ay
				this->coeffOp<int32_t>(
					_INCREMENTAL_DIFF,
					*p,
					*p,
					[](const int32_t v)->int32_t {
						return v + 1;
					});
			};
			auto f = _conc.threadPool().submit(wrapper, std::move(a));
			return f;
		}
#pragma endregion
#pragma region Persistent
	public:
		/// <summary>
		/// ÉÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">lambda</param>
		void persistLambda(bool storing, MatrixRm<float>& lambda);
		/// <summary>
		/// ÎÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">pi</param>
		void persistPi(bool storing, MatrixRm<float>& pi);
		/// <summary>
		/// ÏÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">rho</param>
		void persistRho(bool storing, MatrixRm<float>& rho);
		/// <summary>
		/// RÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">R</param>
		void persistR(bool storing, MatrixRm<float>& r);
		/// <summary>
		/// BELÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">BEL</param>
		void persistBEL(bool storing, MatrixRm<float>& bel);
		/// <summary>
		/// WINÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">WIN</param>
		void persistWin(bool storing, MatrixRm<int32_t>& win);
		/// <summary>
		/// ª\Ìi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">crosstab</param>
		void persistCrossTab(bool storing, MatrixRm<int32_t>& crosstab);
		/// <summary>
		/// ª\Ì·ªÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">diff</param>
		void persistCrossTabDiff(bool storing, MatrixRm<int32_t>& diff);
		/// <summary>
		/// ÝîñÊÌi±»^³
		/// </summary>
		/// <param name="storing">true:i±» | false:³</param>
		/// <param name="lambda">rli</param>
		void persistMi(bool storing, MatrixRm<float>& mi);

#pragma endregion
#pragma region Helper Method
	private:
		/// <summary>
		/// ª\©çÝîñÊðvZ
		/// </summary>
		void _updateMi()
		{
			_grid_matrix_t entryCt, entryMi;
			if (!_findGridMatrix(_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
			if (!_findGridMatrix(_MUTUAL_INFORMATION, entryMi)) throw MonjuException();

			MatrixRm<float> crosstab;
			MatrixRm<float> mi;
			crosstab.resize(_cross.matrix.rows, _cross.matrix.cols);
			mi.resize(_cross.grid.rows, _cross.grid.cols);

			// ObhÌzu@ºOpsñ
			for (int grow = 0; grow < _cross.grid.rows; grow++)
			{
				for (int gcol = 0; gcol <= grow; gcol++)
				{
					_cell_data_t cellCt;
					_getCellData(entryCt, grow, gcol, cellCt);
					_readMatrixData(entryCt, cellCt, crosstab);
					auto fc = crosstab.cast<float>();
					fc.array() /= fc.sum();
					// üÓªzP(x), P(y)
					auto cross = fc.rowwise().sum().col(0) * fc.colwise().sum().row(0);
					// ÝîñÊ
					mi(gcol, grow) = mi(grow, gcol) = (fc.array() * (fc.array() / cross.array()).log().array()).sum();
				}
			}
			_cell_data_t cellMi;
			_getCellData(entryMi, 0, 0, cellMi);
			_writeMatrixData(entryMi, cellMi, mi);
		}
		/// <summary>
		/// ¤}§yieBðª\©çÄvZ
		/// </summary>
		void _updateRLI()
		{
			_grid_matrix_t entryCt, entryRli;
			if (!_findGridMatrix(_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
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

			// ObhÌzu@ºOpsñ
			for (int grow = 0; grow < _cross.grid.rows; grow++)
			{
				for (int gcol = 0; gcol < grow; gcol++)
				{
					_cell_data_t cellCt;
					_getCellData(entryCt, grow, gcol, cellCt);
					_readMatrixData(entryCt, cellCt, crosstab);
					auto fc = crosstab.cast<float>();
					fc.array() /= fc.sum();
					// üÓªzP(x)P(y)
					auto cross = fc.rowwise().sum().col(0) * fc.colwise().sum().row(0);
					// P(x,y) / P(x)P(y)
					auto a = fc.array() / cross.array();
					// exp(°(- 1/t(x,y) * C * s^2 * P(x,y) / P(x)P(y) * log(P(x,y) / P(x)P(y)))
					auto b = coe * a.array() * a.array().log() / fc.array();

					rli.row(grow).array() -= b.rowwise().sum();
					rli.row(gcol).array() -= b.colwise().sum();
				}
			}
			// ÅI»
			rli = rli.array().exp();
			_cell_data_t cellRli;
			_getCellData(entryRli, 0, 0, cellRli);
			_writeMatrixData(entryRli, cellRli, rli);
		}
		void _prepareStorage()
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
			this->prepare<int32_t>(_CONTINGENCY_TABLE, _cross, kDensityLowerTriangular, kRowMajor, kRowMajor);
			this->prepare<int32_t>(_INCREMENTAL_DIFF, _cross, kDensityLowerTriangular, kRowMajor, kRowMajor);
			this->prepare<float>(_MUTUAL_INFORMATION, _cross.grid, kRowMajor);
		}
		void _clearStorage()
		{
			this->clearGrid(_VAR_LAMBDA);
			this->clearGrid(_VAR_PI);
			this->clearGrid(_VAR_RHO);
			this->clearGrid(_VAR_R);
			this->clearGrid(_VAR_R_LI);
			this->clearGrid(_VAR_R_WR);
			this->clearGrid(_VAR_BEL);
			this->clearGrid(_VAR_WIN);
			this->clearGrid(_CONTINGENCY_TABLE);
			this->clearGrid(_INCREMENTAL_DIFF);
			this->clearGrid(_MUTUAL_INFORMATION);
		}
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
