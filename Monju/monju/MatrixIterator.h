#pragma once
#ifndef _MONJU_MATRIX_ITERATOR_H__
#define _MONJU_MATRIX_ITERATOR_H__

#include "_StorageGeometory.h"

namespace monju {

	struct MatrixIndex
	{
		int row;
		int col;
	};

	class MatrixIterator
	{
	private:
		inner::_StorageGeometory* _p_geo;
		int _row;
		int _col;

	public:
		MatrixIterator(inner::_StorageGeometory& geo);
		MatrixIterator(inner::_StorageGeometory& geo, int row, int col);
		MatrixIterator(const MatrixIterator&) = default;
		MatrixIterator& operator=(const MatrixIterator&) = default;
		MatrixIndex operator*();
		MatrixIterator& operator++();
		bool operator!=(const MatrixIterator& o);

	};

	class MatrixIteratorContainer
	{
	protected:
		inner::_StorageGeometory* _p_geo = nullptr;

	protected:
		virtual bool next(int& row, int& col) = 0;
	};

	class GridCellIteratorContainer : public MatrixIteratorContainer
	{
		friend class MatrixItarator;

	private:
		bool next(int&row, int& col) override
		{
			return _p_geo->calcNextCellIndex(row, col);
		}

	public:
		GridCellIteratorContainer(inner::_StorageGeometory& geo)
		{
			_p_geo = &geo;
		}

		MatrixIterator begin()
		{
			MatrixIterator o(*_p_geo);
			return o;
		}

		MatrixIterator end()
		{
			MatrixIterator o(*_p_geo, _p_geo->gridRows(), 0);
			return o;
		}
	};

	class MatrixElementIteratorContainer : public MatrixIteratorContainer
	{
		friend class MatrixItarator;

	private:
		bool next(int&row, int& col) override
		{
			return _p_geo->calcNextMatrixIndex(row, col);
		}

	public:
		MatrixElementIteratorContainer(inner::_StorageGeometory& geo)
		{
			_p_geo = &geo;
		}

		MatrixIterator begin()
		{
			MatrixIterator o(*_p_geo);
			return o;
		}

		MatrixIterator end()
		{
			MatrixIterator o(*_p_geo, _p_geo->gridRows(), 0);
			return o;
		}
	};

} // namespace monju

#endif // _MONJU_MATRIX_ITERATOR_H__