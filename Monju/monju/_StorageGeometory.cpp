#include "_StorageGeometory.h"

monju::inner::_StorageGeometory::_StorageGeometory()
{
	_grid_shape = GridShape::None;
	_grid_rows = 0;
	_grid_cols = 0;
	_mat_rows = 0;
	_mat_cols = 0;
	_elem_bytes = 0;

	_cell_stride_bytes = 0;
	_cell_bytes = 0;
	_num_cells = 0;
}

monju::inner::_StorageGeometory::_StorageGeometory(GridShape grid_shape, int grid_rows, int grid_cols, int mat_rows, int mat_cols, int elem_size)
{
	_grid_shape = grid_shape;
	_grid_rows = grid_rows;
	_grid_cols = grid_cols;
	_mat_rows = mat_rows;
	_mat_cols = mat_cols;
	_elem_bytes = elem_size;

	_cell_stride_bytes = 0;
	_cell_bytes = 0;
	_num_cells = 0;
}

monju::GridShape monju::inner::_StorageGeometory::gridShape() const
{
	return _grid_shape;
}

int monju::inner::_StorageGeometory::gridRows() const
{
	return _grid_rows;
}

int monju::inner::_StorageGeometory::gridCols() const
{
	return _grid_cols;
}

int monju::inner::_StorageGeometory::matRows() const
{
	return _mat_rows;
}

int monju::inner::_StorageGeometory::matCols() const
{
	return _mat_cols;
}

int monju::inner::_StorageGeometory::elemBytes() const
{
	return _elem_bytes;
}

int monju::inner::_StorageGeometory::cellStrideBytes() const
{
	return _cell_stride_bytes;
}

int monju::inner::_StorageGeometory::cellBytes() const
{
	return _cell_bytes;
}


monju::inner::_RectangularStorageGeometory::_RectangularStorageGeometory() : _StorageGeometory()
{
}

monju::inner::_RectangularStorageGeometory::_RectangularStorageGeometory(int grid_rows, int grid_cols, int mat_rows, int mat_cols, int elem_size) :
	_StorageGeometory(GridShape::Rectangular, grid_rows, grid_cols, mat_rows, mat_cols, elem_size)
{
	_cell_stride_bytes = _calcCellStrideBytes(mat_rows, mat_cols, elem_size);
	_cell_bytes = _calcCellBytes(mat_rows, mat_cols, elem_size);
	_num_cells = _calcNumberOfCells(grid_rows, grid_cols);
}

int monju::inner::_RectangularStorageGeometory::_calcCellStrideBytes(int mat_rows, int mat_cols, int elem_size)
{
	return (mat_rows * mat_cols * elem_size) + kPartitionWallSize;
}

int monju::inner::_RectangularStorageGeometory::_calcCellBytes(int mat_rows, int mat_cols, int elem_size)
{
	return  mat_rows * mat_cols * elem_size;
}

int monju::inner::_RectangularStorageGeometory::_calcNumberOfCells(int grid_rows, int grid_cols)
{
	return grid_rows * grid_cols;
}

int monju::inner::_RectangularStorageGeometory::_calcFileSize(int grid_row, int grid_col, int mat_rows, int mat_cols)
{
	return _cell_stride_bytes * _num_cells;
}

bool monju::inner::_RectangularStorageGeometory::checkFile(std::fstream * stream)
{
	const int size = _calcFileSize(_mat_rows, _mat_cols, _grid_rows, _grid_cols);
	const int repeat_size = kPartitionWallSize + _mat_rows * _mat_cols;
	for (int rep_row = 0; rep_row < _grid_rows; rep_row++)
	{
		for (int rep_col = 0; rep_col < _grid_cols; _grid_cols++)
		{
			int offset = rep_row * _grid_cols + rep_col;
			stream->seekg(static_cast<std::streamoff>(offset) * repeat_size, std::fstream::beg);
			for (int n = 0; n < kPartitionWallSize; n++)
			{
				char byte = 0;
				stream->read(&byte, 1);
				if (byte != kPartitionWallByte)
					return false;
			}
		}
	}
	return true;
}

void monju::inner::_RectangularStorageGeometory::format(std::fstream * stream)	// 0Ç≈èâä˙âª
{
	stream->clear();
	for (int rep_row = 0; rep_row < _grid_rows; rep_row++)
	{
		for (int rep_col = 0; rep_col < _grid_cols; rep_col++)
		{
			for (int n = 0; n < kPartitionWallSize; n++)
				stream->write(reinterpret_cast<const char*>(&kPartitionWallByte), 1);
			for (int n = 0; n < _mat_rows * _mat_cols * _elem_bytes; n++)
				stream->write(reinterpret_cast<const char*>(&kZero), 1);
		}
	}
	stream->flush();
}

