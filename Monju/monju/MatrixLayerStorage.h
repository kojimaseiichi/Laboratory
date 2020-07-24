#pragma once
#ifndef _MONJU_MATRIX_LAYER_STORAGE_H__
#define _MONJU_MATRIX_LAYER_STORAGE_H__

#include <vector>
#include "Closable.h"
#include "Environment.h"
#include "ConcurrencyContext.h"
#include "GridMatrixStorage.h"
#include "ConcurrencyContext.h"
#include "Synchronizable.h"
#include "Extent.h"
#include "util_file.h"
#include "util_ai.h"

namespace monju {

	/// <summary>
	/// ��̑w�Ԃ̏����t�m���\(CPT)�A�����\(CT)����т��̑��ϐ����t�@�C���ɕێ�����B
	/// �t�@�C�����o�͂Ɋ֌W����w�K�A���S���Y���̈ꕔ����������B
	/// </summary>
	class MatrixLayerStorage : public GridMatrixStorage
	{
#pragma region Private Field
	private:
		ConcurrencyContext _conc;		// �������s�R���e�L�X�g
		Synchronizable _syn;			// �����I�u�W�F�N�g
		GridExtent _gridExtent;			// CPT�`��
		GridExtent _gridExtentLambda;	// �Ɍ`��
		GridExtent _gridExtentKappa;	// �Ȍ`��
#pragma endregion
#pragma region Private Constant
	private:
		const std::string _CONTINGENCY_TABLE = "ct";				// �����\ �S�K�e���\�� int32_t
		const std::string _INCREMENTAL_DIFF= "df";					// �����\�̍��� �S�K�e���\�� int32_t
		const std::string _MUTUAL_INFORMATION = "mi";				// �m�[�h�ԑ��ݏ��� �s�� float
		const std::string _CONDITIONAL_PROBABILITY_TABLE = "cpt";	// �����t�m���\ �S�K�e���\�� float
		const std::string _LAMBDA_VARIABLES = "lambda";				// �� �R�K�e���\��
		const std::string _KAPPA_VARIABLES = "kappa";				// �� �R�K�e���\��
#pragma endregion
#pragma region Constructor
	public:
		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		/// <param name="fileName">�f�[�^�t�@�C����</param>
		/// <param name="x">��ʃ��C���[�`��</param>
		/// <param name="y">���ʃ��C���[�`��</param>
		/// <returns></returns>
		MatrixLayerStorage(std::string fileName, LayerShape x, LayerShape y);
		/// <summary>
		/// �f�X�g���N�^
		/// </summary>
		~MatrixLayerStorage();
#pragma endregion
#pragma region Persistent
	public:
		/// <summary>
		/// �Ƀf�[�^�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">��</param>
		void persistLambda(bool storing, MatrixCm<float>& lambda);
		/// <summary>
		/// �ȃf�[�^�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">��</param>
		void persistKappa(bool storing, MatrixCm<float>& kappa);
		/// <summary>
		/// �����\�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">��</param>
		void persistCrossTab(bool storing, MatrixCm<int32_t>& crosstab);
		/// <summary>
		/// �����\�̍����̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">��</param>
		void persistCrossTabDiff(bool storing, MatrixCm<int32_t>& diff);
		/// <summary>
		/// ���ݏ��ʂ̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">��</param>
		void persistMi(bool storing, MatrixCm<float>& mi);
		/// <summary>
		/// CPT�f�[�^�̉i�����^����
		/// </summary>
		/// <param name="storing">true:�i���� | false:����</param>
		/// <param name="lambda">CPT</param>
		void persistCpt(bool storing, MatrixCm<float>& cpt);
#pragma endregion
#pragma region Public Method
		/// <summary>
		/// �f�[�^�t�@�C���ɕK�v�ȃG���g�����쐬
		/// </summary>
		void prepareAll()
		{
			_prepareStorage();
		}
		/// <summary>
		/// �����\�̍��������C���[�Ԃ̏��҃��j�b�g�ŉ��Z
		/// </summary>
		/// <param name="xWinner"></param>
		/// <param name="yWinner"></param>
		/// <returns></returns>
		std::future<void> asyncIncrementDiff(std::weak_ptr<MatrixRm<int32_t>> xWinner, std::weak_ptr<MatrixRm<int32_t>> yWinner);
		/// <summary>
		/// �����𕪊��\�ɔ��f���čX�V
		/// </summary>
		/// <returns></returns>
		std::future<void> asyncUpdateContingencyTable();
		/// <summary>
		/// �����𕪊��\�ɔ��f����CPT���Čv�Z
		/// </summary>
		/// <returns></returns>
		std::future<void> asyncUpdateCpt();
		/// <summary>
		/// �����\(CROSSTAB)��������t�m���\(CPT)���v�Z<br>
		/// �Ӓl�m�[�h�͍l�����Ȃ��B�ʏ��updateCpt��CPT���X�V���Ă������߁A�����\���璼�ڌv�Z�������ʂƈ�v���Ȃ��B
		/// </summary>
		/// <param name="cpt"></param>
		void calcCpt(std::weak_ptr<MatrixCm<float>> cpt)
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
		/// CPT���瑊�ݏ��ʂ��v�Z����B
		/// ���ݏ��ʂ���m�[�h�Ԃ�
		/// </summary>
		std::future<void> asyncCalcMi()
		{
			auto future = _conc.threadPool().submit([&] {
				_updateMi();
				});
		}
#pragma endregion
#pragma region Helper
	private:/*�w���p*/
		void _prepareStorage()
		{
			this->prepare<int32_t>(_CONTINGENCY_TABLE, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<int32_t>(_INCREMENTAL_DIFF, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float>(_MUTUAL_INFORMATION, _gridExtent.matrix, kColMajor);
			this->prepare<float>(_CONDITIONAL_PROBABILITY_TABLE, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float>(_LAMBDA_VARIABLES, _gridExtentLambda, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float>(_KAPPA_VARIABLES, _gridExtentKappa, kDensityRectangular, kColMajor, kColMajor);
		}
		void _updateContingencyTableJoiningDiff()
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
		std::function<int(int, int, int)> _cptTopologyTorus()
		{
			return [](int win, int col, int cycle) {
				if (win > col)
					std::swap(win, col);
				return std::min(col - win, (win + cycle) - col);
			};
		}
		void _updateContingencyTableAndCpt(const std::function<float(int, int, int)> cptTopology)
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
			// ���[�v(�O���b�h����)
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
					// ���[�v(�}�g���b�N�X����)
					// �J�E���g���ꂽ�v�f��CPT�ɔ��f
					int cycle = static_cast<int>(matShape.cols);
					for (auto col = 0; col < matShape.cols; col++)
					{
						for (auto row = 0; row < matShape.rows; row++)
						{
							auto count = dif(row, col);
							if (count == 0) continue;
							// �������l�̌W����������K�v������(�������������Ċw�K���𒲐�)
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
		void _updateMi()
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
					// ���ӕ��zP(x), P(y)
					auto cross = cpt.rowwise().sum().col(0) * cpt.colwise().sum().row(0);
					// ���ݏ���
					mi(gcol, grow) = (cpt.array()* (cpt.array() / cross.array()).log().array()).sum();
				}
			}
			_cell_data_t cellMi;
			_getCellData(entryMi, 0, 0, cellMi);
			_writeMatrixData(entryMi, cellMi, mi);
		}
		std::vector<int32_t>&& _toVector(MatrixRm<int32_t>& win) const
		{
			std::vector<int32_t> v;
			for (int row = 0; row < win.rows(); row++)
				v.push_back(win(row, 0));
			return std::move(v);
		}
#pragma endregion
	};
}

#endif // !_MONJU_MATRIX_LAYER_STORAGE_H__

