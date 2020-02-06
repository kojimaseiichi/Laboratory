#pragma once
#ifndef _MONJU_OPEN_CL_EIGEN_MEMORY_H__
#define _MONJU_OPEN_CL_EIGEN_MEMORY_H__

#include "OpenClMemory.h"

namespace monju
{
	template <typename Matrix>
	class OpenClEigenMemory : public OpenClMemory
	{
	private:
		std::weak_ptr<Matrix> _matrix;

	public:
		OpenClEigenMemory(std::weak_ptr<ClContext> clContext, std::weak_ptr<Matrix> matrix)
			:OpenClMemory(clContext, matrix.get()->size() * sizeof(typename Matrix::Scalar), matrix.get()->data())
		{
			_matrix = matrix;
		}
		~OpenClEigenMemory()
		{
			_matrix.reset();
		}
		std::weak_ptr<Matrix> matrix() const
		{
			return _matrix;
		}

		// コピー禁止・ムーブ禁止
	public:
		OpenClEigenMemory(const OpenClEigenMemory&) = delete;
		OpenClEigenMemory(OpenClEigenMemory&&) = delete;
		OpenClEigenMemory& operator=(const OpenClEigenMemory&) = delete;
		OpenClEigenMemory& operator=(OpenClEigenMemory&&) = delete;
	};
}

#endif // !_MONJU_OPEN_CL_EIGEN_MEMORY_H__
