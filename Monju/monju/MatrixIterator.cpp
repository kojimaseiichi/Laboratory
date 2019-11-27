#include "MatrixIterator.h"

monju::MatrixIterator::MatrixIterator(inner::_StorageGeometory & geo)
{
	_p_geo = &geo;
	_row = 0;
	_col = 0;
}

monju::MatrixIterator::MatrixIterator(inner::_StorageGeometory & geo, int row, int col)
{
	_p_geo = &geo;
	_row = row;
	_col = col;
}

monju::MatrixIndex monju::MatrixIterator::operator*()
{
	return { _row, _col };
}

monju::MatrixIterator& monju::MatrixIterator::operator++()
{
	_p_geo->calcNextCellIndex(_row, _col);
	return *this;
}

bool monju::MatrixIterator::operator!=(const monju::MatrixIterator& o)
{
	return _row != o._row || _col != o._col;
}

