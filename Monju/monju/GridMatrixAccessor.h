#pragma once
#ifndef _MONJU_GRID_ACCESSOR_H__
#define _MONJU_GRID_ACCESSOR_H__

#include "MonjuTypes.h"

namespace monju {

	// 行列の要素にアクセス
	template <typename ElemType>
	class MatrixAccessor
	{
	public:
		virtual int		getMatRows() = 0;
		virtual int		getMatCols() = 0;
		virtual int		getCellSize() = 0;
		virtual void	readCell(ElemType* p) = 0;
		virtual void	writeCell(const ElemType* p) = 0;
	};

	template <typename ElemType, typename Matrix>
	class MatrixRowMajorAccessor : public MatrixAccessor<ElemType>
	{
	private:
		const int _kMatRows, _kMatCols;

	private:
		Matrix* _p;

	public:
		MatrixRowMajorAccessor(Matrix& r) :
			_kMatRows(static_cast<int>(r.rows())),
			_kMatCols(static_cast<int>(r.cols()))
		{
			_p = &r;
		}

		int getMatRows() override { return _kMatRows; }
		int getMatCols() override { return _kMatCols; }
		int getCellSize() override { return _kMatRows * _kMatCols; }
		void readCell(ElemType* p) override
		{
			const int bytes = _kMatRows* _kMatCols * sizeof(ElemType);
			memcpy(p, _p->data(), bytes);
		}
		void writeCell(const ElemType* p)
		{
			const int bytes = _kMatRows * _kMatCols * sizeof(ElemType);
			memcpy(_p->data(), p, bytes);
		}
	};

	template <typename ElemType, typename Matrix>
	class MatrixColMajorAccessor : public MatrixAccessor<ElemType>
	{
	private:
		const int _kMatRows, _kMatCols;

	private:
		Matrix* _p;

	public:
		MatrixColMajorAccessor(Matrix& r) :
			_kMatRows(static_cast<int>(r.rows())),
			_kMatCols(static_cast<int>(r.cols()))
		{
			_p = &r;
		}

		int getMatRows() override { return _kMatRows; }
		int getMatCols() override { return _kMatCols; }
		int getCellSize() override { return _kMatRows * _kMatCols; }
		void readCell(ElemType* p) override
		{
			for (int i = 0; i < _kMatRows; i++)
			{
				for (int j = 0; j < _kMatCols; j++)
					*(p++) = _p->coeff(i, j);
			}
		}
		void writeCell(const ElemType* p)
		{
			for (int i = 0; i < _kMatRows; i++)
			{
				for (int j = 0; j < _kMatCols; j++)
					_p->coeffRef(i, j) = *(p++);
			}
		}
	};

	// グリッドの行列にアクセス
	template <typename ElemType>
	class GridMatrixAccessor
	{
	public:
		virtual int		getGridRows() = 0;
		virtual int		getGridCols() = 0;
		virtual int		getMatRows() = 0;
		virtual int		getMatCols() = 0;
		virtual int		getFullSize() = 0;
		virtual int		getCellSize() = 0;
		virtual void	readCell(int grow, int gcol, ElemType* p) = 0;
		virtual void	writeCell(int grow, int gcol, const ElemType* p) = 0;
	};

	// グリッド配置の行列セット
	template <typename ElemType>
	class GridMatrixRowMajorAccessor : public GridMatrixAccessor<ElemType>
	{
	private:
		const int _kGridRows;
		const int _kGridCols;
		const int _kMatRows;
		const int _kMatCols;
		const int _kCellSize;

	private:
		Eigen::Matrix<ElemType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>* _p;

	private:
		void _checkCellSelect(int grow, int gcol)
		{
			if (grow >= _kGridRows || gcol >= _kGridCols)
				throw MonjuException();
		}