void monju::inner::_RectangularStorageGeometory::packJson(json& jdoc)
{
	// èÛë‘ÇÃïúå≥Ç…ïKóvÇ»ëÆê´Çäiî[
	jdoc[kPropShape][kPropGridShape] = static_cast<int>(_grid_shape);
	jdoc[kPropShape][kPropMatRows] = _mat_rows;
	jdoc[kPropShape][kPropMatCols] = _mat_cols;
	jdoc[kPropShape][kPropGridRows] = _grid_rows;
	jdoc[kPropShape][kPropGridCols] = _grid_cols;
	jdoc[kPropShape][kPropElemSize] = _elem_bytes;
}

void monju::inner::_RectangularStorageGeometory::restoreFromJson(json& jdoc)
{
	jdoc[kPropShape][kPropGridShape].get_to(_grid_shape);
	jdoc[kPropShape][kPropMatRows].get_to(_mat_rows);
	jdoc[kPropShape][kPropMatCols].get_to(_mat_cols);
	jdoc[kPropShape][kPropGridRows].get_to(_grid_rows);
	jdoc[kPropShape][kPropGridCols].get_to(_grid_cols);
	jdoc[kPropShape][kPropElemSize].get_to(_elem_bytes);
	// åvéZçÄñ⁄
	_cell_stride_bytes = _calcCellStrideBytes(_mat_rows, _mat_cols, _elem_bytes);
	_cell_bytes = _calcCellBytes(_mat_rows, _mat_cols, _elem_bytes);
	_num_cells = _grid_rows * _grid_cols;
}

bool monju::inner::_RectangularStorageGeometory::verifyAttributes(int grid_rows, int grid_cols, int mat_rows, int mat_cols)
{
	if (grid_rows != _grid_rows)
		return false;
	if (grid_cols != _grid_cols)
		return false;
	if (mat_rows != _mat_rows)
		return false;
	if (mat_cols != _mat_cols)
		return false;
	return true;
}

int monju::inner::_RectangularStorageGeometory::calcCellByteOffsest(int grow, int gcol)
{
	return _cell_stride_bytes * (grow * _grid_cols + gcol);
}

int monju::inner::_RectangularStorageGeometory::calcGlobalByteOffset(int grow, int gcol, int row, int col)
{
	return calcCellByteOffsest(grow, gcol) + kPartitionWallSize + (row * _mat_rows + col) * _elem_bytes;
}

bool monju::inner::_RectangularStorageGeometory::isSquareGrid()
{
	return _grid_rows == _grid_cols;
}

inline bool monju::inner::_RectangularStorageGeometory::calcNextCellIndex(int & grow, int & gcol)
{
	gcol++;
	grow += gcol / _grid_cols;
	gcol = gcol % _grid_cols;
	if (grow >= _grid_rows)
	{
		grow = _grid_rows;
		gcol = 0;
		return false;
	}
	return true;
}

inline bool monju::inner::_RectangularStorageGeometory::calcNextMatrixIndex(int & row, int & col)
{
	col++;
	row += col / _mat_cols;
	col = col % _mat_cols;
	if (row >= _mat_rows)
	{
		row = _mat_rows;
		col = 0;
		return false;
	}
	return true;
}

monju::inner::_TriangularStorageGeometory::_TriangularStorageGeometory() :
	_StorageGeometory()
{
}

monju::inner::_TriangularStorageGeometory::_TriangularStorageGeometory(int grid_dim, int mat_rows, int mat_cols, int elem_size) :
	_StorageGeometory(GridShape::Trianglar, grid_dim, grid_dim, mat_rows, mat_cols, elem_size)
{
	_cell_stride_bytes = _calcCellStrideBytes(mat_rows, mat_cols, elem_size);
	_cell_bytes = _calcCellBytes(mat_rows, mat_cols, elem_size);
	_num_cells = _calcNumberOfCells(grid_dim, grid_dim);
}

int monju::inner::_TriangularStorageGeometory::_calcCellStrideBytes(int mat_rows, int mat_cols, int elem_size)
{
	return (mat_rows * mat_cols * elem_size) + kPartitionWallSize;
}

int monju::inner::_TriangularStorageGeometory::_calcCellBytes(int mat_rows, int mat_cols, int elem_size)
{
	return mat_rows * mat_cols * elem_size;
}

int monju::inner::_TriangularStorageGeometory::_calcNumberOfCells(int grid_rows, int grid_cols)
{
	_ASSERT(grid_rows == grid_cols);
	return grid_rows * (grid_rows + 1) / 2; // éOäpçsóÒÅiLëzíËÅj
}

int monju::inner::_TriangularStorageGeometory::_calcFileSize(int grid_row, int grid_col, int mat_rows, int mat_cols)
{
	return _cell_stride_bytes * _num_cells;
}

