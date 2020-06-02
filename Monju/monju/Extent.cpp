#include "Extent.h"

monju::Extent::Extent()
{
	rows = cols = 0;
}

monju::Extent::Extent(int32_t rows, int32_t cols)
{
	this->rows = rows;
	this->cols = cols;
}

monju::Extent::Extent(const Extent& o)
{
	rows = o.rows;
	cols = o.cols;
}

monju::Extent& monju::Extent::operator =(const monju::Extent& o)
{
	rows = o.rows;
	cols = o.cols;
	return *this;
}

bool monju::Extent::operator ==(const monju::Extent& o)
{
	return rows == o.rows && cols == o.cols;
}

int32_t monju::Extent::size() const
{
	return rows * cols;
}

bool monju::Extent::isSquare() const
{
	return rows == cols;
}

bool monju::Extent::contains(const Entry& e) const
{
	return e.row < rows&& e.col < cols;
}

bool monju::Extent::contains(const int32_t row, const int32_t col) const
{
	return row < rows&& col < cols;
}

int32_t monju::Extent::linearId(const int32_t row, const int32_t col) const
{
	return row * cols + col;
}

monju::Entry::Entry()
{
	row = col = 0;
}

monju::Entry::Entry(int32_t row, int32_t col)
{
	this->row = row;
	this->col = col;
}

monju::Entry::Entry(const Entry& o)
{
	row = o.row;
	col = o.col;
}

monju::Entry& monju::Entry::operator =(const monju::Entry& o)
{
	row = o.row;
	col = o.col;
	return *this;
}

bool monju::Entry::operator ==(const monju::Entry& o)
{
	return row == o.row && col == o.col;
}

int32_t monju::Entry::linearId(const Extent& extent, const int major) const
{
	if (major == kRowMajor)
		return row * extent.cols + col;
	return row + col * extent.rows;
}

monju::GridEntry::GridEntry()
{
}

monju::GridEntry::GridEntry(const GridEntry& o)
{
	grid = o.grid;
	matrix = o.matrix;
}

monju::GridEntry::GridEntry(Entry grid, Entry matrix)
{
	this->grid = grid;
	this->matrix = matrix;
}

monju::GridEntry& monju::GridEntry::operator=(const monju::GridEntry& o)
{
	grid = o.grid;
	matrix = o.matrix;
	return *this;
}

bool monju::GridEntry::operator ==(const monju::GridEntry& o)
{
	return grid == o.grid && matrix == o.matrix;
}

int32_t monju::GridEntry::linearId(const GridExtent& extent, const int grid_major, const int matrix_major) const
{
	return
		grid.linearId(extent.grid, grid_major) * extent.grid.size() +
		matrix.linearId(extent.matrix, matrix_major);
}

monju::LayerStruct::LayerStruct()
{
}

monju::LayerStruct::LayerStruct(const LayerStruct& o)
{
	nodes = o.nodes;
	units = o.units;
}

monju::LayerStruct::LayerStruct(Extent nodes, Extent units)
{
	this->nodes = nodes;
	this->units = units;
}

monju::LayerStruct::LayerStruct(int32_t nodes_rows, int32_t nodes_cols, int32_t units_rows, int32_t units_cols)
	: nodes(nodes_rows, nodes_cols), units(units_rows, units_cols)
{
}

monju::LayerStruct& monju::LayerStruct::operator=(const monju::LayerStruct& o)
{
	nodes = o.nodes;
	units = o.units;
	return *this;
}

bool monju::LayerStruct::operator ==(const monju::LayerStruct& o)
{
	return nodes == o.nodes && units == o.units;
}

monju::Extent monju::LayerStruct::flatten()
{
	return Extent(nodes.size(), units.size());
}

monju::GridExtent monju::LayerStruct::asGridExtent()
{
	return GridExtent(nodes, units);
}

monju::GridExtent::GridExtent()
{
}

monju::GridExtent::GridExtent(const GridExtent& o)
{
	grid = o.grid;
	matrix = o.matrix;
}

monju::GridExtent::GridExtent(Extent grid, Extent matrix)
{
	this->grid = grid;
	this->matrix = matrix;
}

monju::GridExtent::GridExtent(const int grid_rows, const int grid_cols, const int mat_rows, const int mat_cols)
	:grid(grid_rows, grid_cols), matrix(mat_rows, mat_cols)
{
}

monju::GridExtent& monju::GridExtent::operator=(const monju::GridExtent& o)
{
	grid = o.grid;
	matrix = o.matrix;
	return *this;
}

bool monju::GridExtent::operator ==(const monju::GridExtent& o)
{
	return grid == o.grid && matrix == o.matrix;
}

int32_t monju::GridExtent::size() const
{
	return grid.size() * matrix.size();
}

bool monju::GridExtent::contains(const GridEntry& e) const
{
	return grid.contains(e.grid) && matrix.contains(e.matrix);
}

bool monju::GridExtent::contains(const int32_t grid_row, const int32_t grid_col, const int32_t row, const int32_t col) const
{
	return grid.contains(grid_row, grid_col) && matrix.contains(row, col);
}
