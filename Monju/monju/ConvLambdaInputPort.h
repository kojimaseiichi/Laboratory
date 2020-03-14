#pragma once
#ifndef _MONJU_CONV_LAMBDA_INPUT_PORT_H__
#define _MONJU_CONV_LAMBDA_INPUT_PORT_H__

#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"

namespace monju
{
	class ConvLambdaInputPort
	{
	private:
		std::string _id;
		UniformBasisShape _shape;
		Extent _extImage, _extFilter;
		std::shared_ptr<MatrixRm<float_t>> _image;
		std::shared_ptr<MatrixCm<float_t>> _cpt;

	public:
		std::string id() const { return _id; }
		UniformBasisShape shape() const { return _shape; }
		Extent extImage() const { return _extImage; }
		Extent extFilter() const { return _extFilter; }
		std::weak_ptr<MatrixRm<float_t>> image() const { return _image; }
		std::weak_ptr<MatrixCm<float_t>> cpt() const { return _cpt; }

	public:
		ConvLambdaInputPort(std::string id, UniformBasisShape shape, Extent extImage, Extent extFilter)
		{
			_id = id;
			_shape = shape;
			_extImage = extImage;
			_extFilter = extFilter;

			_image = std::make_shared<MatrixRm<float_t>>();
			_image->resize(extImage.rows, extImage.cols);

			_cpt = std::make_shared<MatrixCm<float_t>>();
			_cpt->resize(extFilter.rows, extFilter.cols);
			
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

		// コピー禁止・ムーブ禁止
	public:
		ConvLambdaInputPort(const ConvLambdaInputPort&) = delete;
		ConvLambdaInputPort(ConvLambdaInputPort&&) = delete;
		ConvLambdaInputPort& operator =(const ConvLambdaInputPort&) = delete;
		ConvLambdaInputPort& operator =(ConvLambdaInputPort&&) = delete;
	};
}

#endif // _MONJU_CONV_LAMBDA_INPUT_PORT_H__