bool monju::inner::_TriangularStorageGeometory::checkFile(std::fstream * stream)
{
	for (int grow = 0; grow < _grid_rows; grow++)
	{
		for (int gcol = 0; gcol <= grow; gcol++)
		{
			int offset = calcCellByteOffsest(grow, gcol);
			stream->seekg(offset, std::fstream::beg);
			for (int n = 0; n < kPartitionWallSize; n++)
			{
				char byte = 0;
				stream->read(&byte, 1);
				if (byte != kPartitionWallByte)
					return false;
			}
		}
	}
	return true;
}

void monju::inner::_TriangularStorageGeometory::format(std::fstream * stream)	// 0Ç≈èâä˙âª
{
	stream->clear();
	for (int grow = 0; grow < _grid_rows; grow++)
	{
		for (int gcol = 0; gcol <= grow; gcol++)
		{
			_ASSERT(static_cast<int>(stream->tellp()) == calcCellByteOffsest(grow, gcol));
			for (int n = 0; n < kPartitionWallSize; n++)
				stream->write(reinterpret_cast<const char*>(&kPartitionWallByte), 1);
			for (int n = 0; n < _cell_bytes; n++)
				stream->write(reinterpret_cast<const char*>(&kZero), 1);
		}
	}
	stream->flush();
}

void monju::inner::_TriangularStorageGeometory::packJson(json& jdoc)
{
	// èÛë‘ÇÃïúå≥Ç…ïKóvÇ»ëÆê´Çäiî[
	jdoc[kPropShape][kPropGridShape] = static_cast<int>(_grid_shape);
	jdoc[kPropShape][kPropMatRows] = _mat_rows;
	jdoc[kPropShape][kPropMatCols] = _mat_cols;
	jdoc[kPropShape][kPropGridRows] = _grid_rows;
	jdoc[kPropShape][kPropGridCols] = _grid_cols;
	jdoc[kPropShape][kPropElemSize] = _elem_bytes;
}

void monju::inner::_TriangularStorageGeometory::restoreFromJson(json& jdoc)
{
	jdoc[kPropShape][kPropGridShape].get_to(_grid_shape);
	jdoc[kPropShape][kPropMatRows].get_to(_mat_rows);
	jdoc[kPropShape][kPropMatCols].get_to(_mat_cols);
	jdoc[kPropShape][kPropGridRows].get_to(_grid_rows);
	jdoc[kPropShape][kPropGridCols].get_to(_grid_cols);
	jdoc[kPropShape][kPropElemSize].get_to(_elem_bytes);
	_ASSERT(_grid_rows == _grid_cols);
	// åvéZçÄñ⁄
	_cell_stride_bytes = _calcCellStrideBytes(_mat_rows, _mat_cols, _elem_bytes);
	_cell_bytes = _calcCellBytes(_mat_rows, _mat_cols, _elem_bytes);
	_num_cells = _grid_rows * (_grid_rows + 1) / 2;
}

bool monju::inner::_TriangularStorageGeometory::verifyAttributes(int grid_rows, int grid_cols, int mat_rows, int mat_cols)
{
	if (grid_rows != _grid_rows)
		return false;
	if (grid_cols != _grid_cols)
		return false;
	if (mat_rows != _mat_rows)
		return false;
	if (mat_cols != _mat_cols)
		return false;
	return true;
}

int monju::inner::_TriangularStorageGeometory::calcCellByteOffsest(int grow, int gcol)
{
	if (grow < gcol)
		std::swap(grow, gcol);
	return ((grow + 1) * grow / 2 + gcol) * _cell_stride_bytes;
}

int monju::inner::_TriangularStorageGeometory::calcGlobalByteOffset(int grow, int gcol, int row, int col)
{
	return calcCellByteOffsest(grow, gcol) + kPartitionWallSize + (row * _mat_rows + col) * _elem_bytes;
}

bool monju::inner::_TriangularStorageGeometory::isSquareGrid()
{
	return _grid_rows == _grid_cols;
}

inline bool monju::inner::_TriangularStorageGeometory::calcNextCellIndex(int & grow, int & gcol)
{
	gcol++;
	while (gcol > grow)
	{
		gcol -= (grow + 1);
		grow++;
	}
	if (grow >= _grid_rows)
	{
		grow = _grid_rows;
		gcol = 0;
		return false;
	}
	return true;
}

inline bool monju::inner::_TriangularStorageGeometory::calcNextMatrixIndex(int & row, int & col)
{
	col++;
	row += col / _mat_cols;
	col = col % _mat_cols;
	if (row >= _mat_rows)
	{
		row = _mat_rows;
		col = 0;
		return false;
	}
	return true;
}

