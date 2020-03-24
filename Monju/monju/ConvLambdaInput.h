#pragma once
#ifndef _MONJU_CONV_LAMBDA_INPUT_H__
#define _MONJU_CONV_LAMBDA_INPUT_H__

#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "util_math.h"
#include "DeviceMemory.h"
#include "ConvLambdaInputDevice.h"

namespace monju
{
	class ConvLambdaInput
	{
	private:
		std::string _id;
		UniformBasisShape _shapeBottom;
		Extent _extImage, _extFilter, _extStride;
		std::shared_ptr<MatrixRm<float_t>> _image;
		std::shared_ptr<MatrixCm<float_t>> _cpt;
		std::weak_ptr<MatrixRm<float_t>> _lambda;
		std::shared_ptr<MatrixCm<float_t>> _delta; // internal

	public:
		std::string id() const { return _id; }
		UniformBasisShape shape() const { return _shapeBottom; }
		Extent extImage() const { return _extImage; }
		Extent extFilter() const { return _extFilter; }
		Extent extStride() const { return _extStride; }
		std::weak_ptr<MatrixRm<float_t>> image() const { return _image; }
		std::weak_ptr<MatrixCm<float_t>> cpt() const { return _cpt; }
		std::weak_ptr<MatrixRm<float_t>> lambda() const { return _lambda; }

	public:
		ConvLambdaInput(
			std::string id,
			UniformBasisShape shapeBottom,
			std::weak_ptr<MatrixRm<float_t>> lambda,
			Extent extImage,
			Extent extFilter)
		{
			_id = id;
			_shapeBottom = shapeBottom;
			_lambda = lambda;
			_extImage = extImage;
			_extFilter = extFilter;

			_image = std::make_shared<MatrixRm<float_t>>();
			_image->resize(extImage.rows, extImage.cols);

			_cpt = std::make_shared<MatrixCm<float_t>>();
			_cpt->resize(_extFilter.size(), _shapeBottom.units);

			_delta = std::make_shared<MatrixCm<float_t>>();
			_delta->resizeLike(*_cpt);
			_delta->setZero();
			
			// stride計算
			_extStride = calcStride(_extImage, _shapeBottom.extent, _extFilter);

			// 形状チェック
			if (!_shapeBottom.checkExtent())
				throw MonjuException("invalid shapeBottom");
		}
		~ConvLambdaInput()
		{

		}
		void initRandom()
		{
			_cpt->setRandom();
			_cpt->array().rowwise() /= _cpt->array().colwise().sum();
		}
		void store(std::string dir)
		{
			std::string extension = "mat2";
			util_eigen::write_binary(dir, _id, "lambda", extension, *_cpt);
		}
		void load(std::string dir)
		{
			std::string extension = "mat2";
			if (util_eigen::read_binary(dir, _id, "lambda", extension, *_cpt) == false)
				util_eigen::init_matrix_zero(*_cpt, _extFilter.rows, _extFilter.cols);
		}
		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixCm<float_t>>(_cpt);
			return a;
		}
		void up()
		{
			Extent extBottom = _shapeBottom.extent;
			auto pLam = _lambda.lock();
			for (size_t row = 0; row < extBottom.rows; row++)
			{
				for (size_t col = 0; col < extBottom.cols; col++)
				{
					auto a = _image->block(row * _extStride.rows, col * _extStride.cols, _extFilter.rows, _extFilter.cols)
						.reshaped(1, _extFilter.size()).matrix();	// (1, filter size)
					auto b = a * (*_cpt);	// (1, units)
					auto linearId = extBottom.linearId(row, col);
					pLam->block(linearId, 0, 1, _shapeBottom.units) = b;
				}
			}
		}
		void accumulateCptIncrement(std::weak_ptr<MatrixRm<int32_t>>& win, float lr, float c0, float c1)
		{
			auto pw = win.lock();
			if (pw->size() != _shapeBottom.nodes)
				throw MonjuException("unexpected size of win matrix");
			auto shapedWin = pw->reshaped(_shapeBottom.extent.rows, _shapeBottom.extent.cols);
			// smoothstep関数のパラメータ
			const float edge0 = lr * c0;
			const float edge1 = lr * c1 + 1;
			// フィルターのサイズ
			const int sizeFilter = _extFilter.size();
			for (size_t row = 0; row < _shapeBottom.extent.rows; row++)
			{
				for (size_t col = 0; col < _shapeBottom.extent.cols; col++)
				{
					int winUnit = shapedWin(row, col);
					auto inputVec = _image->block(row, col, _extFilter.rows, _extFilter.cols)
						.reshaped(sizeFilter, 1);
					for (int unit = 0; unit < _shapeBottom.units; unit++)
					{
						float distance = static_cast<float>(std::abs(winUnit - unit));	// トーラスSOM
						float neighbour = 1 - util_math::smoothstep3(edge0, edge1, distance);	// 近傍関数
						if (neighbour == 0.f)
							continue;
						_delta->col(unit).array() += neighbour * lr * (inputVec.array() - _cpt->col(unit).array());
					}
				}
			}
		}
		void updateCpt()
		{
			// cpt += delta
			_cpt->array() += _delta->array();
			_delta->setZero();
		}
	private:
		Extent bottomExtent(Extent extImage, Extent extFilter, Extent stride)
		{
			const int32_t rows = (static_cast<size_t>(extImage.rows) - extFilter.rows) / stride.rows + 1;
			const int32_t cols = (static_cast<size_t>(extImage.cols) - extFilter.cols) / stride.cols + 1;
			return { rows, cols };
		}
		Extent calcStride(Extent image, Extent bottom, Extent filter)
		{
			const int32_t strideRow = (image.rows - filter.rows) / (bottom.rows - 1);
			const int32_t strideCol = (image.cols - filter.cols) / (bottom.cols - 1);
			return { strideRow, strideCol };
		}
		// コピー禁止・ムーブ禁止
	public:
		ConvLambdaInput(const ConvLambdaInput&) = delete;
		ConvLambdaInput(ConvLambdaInput&&) = delete;
		ConvLambdaInput& operator =(const ConvLambdaInput&) = delete;
		ConvLambdaInput& operator =(ConvLambdaInput&&) = delete;
	};
}

#endif // _MONJU_CONV_LAMBDA_INPUT_H__