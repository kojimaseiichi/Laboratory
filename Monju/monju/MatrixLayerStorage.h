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
		/// �����𕪊��\�ɔ��f����CPT���Čv�Z(�����g��Ȃ�)
		/// </summary>
		/// <returns></returns>
		std::future<void> asyncUpdateCpt();
		/// <summary>
		/// �����\(CROSSTAB)��������t�m���\(CPT)���v�Z<br>
		/// �Ӓl�m�[�h�͍l�����Ȃ��B�ʏ��updateCpt��CPT���X�V���Ă������߁A�����\���璼�ڌv�Z�������ʂƈ�v���Ȃ��B
		/// </summary>
		/// <param name="cpt"></param>
		void calcCpt(std::weak_ptr<MatrixCm<float>> cpt);
		/// <summary>
		/// CPT���瑊�ݏ��ʂ��v�Z����B
		/// ���ݏ��ʂ���m�[�h�Ԃ�
		/// </summary>
		std::future<void> asyncCalcMi();
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
		void _updateContingencyTableJoiningDiff();
		std::function<int(int, int, int)> _cptTopologyTorus();
		void _updateContingencyTableAndCpt(const std::function<float(int, int, int)> cptTopology);
		void _updateMi();
		std::vector<int32_t>&& _toVector(MatrixRm<int32_t>& win) const;
#pragma endregion
	};
}

#endif // !_MONJU_MATRIX_LAYER_STORAGE_H__

