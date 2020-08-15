#pragma once
#ifndef _MONJU_BEL_LAYER_STORAGE_H__
#define _MONJU_BEL_LAYER_STORAGE_H__

#include "Extent.h"
#include "GridMatrixStorage.h"
#include "ConcurrencyContext.h"
#include "Synchronizable.h"

namespace monju
{
	/// <summary>
	/// BEL���C���[�̉i�����f�[�^
	/// BelLayer�N���X�ŕێ�����f�[�^���i�����B
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
		float _coefLateralInhibition;	// ���}���̌W��
#pragma endregion
#pragma region Storage Key
	private:
		// ���f���f�[�^
		const std::string _VAR_LAMBDA = "lambda";					// ��(�ޓx)
		const std::string _VAR_PI = "pi";							// ��(���O���z)
		const std::string _VAR_RHO = "rho";							// ��(���㕪�z)
		const std::string _VAR_R = "r";								// �y�i���e�B
		const std::string _VAR_R_LI = "r_li";						// ���}���y�i���e�B(Win-Rate)
		const std::string _VAR_R_WR = "r_wr";						// �����y�i���e�B(Lateral Inhibition)
		const std::string _VAR_BEL = "bel";							// BELIEVE
		const std::string _VAR_WIN = "win";							// �������j�b�g
		// ���v��
		const std::string _STAT_CONTINGENCY_TABLE = "ct";			// �����\ �S�K�e���\�� int32_t
		const std::string _STAT_INCREMENTAL_DIFF = "df";			// �����\�̍��� �S�K�e���\�� int32_t
		const std::string _STAT_MUTUAL_INFORMATION = "mi";			// �w���̃m�[�h�ԑ��ݏ��� �s�� float
		// Key-Value
		const std::string _KV_COE_LATERAL_INHIBITION = "cli";		// ���}���y�i���e�B�̌W��
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
			this->setKey<float>(_KV_COE_LATERAL_INHIBITION, v);
		}
#pragma endregion
#pragma region Public Method
	public:
		/// <summary>
		/// �X�g���[�W�̎g�p����
		/// </summary>
		void prepareAll();
		void clearAll();
		/// <summary>
		/// ���҃��j�b�g�𕪊��\�ɔ��f
		/// </summary>
		/// <param name="winner"></param>
		/// <returns></returns>
		std::future<void> asyncIncrementDiff(std::weak_ptr<MatrixRm<int32_t>> winner)
		{
			// �񓯊������̏����Ƃ��ď��҃��j�b�g�̃f�[�^���R�s�[
			auto px = winner.lock();
			auto a = _toVector(*px);
			auto wrapper = [&] (std::shared_ptr<std::vector<int32_t>> p){
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
		/// <summary>
		/// �����\���瑊�ݏ��ʂ��v�Z
		/// </summary>
		/// <returns></returns>
		std::future<void> asyncUpdateMi()
		{
			auto wrapper = [&]() {
				this->_updateMi();
			};
			auto f = _conc.threadPool().submit(wrapper);
			return f;
		}
		/// <summary>
		/// �����\���瑤�}���y�i���e�B���v�Z
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
		/// �ɂ̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">lambda</param>
		void persistLambda(bool storing, MatrixRm<float>& lambda);
		/// <summary>
		/// �΂̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">pi</param>
		void persistPi(bool storing, MatrixRm<float>& pi);
		/// <summary>
		/// �ς̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">rho</param>
		void persistRho(bool storing, MatrixRm<float>& rho);
		/// <summary>
		/// R�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">R</param>
		void persistR(bool storing, MatrixRm<float>& r);
		/// <summary>
		/// BEL�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">BEL</param>
		void persistBEL(bool storing, MatrixRm<float>& bel);
		/// <summary>
		/// WIN�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">WIN</param>
		void persistWin(bool storing, MatrixRm<int32_t>& win);
		/// <summary>
		/// �����\�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">crosstab</param>
		void persistCrossTab(bool storing, MatrixRm<int32_t>& crosstab);
		/// <summary>
		/// �����\�̍����̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">diff</param>
		void persistCrossTabDiff(bool storing, MatrixRm<int32_t>& diff);
		/// <summary>
		/// ���ݏ��ʂ̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">rli</param>
		void persistMi(bool storing, MatrixRm<float>& mi);

#pragma endregion
#pragma region Helper Method
	private:
		/// <summary>
		/// �����\���瑊�ݏ��ʂ��v�Z
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

			// �O���b�h�̔z�u�@���O�p�s��
			for (int grow = 0; grow < _cross.grid.rows; grow++)
			{
				for (int gcol = 0; gcol <= grow; gcol++)
				{
					_cell_data_t cellCt;
					_getCellData(entryCt, grow, gcol, cellCt);
					_readMatrixData(entryCt, cellCt, crosstab);
					auto fc = crosstab.cast<float>();
					fc.array() /= fc.sum();
					// ���ӕ��zP(x), P(y)
					auto cross = fc.rowwise().sum().col(0) * fc.colwise().sum().row(0);
					// ���ݏ���
					mi(gcol, grow) = mi(grow, gcol) = (fc.array() * (fc.array() / cross.array()).log().array()).sum();
				}
			}
			_cell_data_t cellMi;
			_getCellData(entryMi, 0, 0, cellMi);
			_writeMatrixData(entryMi, cellMi, mi);
		}
		/// <summary>
		/// ���}���y�i���e�B�𕪊��\����Čv�Z
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
			// �W�� C * s^2
			float coe = static_cast<float>(flat.cols);
			coe *= coe * _coefLateralInhibition;

			// �O���b�h�̔z�u�@���O�p�s��
			for (int grow = 0; grow < _cross.grid.rows; grow++)
			{
				for (int gcol = 0; gcol < grow; gcol++)
				{
					_cell_data_t cellCt;
					_getCellData(entryCt, grow, gcol, cellCt);
					_readMatrixData(entryCt, cellCt, crosstab);
					auto fc = crosstab.cast<float>();
					fc.array() /= fc.sum();
					// ���ӕ��zP(x)P(y)
					auto cross = fc.rowwise().sum().col(0) * fc.colwise().sum().row(0);
					// P(x,y) / P(x)P(y)
					auto a = fc.array() / cross.array();
					// exp(��(- 1/t(x,y) * C * s^2 * P(x,y) / P(x)P(y) * log(P(x,y) / P(x)P(y)))
					auto b = coe * a.array() * a.array().log() / fc.array();

					rli.row(grow).array() -= b.rowwise().sum();
					rli.row(gcol).array() -= b.colwise().sum();
				}
			}
			// �ŏI��
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
