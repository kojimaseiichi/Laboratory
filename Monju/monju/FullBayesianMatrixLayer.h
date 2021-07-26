#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__

#include <string>
#include "eigentypes.h"
#include "Synchronizable.h"
#include "VariableKind.h"
#include "Extent.h"

namespace monju {

	class Environment;
	class MatrixLayerStorage;
	class BelLayer;
	class GridMatrixStorage;

	// ���Ԃ̃f�[�^���t�B���j�ۑ�
	// �ێ�����f�[�^
	// �E�x�C�Y�v�Z�I�ȕϐ��iCPT/OOBP�j
	class FullBayesianMatrixLayer : public Synchronizable
	{
	protected:
		std::shared_ptr<Environment> _env;
		std::string _id;
		LayerShape _shapeX, _shapeY;
		GridExtent _gridExtent;
		std::shared_ptr<MatrixCm<float>>
			_lambda,				// �Ɂi�m�[�h��Y * ���j�b�g��Y, �m�[�h��X�j
			_kappa,					// �ȁi�m�[�h��Y * ���j�b�g��X, �m�[�h��X�j
			_cpt;					// �d�ݍs��i���j�b�g��Y * ���j�b�g��X * �m�[�h��Y, �m�[�h��X�j
		std::unique_ptr<MatrixLayerStorage> _storage;


	public: // �v���p�e�B
		std::string id() const { return _id; }
		LayerShape shapeX() const { return _shapeX; }
		LayerShape shapeY() const { return _shapeY; }
		std::weak_ptr<MatrixCm<float>> lambda() { return _lambda; }
		std::weak_ptr<MatrixCm<float>> kappa() { return _kappa; }
		std::weak_ptr<MatrixCm<float>> cpt() { return _cpt; }

	public:
		FullBayesianMatrixLayer(
			std::weak_ptr<Environment> env,
			std::string id,
			LayerShape shapeX,
			LayerShape shapeY
		);
		~FullBayesianMatrixLayer();
		void initVariables();
		void store();
		void store(std::string path);
		void load();
		void load(std::string path);
		bool containsNan() const;
		void copyData(const FullBayesianMatrixLayer& o);
		void performUp(BelLayer& x, BelLayer& y);
		void performDown(BelLayer& x, BelLayer& y);

	protected:
		std::string _dataFileName() const;
		void _makeGridExtent(GridExtent& cpt, GridExtent& lambda, GridExtent& kappa);
		void _initHostMemory();
		void _prepareStorage(GridMatrixStorage& storage, GridExtent cptExtent, GridExtent lambdaExtent, GridExtent kappaExtent);
		void _setRandom();
		void _storeMatrices(std::string& dir);
		void _loadMatrices(std::string& dir);

		// �R�s�[�֎~�E���[�u�֎~
	public:
		FullBayesianMatrixLayer(const FullBayesianMatrixLayer&) = delete;
		FullBayesianMatrixLayer(FullBayesianMatrixLayer&&) = delete;
		FullBayesianMatrixLayer& operator =(const FullBayesianMatrixLayer&) = delete;
		FullBayesianMatrixLayer& operator =(FullBayesianMatrixLayer&&) = delete;
	};
}

#endif // !_MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
