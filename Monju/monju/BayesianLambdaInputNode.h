#pragma once
#ifndef _MONJU_BAYESIAN_LAMBDA_INPUT_NODE_H__
#define _MONJU_BAYESIAN_LAMBDA_INPUT_NODE_H__
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"

namespace monju
{
	class BayesianLambdaInputNode : public Synchronizable
	{
	private:
		std::string _id;
		UniformBasisShape _shape;
		std::shared_ptr<MatrixRm<float_t>>
			_lambda;	// λ(x)

	public:
		std::string id() const { return _id; }
		UniformBasisShape shape() const { return _shape; }
		std::weak_ptr<MatrixRm<float_t>> lambda() const { return _lambda; }

	public:
		BayesianLambdaInputNode(std::string id, UniformBasisShape shape)
		{
			_id = id;
			_shape = shape;

			_lambda = std::make_shared<MatrixRm<float_t>>();

			_lambda->resize(shape.nodes, shape.units);
		}
		~BayesianLambdaInputNode()
		{
		}

		void initRandom()
		{
			_setRandomProb(_lambda);
		}
		void store(std::string dir)
		{
			std::string extension = "mat2";
			util_eigen::write_binary(dir, _id, "lambda", extension, *_lambda);
		}
		void load(std::string dir)
		{
			std::string extension = "mat2";
			if (util_eigen::read_binary(dir, _id, "lambda", extension, *_lambda) == false)
				util_eigen::init_matrix_zero(*_lambda, _shape.nodes, _shape.units);
		}
		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixRm<float_t>>(_lambda);
			return a;
		}
	private:
		void _setRandomProb(std::shared_ptr<MatrixRm<float_t>> m)
		{
			util_eigen::set_random_prob(*m);
		}

		// コピー禁止・ムーブ禁止
	public:
		BayesianLambdaInputNode(const BayesianLambdaInputNode&) = delete;
		BayesianLambdaInputNode(BayesianLambdaInputNode&&) = delete;
		BayesianLambdaInputNode& operator =(const BayesianLambdaInputNode&) = delete;
		BayesianLambdaInputNode& operator =(BayesianLambdaInputNode&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_LAMBDA_INPUT_NODE_H__
