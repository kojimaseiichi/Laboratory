/*

[概要]
行列を要素とする行列のデータを永続化
行列をGrid、小行列をCellと呼ぶ
SQLite3の１レコードのBLOBに小行列データを保存
離散確率変数のカウンティング表として利用

SQLiteデータベースは以下のテーブルで構成
・grid_matrix
・cell_data

*/
#pragma once
#ifndef _MONJU_GRID_MATRIX_STORAGE_H__
#define _MONJU_GRID_MATRIX_STORAGE_H__

#include "MonjuTypes.h"
#include "sqlite\sqlite_wrapper.h"
#include "MonjuException.h"

namespace monju
{
	class GridMatrixStorage
	{
	private:
		// 構造体　grid_matrixテーブルの属性
		struct _grid_matrix_t
		{
			int64_t rowid;
			GridExtent grid_extent;
			int grid_density;
			int coeff_size;
			int grid_major;
			int mat_major;

		};
		// 構造体　cell_dataテーブルの属性
		struct _cell_data_t
		{
			int64_t rowid;
			int64_t grid_id;
			int grid_row;
			int grid_col;
		};

	private:
		std::shared_ptr<sqlite::Database> _db;


	public:
		GridMatrixStorage(const std::string& path)
		{
			_db = std::make_shared<sqlite::Database>(path);
			_createTables();
		}

		~GridMatrixStorage()
		{
		}

		template <typename T>
		void prepare(const std::string& name, const GridExtent& grid_extent, const int grid_density, const int grid_major, const int mat_major)
		{
			_grid_matrix_t entry = { 0 };
			if (_findGridMatrix(name, entry))
				if (!_checkGridMatrix(entry, grid_extent, grid_density, grid_major, mat_major, sizeof(T))) throw MonjuException();
			else
			{
				_makeMatrixEntry(grid_extent, grid_density, grid_major, mat_major, sizeof(T), entry);
				_addGridMatrix(name, entry);
			}
		}

		template <typename Grid>
		void readGrid(const std::string& name, Grid& g)
		{
			_grid_matrix_t entry;
			if (!_findGridMatrix(name, entry)) throw MonjuException();

			const int mat_size = entry.grid_extent.matrix.size() * entry.coeff_size;
			if (entry.grid_density == kDensityRectangular)
			{
				if (entry.grid_major == kRowMajor)
				{
					for (int row = 0; row < entry.grid_extent.grid.rows; row++)
					{
						for (int col = 0; col < entry.grid_extent.grid.cols; col++)
						{

						}
					}
				}
				else if (entry.grid_major == kColMajor)
				{
				}
			}
			else if (entry.grid_density == kDensityLowerTriangular)
			{
				if (entry.grid_major == kRowMajor)
				{
				}
				else if (entry.grid_major == kColMajor)
				{
				}
			}

			return true;
		}


		template <typename Matrix>
		void readMatrix(std::string name, int grid_row, int grid_col, Matrix& m)
		{
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (_checkMatrixShape(m, entry)) throw MonjuException();

			_cell_data_t cellData = { 0 };
			if (!_findCellData(entry.rowid, grid_row, grid_col, cellData)) m.setZero();
			else _readMatrixData(entry, cellData, m);
		}

		template <typename Matrix>
		void readMatrix(std::string name, Matrix& m)
		{
			readMatrix(name, 0, 0, m);
		}

		template <typename Matrix>
		void writeMatrix(std::string name, int grid_row, int grid_col, Matrix& m)
		{
			_grid_matrix_t entry;
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (!_checkMatrixEntry(m, entry)) throw MonjuException();

			_cell_data_t cellData = { 0 };
			if (!_findCellData(entry.rowid, grid_row, grid_col, cellData))
			{
				_makeCellData(entry, grid_row, grid_col, cellData);
				_addCellData(cellData);
			}
			_writeMatrixData(entry, cellData, m);
		}

		template <typename Matrix>
		void writeMatrix(std::string name, Matrix& m)
		{
			writeMatrix(name, 0, 0, m);
		}


	private:
		template <typename Matrix>
		bool _checkMatrixShape(const Matrix& m, const _grid_matrix_t& entry)
		{
			return
				entry.grid_extent.matrix.rows == m.rows() &&
				entry.grid_extent.matrix.cols == m.cols();
		}

