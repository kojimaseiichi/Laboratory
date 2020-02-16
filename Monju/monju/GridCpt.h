#pragma once
#ifndef _MONJU_GRID_CPT_H__
#define _MONJU_GRID_CPT_H__

#include "MonjuTypes.h"
#include "util_eigen.h"

namespace monju {

	/// ��������̎�ނ̊w�K�A���S���Y��������
	/// �֐��������[��
	/// SOM�g�|���W�[_�w�K���[��
	class GridCpt
	{
	private:
		std::string _id;
		UniformBasisShape
			_shapeX,
			_shapeY;
		std::shared_ptr<MatrixCm<float_t>>
			_cpt,			// �O���b�hCPT�i�S�K�e���\���j
			_deltaCpt;		// �O���b�hCPT����
		const int _kCellSize;	// �Z���T�C�Y

	public:
		std::string id() const { return _id; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }
		std::weak_ptr<MatrixCm<float_t>> deltaCpt() { return _deltaCpt; }

	public:
		GridCpt(
			std::string id,
			UniformBasisShape shapeX,
			UniformBasisShape shapeY
		):
			_kCellSize(shapeX.units * shapeY.units)
		{
			_id = id;
			_shapeX = shapeX;
			_shapeY = shapeY;
			const int rows = _kCellSize * _shapeY.nodes;
			
			_cpt = std::make_shared<MatrixCm<float_t>>();
			_deltaCpt = std::make_shared<MatrixCm<float_t>>();

			_cpt->resize(rows, shapeX.nodes);
			_cpt->setZero();

			_deltaCpt->resize(rows, shapeX.nodes);
			_deltaCpt->setZero();
		}
		void store(std::string dir)
		{
			std::string extension = "mat";
			util_eigen::write_binary(dir, _id, "cpt", extension, *_cpt);
			util_eigen::write_binary(dir, _id, "delta-cpt", extension, *_deltaCpt);
		}
		void load(std::string dir)
		{
			const int rows = _kCellSize * _shapeY.nodes;
			std::string extension = "mat";
			if (util_eigen::read_binary(dir, _id, "cpt", extension, *_cpt) == false)
				util_eigen::init_matrix_zero(*_cpt, rows, _shapeX.nodes);
			if (util_eigen::read_binary(dir, _id, "delta-cpt", extension, *_deltaCpt) == false)
				util_eigen::init_matrix_zero(*_deltaCpt, rows, _shapeX.nodes);
		}
		void initRandom()
		{
			// CPT
			_cpt->setRandom();
			*_cpt = _cpt->array().abs();
			for (int col = 0; col < _shapeX.nodes; col++)
			{
				for (int row = 0; row < _shapeY.nodes; row++)
				{
					auto map = Eigen::Map<MatrixCm<float_t>>(_cpt->col(col).data() + row * static_cast<uintptr_t>(_kCellSize), _shapeY.nodes, _shapeX.units);
					map.array().rowwise() /= map.colwise().sum().array();
				}
			}
			// delta CPT
			_deltaCpt->setZero();
		}
		void winnerTakerAll(
			MatrixRm<int32_t>& winX,
			MatrixRm<int32_t>& winY,
			float learningRate
		)
		{
			/// �P����WTA
			/// ��ʊ��Ɖ��ʊ��̏��҃��j�b�g�ɑ΂��Ďw�肵���w�K���ő���
			/// �㉺�̏��҃��j�b�g��u,v�Ƃ����Ƃ��Ɏ��̂悤�ɍX�V
			/// w(v,u) = (1-a)w(v,u) + a
			
			for (int col = 0; col < _shapeX.nodes; col++)
			{
				int winUnitX = winX(col, 0);
				for (int row = 0; row < _shapeY.nodes; row++)
				{
					int winUnitY = winY(row, 0);
					auto map = Eigen::Map<MatrixCm<float_t>>(_cpt->col(col).data() + static_cast<int>(row * _kCellSize), _shapeY.units, _shapeX.units);
					auto mapDelta = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(col).data() + static_cast<int>(row * _kCellSize), _shapeY.units, _shapeX.units);
					mapDelta(winUnitY, winUnitX) += learningRate * (1 - map(winUnitY, winUnitX));
				}
			}
		}