	public:
		GridMatrixRowMajorAccessor(
			int grid_rows,
			int grid_cols,
			int mat_rows,
			int mat_cols,
			Eigen::Matrix<ElemType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& r) :
			_kGridRows(grid_rows),
			_kGridCols(grid_cols),
			_kMatRows(mat_rows),
			_kMatCols(mat_cols),
			_kCellSize(mat_rows * mat_cols)
		{
			_ASSERT(grid_rows == r.rows());
			_ASSERT(grid_cols * mat_rows * mat_cols == r.cols());
			_p = &r;
		}
		GridMatrixRowMajorAccessor(Eigen::Matrix<ElemType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& r) :
			_kGridRows(1),
			_kGridCols(1),
			_kMatRows(static_cast<int>(r.rows())),
			_kMatCols(static_cast<int>(r.cols())),
			_kCellSize(static_cast<int>(r.rows() * r.cols()))
		{
			_p = &r;
		}
		int getGridRows() override
		{
			return _kGridRows;
		}
		int getGridCols() override
		{
			return _kGridCols;
		}
		int getMatRows() override
		{
			return _kMatRows;
		}
		int getMatCols() override
		{
			return _kMatCols;
		}
		int getFullSize() override
		{
			return static_cast<int>(_p->size());
		}
		int getCellSize() override
		{
			return _kCellSize;
		}
		void readCell(int grow, int gcol, ElemType* p) override
		{
			_checkCellSelect(grow, gcol);
			const int bytesToRead = _kCellSize * sizeof(ElemType);
			memcpy(p, &(_p->coeffRef(grow, gcol * _kCellSize)), bytesToRead);
		}
		void writeCell(int grow, int gcol, const ElemType* p) override
		{
			_checkCellSelect(grow, gcol);
			const int bytesToWrite = _kCellSize * sizeof(ElemType);
			memcpy(&(_p->coeffRef(grow, gcol * _kCellSize)), p, bytesToWrite);
		}
	};

	template <typename ElemType>
	class GridMatrixColMajorAccessor : public GridMatrixAccessor<ElemType>
	{
	private:
		const int _kGridRows;
		const int _kGridCols;
		const int _kMatRows;
		const int _kMatCols;
		const int _kCellSize;

	private:
		Eigen::Matrix<ElemType, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>* _p;

	private:
		// 行優先オフセット
		int _calcCellOffset(int grow, int gcol)
		{
			return (grow + gcol * _kGridRows) * _kCellSize;
		}
		void _checkCellSelect(int grow, int gcol)
		{
			if (grow >= _kGridRows || gcol >= _kGridCols)
				throw MonjuException();
		}

	public:
		GridMatrixColMajorAccessor(
			int grid_rows,
			int grid_cols,
			int mat_rows,
			int mat_cols,
			Eigen::Matrix<ElemType, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>& r) :
			_kGridRows(grid_rows),
			_kGridCols(grid_cols),
			_kMatRows(mat_rows),
			_kMatCols(mat_cols),
			_kCellSize(mat_rows * mat_cols)
		{
			_ASSERT(grid_cols == r.cols());
			_ASSERT((grid_rows * mat_rows * mat_cols) == r.rows());
			_p = &r;
		}
		GridMatrixColMajorAccessor(Eigen::Matrix<ElemType, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>& r) :
			_kGridRows(1),
			_kGridCols(1),
			_kMatRows(static_cast<int>(r.rows())),
			_kMatCols(static_cast<int>(r.cols())),
			_kCellSize(static_cast<int>(r.rows() * r.cols()))
		{
			_p = &r;
		}

		int getGridRows() override
		{
			return _kGridRows;
		}
		int getGridCols() override
		{
			return _kGridCols;
		}
		int getMatRows() override
		{
			return _kMatRows;
		}
		int getMatCols() override
		{
			return _kMatCols;
		}
		int getFullSize() override
		{
			return static_cast<int>(_p->size());
		}
		int getCellSize() override
		{
			return _kCellSize;
		}
		void readCell(int grow, int gcol, ElemType* p) override
		{
			_checkCellSelect(grow, gcol);
			for (int row = 0; row < _kMatRows; row++)
			{
				for (int col = 0; col < _kMatCols; col++)
					p[row * _kMatCols + col] = _p->coeff(grow * _kCellSize + (row + col * _kMatRows), gcol);
			}
		}
		void writeCell(int grow, int gcol, const ElemType* p) override
		{
			_checkCellSelect(grow, gcol);
			for (int row = 0; row < _kMatRows; row++)
			{
				for (int col = 0; col < _kMatCols; col++)
					_p->coeffRef(grow * _kCellSize + (row + col * _kMatRows), gcol) = p[row * _kMatCols + col];
			}
		}
	};

} // namespace monju

#endif // _MONJU_GRID_ACCESSOR_H__