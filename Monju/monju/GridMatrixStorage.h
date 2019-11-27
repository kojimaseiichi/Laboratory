#pragma once
#ifndef _MONJU_GRID_MATRIX_STORAGE_H__
#define _MONJU_GRID_MATRIX_STORAGE_H__

#include "_GridMatrixStorage.h"
#include "MatrixIterator.h"

namespace monju {

	// 矩形グリッドに配置された行列のストレージ
	template <typename ElemType>
	class RectangularGridMatrixStorage : public inner::_GridMatrixStorage
	{
		// 初期化・生成
	public:
		RectangularGridMatrixStorage();

	public:
		void		create(std::string file_name, int grid_rows, int grid_cols, int mat_rows, int mat_cols);
		void		create(std::string file_name, int mat_rows, int mat_cols);
		void		open(std::string file_name);
		void		readCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol);
		void		readCell(MatrixAccessor<ElemType>& r, int grow, int gcol);
		void		readCell(MatrixAccessor<ElemType>& r);
		void		writeCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol);
		void		writeCell(MatrixAccessor<ElemType>& r, int grow, int gcol);
		void		writeCell(MatrixAccessor<ElemType>& r);
		ElemType	readElement(int grow, int gcol, int row, int col);
		ElemType	readElement(const MatrixIndex& cell, const MatrixIndex& elem);
		void		writeElement(int grow, int gcol, int row, int col, ElemType value);
		void		writeElement(const MatrixIndex& cell, const MatrixIndex& elem, ElemType value);
		ElemType	addElement(int grow, int gcol, int row, int col, ElemType addent);
		ElemType	addElement(const MatrixIndex& cell, const MatrixIndex& elem, ElemType addent);
		void		load(GridMatrixAccessor<ElemType>& r);
		void		load(MatrixAccessor<ElemType>& r);
		void		store(GridMatrixAccessor<ElemType>& r);
		void		store(MatrixAccessor<ElemType>& r);

	};

	// 三角グリッドに配置された行列のストレージ
	template <typename ElemType>
	class TriangularGridMatrixStorage : public inner::_GridMatrixStorage
	{
		// 初期化・生成
	public:
		TriangularGridMatrixStorage();

	public:
		void		create(std::string file_name, int grid_dim, int mat_rows, int mat_cols);
		void		readCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol);
		void		readCell(MatrixAccessor<ElemType>& r, int grow, int gcol);
		void		writeCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol);
		void		writeCell(MatrixAccessor<ElemType>& r, int grow, int gcol);
		ElemType	readElement(int grow, int gcol, int row, int col);
		ElemType	readElement(const MatrixIndex& cell, const MatrixIndex& elem);
		void		writeElement(int grow, int gcol, int row, int col, ElemType value);
		void		writeElement(const MatrixIndex& cell, const MatrixIndex& elem, ElemType value);
		ElemType	addElement(int grow, int gcol, int row, int col, ElemType addent);
		ElemType	addElement(const MatrixIndex& cell, const MatrixIndex& elem, ElemType addent);
		void		load(GridMatrixAccessor<ElemType>& r);
		void		store(GridMatrixAccessor<ElemType>& r);

	};

	template<typename ElemType>
	RectangularGridMatrixStorage<ElemType>::RectangularGridMatrixStorage() :
		_GridMatrixStorage(new inner::_RectangularStorageGeometory())
	{

	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::create(std::string file_name, int grid_rows, int grid_cols, int mat_rows, int mat_cols)
	{
		inner::_RectangularStorageGeometory* p_geo = new inner::_RectangularStorageGeometory(grid_rows, grid_cols, mat_rows, mat_cols, sizeof(ElemType));
		_create(file_name, p_geo);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::create(std::string file_name, int mat_rows, int mat_cols)
	{
		inner::_RectangularStorageGeometory* p_geo = new inner::_RectangularStorageGeometory(1, 1, mat_rows, mat_cols, sizeof(ElemType));
		_create(file_name, p_geo);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::open(std::string file_name)
	{
		inner::_RectangularStorageGeometory* p_geo = new inner::_RectangularStorageGeometory();
		_open(file_name);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::readCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_readCell(&r, grow, gcol);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::readCell(MatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_readCell(&r, grow, gcol);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::readCell(MatrixAccessor<ElemType>& r)
	{
		_readCell(&r, 0, 0);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::writeCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_writeCell(&r, grow, gcol);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::writeCell(MatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_writeCell(&r, grow, gcol);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::writeCell(MatrixAccessor<ElemType>& r)
	{
		_writeCell(&r, 0, 0);
	}

	template<typename ElemType>
	ElemType RectangularGridMatrixStorage<ElemType>::readElement(int grow, int gcol, int row, int col)
	{
		return _readElement<ElemType>(grow, gcol, row, col);
	}

	template<typename ElemType>
	ElemType RectangularGridMatrixStorage<ElemType>::readElement(const MatrixIndex & cell, const MatrixIndex & elem)
	{
		return _readElement<ElemType>(cell.row, cell.col, elem.row, elem.col);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::writeElement(int grow, int gcol, int row, int col, ElemType value)
	{
		_writeElement(grow, gcol, row, col, value);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::writeElement(const MatrixIndex & cell, const MatrixIndex & elem, ElemType value)
	{
		_writeElement(cell.row, cell.col, elem.row, elem.col, value);
	}

	template<typename ElemType>
	ElemType RectangularGridMatrixStorage<ElemType>::addElement(int grow, int gcol, int row, int col, ElemType addent)
	{
		return _addElement(grow, gcol, row, col, addent);
	}

	template<typename ElemType>
	ElemType RectangularGridMatrixStorage<ElemType>::addElement(const MatrixIndex & cell, const MatrixIndex & elem, ElemType addent)
	{
		return _addElement(cell.row, cell.col, elem.row, elem.col, addent);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::load(GridMatrixAccessor<ElemType>& r)
	{
		_load(&r);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::load(MatrixAccessor<ElemType>& r)
	{
		_readCell(&r, 0, 0);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::store(GridMatrixAccessor<ElemType>& r)
	{
		_store(&r);
	}

	template<typename ElemType>
	void RectangularGridMatrixStorage<ElemType>::store(MatrixAccessor<ElemType>& r)
	{
		_writeCell(&r, 0, 0);
	}

	template<typename ElemType>
	TriangularGridMatrixStorage<ElemType>::TriangularGridMatrixStorage() :
		_GridMatrixStorage(new inner::_TriangularStorageGeometory())
	{
	}

	template<typename ElemType>
	void TriangularGridMatrixStorage<ElemType>::create(std::string file_name, int grid_dim, int mat_rows, int mat_cols)
	{
		inner::_TriangularStorageGeometory* p_geo = new inner::_TriangularStorageGeometory(grid_dim, mat_rows, mat_cols, sizeof(ElemType));
		_create(file_name, p_geo);
	}

	template<typename ElemType>
	void TriangularGridMatrixStorage<ElemType>::readCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_readCell(&r, grow, gcol);
	}

	template<typename ElemType>
	inline void TriangularGridMatrixStorage<ElemType>::readCell(MatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_readCell(&r, grow, gcol);
	}

	template<typename ElemType>
	void TriangularGridMatrixStorage<ElemType>::writeCell(GridMatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_writeCell(&r, grow, gcol);
	}

	template<typename ElemType>
	inline void TriangularGridMatrixStorage<ElemType>::writeCell(MatrixAccessor<ElemType>& r, int grow, int gcol)
	{
		_writeCell(&r, grow, gcol);
	}

	template<typename ElemType>
	ElemType TriangularGridMatrixStorage<ElemType>::readElement(int grow, int gcol, int row, int col)
	{
		return _readElement<ElemType>(grow, gcol, row, col);
	}

	template<typename ElemType>
	ElemType TriangularGridMatrixStorage<ElemType>::readElement(const MatrixIndex & cell, const MatrixIndex & elem)
	{
		return _readElement<ElemType>(cell.row, cell.col, elem.row, elem.col);
	}

	template<typename ElemType>
	void TriangularGridMatrixStorage<ElemType>::writeElement(int grow, int gcol, int row, int col, ElemType value)
	{
		_writeElement(grow, gcol, row, col, value);
	}

	template<typename ElemType>
	void TriangularGridMatrixStorage<ElemType>::writeElement(const MatrixIndex & cell, const MatrixIndex & elem, ElemType value)
	{
		_writeElement(cell.row, cell.col, elem.row, elem.col, value);
	}

	template<typename ElemType>
	ElemType TriangularGridMatrixStorage<ElemType>::addElement(int grow, int gcol, int row, int col, ElemType addent)
	{
		return _addElement(grow, gcol, row, col, addent);
	}

	template<typename ElemType>
	ElemType TriangularGridMatrixStorage<ElemType>::addElement(const MatrixIndex & cell, const MatrixIndex & elem, ElemType addent)
	{
		return _addElement(cell.row, cell.col, elem.row, elem.col, addent);
	}

	template<typename ElemType>
	void TriangularGridMatrixStorage<ElemType>::load(GridMatrixAccessor<ElemType>& r)
	{
		_load(&r);
	}

	template<typename ElemType>
	void TriangularGridMatrixStorage<ElemType>::store(GridMatrixAccessor<ElemType>& r)
	{
		_store(&r);
	}

}
#endif // _MONJU_GRID_MATRIX_STORAGE_H__
