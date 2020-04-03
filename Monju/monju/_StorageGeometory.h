/*
�s��O���b�h�̌`����v�Z
�`��̃p�����[�^���t�@�C���ɕۑ��iJSON�j

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
			// �萔
		protected:
			const int			kPartitionWallSize = 32; // bytes
			const uint8_t		kPartitionWallByte = UINT8_C(0xcc);
			const uint8_t		kZero = UINT8_C(0);
			// JSON�v���p�e�B��
			const char* kPropShape = "shape";
			const char* kPropGridShape = "grid_shape";
			const char* kPropMatRows = "mat_rows";
			const char* kPropMatCols = "mat_cols";
			const char* kPropGridRows = "grid_rows";
			const char* kPropGridCols = "grid_cols";
			const char* kPropElemSize = "elem_size";

			// �ϐ�
		protected:
			monju::GridShape	_grid_shape;		// �O���b�h�`��
			int					_grid_rows;			// �O���b�h�̍s��
			int					_grid_cols;			// �O���b�h�̗�
			int					_mat_rows;			// �s��(�Z��)�̍s��
			int					_mat_cols;			// �s��(�Z��)�̗�
			int					_elem_bytes;		// �v�f�T�C�Y
			int					_cell_stride_bytes;	// �s��(�Z��)�̃X�g���C�h
			int					_cell_bytes;		// �Z���̃o�C�g�T�C�Y
			int					_num_cells;			// �Z���̌�

			// �������E����
		protected:
			_StorageGeometory();
			_StorageGeometory(GridShape grid_shape, int grid_rows, int grid_cols, int mat_rows, int mat_cols, int elem_size);

			// �v���p�e�B
		public:
			GridShape		gridShape() const;
			int				gridRows() const;
			int				gridCols() const;
			int				matRows() const;
			int				matCols() const;
			int				elemBytes() const;
			int				cellStrideBytes() const;
			int				cellBytes() const;

			// �Ӗ�
		protected:
			virtual int		_calcCellStrideBytes(int mat_rows, int mat_cols, int elem_size) = 0;
			virtual int		_calcCellBytes(int mat_rows, int mat_cols, int elem_size) = 0;
			virtual	int		_calcNumberOfCells(int grid_rows, int grid_cols) = 0;
			virtual int		_calcFileSize(int grid_row, int grid_col, int mat_rows, int mat_cols) = 0;;

		public:
			virtual bool	checkFile(std::fstream* stream) = 0;
			virtual void	format(std::fstream* stream) = 0;	// 0�ŏ�����
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
			// �������E����
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

		// ���O�p�s��
		class _TriangularStorageGeometory : public _StorageGeometory
		{
			// �������E����
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