		void winnerTakerAll(
			MatrixRm<int32_t>& winX,
			MatrixRm<int32_t>& winY,
			MatrixCm<float_t>& lambdaX,
			float learningRate
		)
		{
			// ���҃Z���̂݊w�K����WTA

			for (int col = 0; col < _shapeX.nodes; col++)
			{
				int winUnitX = winX(col, 0);
				auto mapLambda = lambdaX.col(col).reshaped(_shapeY.nodes, _shapeX.units);
				Eigen::Index maxRow;
				mapLambda.col(winUnitX).maxCoeff(&maxRow);
				{
					int winUnitY = winY(maxRow, 0);
					auto map = Eigen::Map<MatrixCm<float_t>>(_cpt->col(col).data() + static_cast<Eigen::Index>(maxRow * _kCellSize), _shapeY.units, _shapeX.units);
					auto mapDelta = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(col).data() + static_cast<Eigen::Index>(maxRow * _kCellSize), _shapeY.units, _shapeX.units);
					mapDelta(winUnitY, winUnitX) += learningRate * (1 - map(winUnitY, winUnitX));
				}
			}
		}

		void cellWta(
			MatrixRm<int32_t>& winX,
			MatrixRm<int32_t>& winY,
			MatrixCm<float_t>& lambda)
		{
			// ���ԑw��z�肵��WTA�ɂ��CPT�����ώZ
			// �Z�����Ƃ�

			for (int x = 0; x < _shapeX.nodes; x++)
			{
				// ��ʑw���猩�Ċw�K�ΏۂɂȂ�CPT�Z��������
				auto map = lambda.col(x).reshaped(_shapeY.nodes, _shapeX.units);
				MatrixCm<float_t>::Index row, col;
				map.col(winX(x, 0)).maxCoeff(&row, &col); // ��row�Ɋw�K�Ώۂ�CPT
				// �w�K�Ώۂ�row, x, winY(row), winX(x)
				int r = static_cast<int>(row);
				auto a = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(x).data() + static_cast<int>(_kCellSize * row), _shapeY.units, _shapeX.units);
				a(winY(r), winX(x)) += 1.f;
			}
		}

		void wtaDown(
			MatrixRm<int32_t>& winX,
			MatrixRm<float_t>& belY,
			MatrixCm<float_t>& lambda)
		{
			// ��ʑw�FWTA
			// ���ʑw�F�x�N�g��

			for (int x = 0; x < _shapeX.nodes; x++)
			{
				// ��ʑw���猩�Ċw�K�ΏۂɂȂ�CPT�Z��������
				auto map = lambda.col(x).reshaped(_shapeY.nodes, _shapeX.units);
				MatrixCm<float_t>::Index row, col;
				map.col(winX(x, 0)).maxCoeff(&row, &col); // ��row�Ɋw�K�Ώۂ�CPT
				// �w�K�Ώۂ�row, x, winY(row), winX(x)
				int r = static_cast<int>(row);
				auto a = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(x).data() + static_cast<int>(_kCellSize * row), _shapeY.units, _shapeX.units);
				a.col(winX(x)).array() += belY.row(r).array();
			}
		}

		void addDelta()
		{
			// CPT�̑����𑫂�

			_cpt->array() += _deltaCpt->array();
			_deltaCpt->setZero();
		}
		// �R�s�[�֎~�E���[�u�֎~
	public:
		GridCpt(const GridCpt&) = delete;
		GridCpt(GridCpt&&) = delete;
		GridCpt& operator =(const GridCpt&) = delete;
		GridCpt& operator =(GridCpt&&) = delete;
	};

}

#endif // !_MONJU_GRID_CPT_H__
