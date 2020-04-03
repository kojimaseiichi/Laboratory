/*
行列グリッドの形状を計算
形状のパラメータをファイルに保存（JSON）

_StorageGeometory
	_RectangularStorageGeometory
	_TriangularStorageGeometory
*/
#pragma once
#ifndef _MONJU_STORAGE_GEOMETORY_H__
#define _MONJU_STORAGE_GEOMETORY_H__

#include "MonjuTypes.h"

namespace monju {
	namespace inner {

		class _StorageGeometory
		{
			// 定数
		protected:
			const int			kPartitionWallSize = 32; // bytes
			const uint8_t		kPartitionWallByte = UINT8_C(0xcc);
			const uint8_t		kZero = UINT8_C(0);
			// JSONプロパティ名
			const char* kPropShape = "shape";
			const char* kPropGridShape = "grid_shape";
			const char* kPropMatRows = "mat_rows";
			const char* kPropMatCols = "mat_cols";
			const char* kPropGridRows = "grid_rows";
			const char* kPropGridCols = "grid_cols";
			const char* kPropElemSize = "elem_size";

			// 変数
		protected:
			monju::GridShape	_grid_shape;		// グリッド形状
			int					_grid_rows;			// グリッドの行数
			int					_grid_cols;			// グリッドの列数
			int					_mat_rows;			// 行列(セル)の行数
			int					_mat_cols;			// 行列(セル)の列数
			int					_elem_bytes;		// 要素サイズ
			int					_cell_stride_bytes;	// 行列(セル)のストライド
			int					_cell_bytes;		// セルのバイトサイズ
			int					_num_cells;			// セルの個数

			// 初期化・生成
		protected:
			_StorageGeometory();
			_StorageGeometory(GridShape grid_shape, int grid_rows, int grid_cols, int mat_rows, int mat_cols, int elem_size);

			// プロパティ
		public:
			GridShape		gridShape() const;
			int				gridRows() const;
			int				gridCols() const;
			int				matRows() const;
			int				matCols() const;
			int				elemBytes() const;
			int				cellStrideBytes() const;
			int				cellBytes() const;

			// 責務
		protected:
			virtual int		_calcCellStrideBytes(int mat_rows, int mat_cols, int elem_size) = 0;
			virtual int		_calcCellBytes(int mat_rows, int mat_cols, int elem_size) = 0;
			virtual	int		_calcNumberOfCells(int grid_rows, int grid_cols) = 0;
			virtual int		_calcFileSize(int grid_row, int grid_col, int mat_rows, int mat_cols) = 0;;

		public:
			virtual bool	checkFile(std::fstream* stream) = 0;
			virtual void	format(std::fstream* stream) = 0;	// 0で初期化
			virtual void	packJson(json& jdoc) = 0;
			virtual void	restoreFromJson(json& jdoc) = 0;
			virtual bool	verifyAttributes(int mat_rows, int grid_rows, int grid_cols, int mat_cols) = 0;
			virtual int		calcCellByteOffsest(int grow, int gcol) = 0;
			virtual int		calcGlobalByteOffset(int grow, int gcol, int row, int col) = 0;
			virtual bool	isSquareGrid() = 0;
			virtual bool	calcNextCellIndex(int& grow, int& gcol) = 0;
			virtual bool	calcNextMatrixIndex(int& row, int& col) = 0;

		};

		class _RectangularStorageGeometory : public _StorageGeometory
		{
			// 初期化・生成
		public:
			_RectangularStorageGeometory();
			_RectangularStorageGeometory(int grid_rows, int grid_cols, int mat_rows, int mat_cols, int elem_size);

		protected:
			int		_calcCellStrideBytes(int mat_rows, int mat_cols, int elem_size) override;
			int		_calcCellBytes(int mat_rows, int mat_cols, int elem_size) override;
			int		_calcNumberOfCells(int grid_rows, int grid_cols) override;
			int		_calcFileSize(int grid_row, int grid_col, int mat_rows, int mat_cols) override;

		public:
			bool	checkFile(std::fstream* stream) override;
			void	format(std::fstream* stream) override;
			void	packJson(json& jdoc) override;
			void	restoreFromJson(json& jdoc) override;
			bool	verifyAttributes(int grid_rows, int grid_cols, int mat_rows, int mat_cols) override;
			int		calcCellByteOffsest(int grow, int gcol) override;
			int		calcGlobalByteOffset(int grow, int gcol, int row, int col) override;
			bool	isSquareGrid() override;
			bool	calcNextCellIndex(int& grow, int& gcol);
			bool	calcNextMatrixIndex(int& row, int& col);

		};

		// 下三角行列
		class _TriangularStorageGeometory : public _StorageGeometory
		{
			// 初期化・生成
		public:
			_TriangularStorageGeometory();
			_TriangularStorageGeometory(int grid_dim, int mat_rows, int mat_cols, int elem_size);

		protected:
			int		_calcCellStrideBytes(int mat_rows, int mat_cols, int elem_size) override;
			int		_calcCellBytes(int mat_rows, int mat_cols, int elem_size) override;
			int		_calcNumberOfCells(int grid_rows, int grid_cols) override;
			int		_calcFileSize(int grid_row, int grid_col, int mat_rows, int mat_cols) override;

		public:
			bool	checkFile(std::fstream* stream) override;
			void	format(std::fstream* stream) override;
			void	packJson(json& jdoc) override;
			void	restoreFromJson(json& jdoc) override;
			bool	verifyAttributes(int mat_rows, int grid_rows, int grid_cols, int mat_cols) override;
			int		calcCellByteOffsest(int grow, int gcol) override;
			int		calcGlobalByteOffset(int grow, int gcol, int row, int col) override;
			bool	isSquareGrid() override;
			bool	calcNextCellIndex(int& grow, int& gcol);
			bool	calcNextMatrixIndex(int& row, int& col);
		};

	} // namespace inner
} // namespace monju

#endif // _MONJU_STORAGE_GEOMETORY_H__