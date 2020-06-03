#pragma once
#ifndef _MONJU_IMAGE_INPUT_GRID_CPT_H__
#define _MONJU_IMAGE_INPUT_GRID_CPT_H__

#include "MonjuTypes.h"
#include "util_eigen.h"
#include "Extent.h"

namespace monju
{
	class ImageInputGridCpt
	{
	private:
		std::string _id;
		LayerShape
			_shapeX,
			_shapeY;
		std::shared_ptr<MatrixCm<float_t>>
			_cpt,			// グリッドCPT（４階テンソル）
			_deltaCpt;		// グリッドCPT増分
		const size_t _kCellSize;	// セルサイズ

	public:
		std::string id() const { return _id; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }
		std::weak_ptr<MatrixCm<float_t>> deltaCpt() { return _deltaCpt; }
		LayerShape shapeX() const { return _shapeX; }
		LayerShape shapeY() const { return _shapeY; }

	public:
		ImageInputGridCpt(
			std::string id,
			LayerShape shapeX,
			LayerShape shapeY
		) :
			_kCellSize(shapeX.units.size() * shapeY.units.size())
		{
			_id = id;
			_shapeX = shapeX;
			_shapeY = shapeY;
			const size_t rows = _kCellSize * 1;

			_cpt = std::make_shared<MatrixCm<float_t>>();
			_deltaCpt = std::make_shared<MatrixCm<float_t>>();

			_cpt->resize(rows, shapeX.nodes.size());
			_cpt->setZero();

			_deltaCpt->resize(rows, shapeX.nodes.size());
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
			const size_t rows = _kCellSize * 1;
			std::string extension = "mat";
			util_eigen::restore_binary(dir, _id, "cpt", extension, *_cpt, rows, _shapeX.nodes.size());
			util_eigen::restore_binary(dir, _id, "delta-cpt", extension, *_deltaCpt, rows, _shapeX.nodes.size());
		}
		void initRandom()
		{
			// CPT
			_cpt->setRandom();
			*_cpt = _cpt->array().abs();
			for (int col = 0; col < _shapeX.nodes.size(); col++)
			{
				for (int row = 0; row < _shapeY.nodes.size(); row++)
				{
					auto mm = _cpt->block(row * _kCellSize, col, _kCellSize, 1).reshaped(_shapeY.units.size(), _shapeX.units.size());
					mm.array().rowwise() /= mm.colwise().sum().array();
				}
			}
			// delta CPT
			_deltaCpt->setZero();
		}
		
		void addDelta()
		{
			// CPTの増分を足す

			_cpt->array() += _deltaCpt->array();
			_deltaCpt->setZero();
		}
		// コピー禁止・ムーブ禁止
	public:
		ImageInputGridCpt(const ImageInputGridCpt&) = delete;
		ImageInputGridCpt(ImageInputGridCpt&&) = delete;
		ImageInputGridCpt& operator =(const ImageInputGridCpt&) = delete;
		ImageInputGridCpt& operator =(ImageInputGridCpt&&) = delete;
	};
}

#endif // !_MONJU_IMAGE_INPUT_GRID_CPT_H__