		void _makeMatrixEntry(const GridExtent& extent, const int grid_density, const int grid_major, const int mat_major, const int coeff_size, _grid_matrix_t& entry)
		{
			entry.grid_extent = extent;
			entry.grid_density = grid_density;
			entry.grid_major = grid_major;
			entry.mat_major = mat_major;
			entry.coeff_size = coeff_size;
		}

		void _makeCellData(_grid_matrix_t& entry, int grid_row, int grid_col, _cell_data_t& cell)
		{
			cell.grid_id = entry.rowid;
			cell.grid_row = grid_row;
			cell.grid_col = grid_col;
		}
		bool _checkGridMatrix(_grid_matrix_t& entry, const GridExtent& grid_extent, const int grid_density, const int grid_major, const int mat_major, const int coeff_size)
		{
			return
				entry.grid_extent == grid_extent &&
				entry.grid_density = grid_density &&
				entry.grid_major == grid_major &&
				entry.mat_major == mat_major &&
				entry.coeff_size == coeff_size;
		}
		template <typename Matrix>
		bool _checkMatrixEntry(Matrix& m, _grid_matrix_t& entry)
		{
			if (entry.mat_rows == static_cast<int32_t>(m.rows()) &&
				entry.mat_cols == static_cast<int32_t>(m.cols()) &&
				entry.mat_major == m.IsRowMajor ? kRowMajor : kColMajor &&
				entry.coeff_size = static_cast<int32_t>(sizeof(typename Matrix::Scalar)))
				return true;
			return false;
		}
		void _createTables()
		{
			const std::string ddlGridMatrix =
				"create table if not exists grid_matrix ( "
					"name text, "
					"grid_rows integer not null, "
					"grid_cols integer not null, "
					"mat_rows integer not null, "
					"mat_cols integer not null, "
					"coeff_size integer not null, "
					"grid_major integer not null, "
					"mat_major integer not null "
				") "
				"constraint pk_grid_matrix primary key (name)"
				;
			const std::string ddlCellData =
				"create table if not exists cell_data ( "
					"grid_id integer, "
					"cell_row integer, "
					"cell_col integer, "
					"data blob) "
				"constraint pk_cell_data primary key (grid_id, row, col)"
				;
			_db->exec(ddlGridMatrix);
			_db->exec(ddlCellData);
		}
		bool _findGridMatrix(const std::string& name, _grid_matrix_t& entry)
		{
			const std::string sql =
				"select rowid, grid_rows, grid_cols, mat_rows, mat_cols, coeff_size, grid_major, mat_major "
				"from grid_matrix "
				"where name = ?"
				;
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramString(1, name);
				if (stmt.step())
				{
					entry.rowid = stmt.column_int64(0);
					entry.grid_extent.grid.rows = stmt.column_int32(1);
					entry.grid_extent.grid.cols = stmt.column_int32(2);
					entry.grid_extent.matrix.rows = stmt.column_int32(3);
					entry.grid_extent.matrix.cols = stmt.column_int32(4);
					entry.coeff_size = stmt.column_int32(5);
					entry.grid_major = stmt.column_int32(6);
					entry.mat_major = stmt.column_int32(7);
					return true;
				}
			}
			return false;
		}
		bool _findCellData(int64_t grid_id, int cellRow, int cellCol, _cell_data_t& entry)
		{
			const std::string sql =
				"select rowid, grid_id, cell_row, cell_col "
				"from cell_data "
				"where grid_id = ? and cell_row = ? and cell_col = ?"
				;
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramInt64(1, grid_id);
				stmt.paramInt32(2, cellRow);
				stmt.paramInt32(3, cellCol);
				if (stmt.step())
				{
					entry.rowid = stmt.column_int64(0);
					entry.grid_id = stmt.column_int64(1);
					entry.grid_row = stmt.column_int32(2);
					entry.grid_col = stmt.column_int32(3);
					return true;
				}
			}
			return false;
		}
		void _addGridMatrix(const std::string& name,_grid_matrix_t& entry)
		{
			const std::string sql = 
				"insert into grid_matrix(name, grid_rows, grid_cols, mat_rows, mat_cols, coeff_size, grid_major, mat_major) "
				"values (?, ?, ?, ?, ?, ?, ?, ?);";
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramString(1, name);
				stmt.paramInt32(2, entry.grid_extent.grid.rows);
				stmt.paramInt32(3, entry.grid_extent.grid.cols);
				stmt.paramInt32(4, entry.grid_extent.matrix.rows);
				stmt.paramInt32(5, entry.grid_extent.matrix.cols);
				stmt.paramInt32(6, entry.coeff_size);
				stmt.paramInt32(7, entry.grid_major);
				stmt.paramInt32(8, entry.mat_major);
				stmt.step();
			}
			entry.rowid = _db->lastRowId();
		}
		void _addCellData(_cell_data_t& cell)
		{
			const std::string sql =
				"insert into cell_data(grid_id, cell_row, cell_col) "
				"values (?, ?, ?)"
				;
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramInt64(1, cell.grid_id);
				stmt.paramInt32(2, cell.grid_row);
				stmt.paramInt32(3, cell.grid_col);
				stmt.step();
			}
			cell.rowid = _db->lastRowId();
		}
		void _setBlobZero(std::string& name, int grid_row, int grid_col)
		{
			std::string sql = R"(update matrix_data set matrix = ? where name = ? and grid_row = ? and grid_col = ?;)";
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramZeroBlob(1, 0);
				stmt.paramString(2, name);
				stmt.paramInt32(3, grid_row);
				stmt.paramInt32(4, grid_col);
				stmt.step();
			}
		}
		template <typename Matrix>
		bool ckeckMatrixShape(const _grid_matrix_t& entry, const Matrix& m)
		{
			return 
				entry.grid_extent.matrix.rows == m.rows() &&
				entry.grid_extent.matrix.cols == m.cols();
		}
		template <typename Matrix>
		void _readMatrixData(_grid_matrix_t& entry, _cell_data_t& cell, Matrix& r)
		{
			if (_checkMatrixShape(r, entry)) throw MonjuException("");

			sqlite::Blob blob(_db, "cell_data", "data", cell.rowid);
			int dataSize = blob.bytes();
			if (dataSize == 0)
			{
				r.setZero();
				return;
			}
			if (dataSize != entry.grid_extent.matrix.size()) throw MonjuException("");
			sqlite::BlobReader reader(blob);
			_readMatrixDataFromBlob(entry, reader, r);
		}
		template <typename Matrix>
		void _readMatrixDataFromBlob(_grid_matrix_t& entry, sqlite::BlobReader& blob, Matrix& m)
		{
			if (entry.mat_major == kRowMajor)
			{
				for (int row = 0; row < entry.grid_extent.matrix.rows; row++)
				{
					for (int col = 0; col < entry.grid_extent.matrix.cols; col++)
						blob.read(m.coeffRef(row, col));
				}
			}
			else // column major
			{
				for (int col = 0; col < entry.grid_extent.matrix.cols; col++)
				{
					for (int row = 0; row < entry.grid_extent.matrix.rows; row++)
						blob.read(m.coeffRef(row, col));
				}
			}
		}
		template <typename Matrix>
		void _writeMatrixData(_grid_matrix_t& entry, _cell_data_t& cell,  Matrix& m)
		{
			if (_checkMatrixShape(m, entry)) throw MonjuException("");

			sqlite::Blob blob(_db, "cell_data", "data", cell.rowid);
			sqlite::BlobWriter writer(blob);
			_writeMatrixDataToBlob(entry, writer, m);
		}
		template <typename Matrix>
		void _writeMatrixDataToBlob(_grid_matrix_t& entry, sqlite::BlobWriter& blob, Matrix& m)
		{
			if (entry.mat_major == kRowMajor)
			{
				for (int row = 0; row < entry.grid_extent.matrix.rows; row++)
				{
					for (int col = 0; col < entry.grid_extent.matrix.cols; col++)
						blob.write(m.coeffRef(row, col));
				}
			}
			else
			{
				for (int col = 0; col < entry.grid_extent.matrix.cols; col++)
				{
					for (int row = 0; row < entry.grid_extent.matrix.rows; row++)
						blob.write(m.coeffRef(row, col));
				}
			}
		}
	};
}

#endif // !_MONJU_GRID_MATRIX_STORAGE_H__
