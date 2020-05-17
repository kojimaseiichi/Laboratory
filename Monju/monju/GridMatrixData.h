#pragma once
#ifndef _MONJU_GRID_MATRIX_DATA_H__
#define _MONJU_GRID_MATRIX_DATA_H__

#include "MonjuTypes.h"
#include "sqlite\sqlite_wrapper.h"
#include "MonjuException.h"

namespace monju
{
	class GridMatrixData
	{
	private:
		struct _matrix_entry
		{
			int64_t rowid;
			int grid_row;
			int grid_col;
			int rows;
			int cols;
			int coeff_size;
			int major;

			int size()
			{
				return rows * cols;
			}
			int bytes()
			{
				return rows * cols * coeff_size;
			}
		};
		const std::string _table = "matrix_data";
	private:
		std::shared_ptr<sqlite::Database> _db;


	public:
		GridMatrixData(const std::string& path)
		{
			_db = std::make_shared<sqlite::Database>(path);
			_createTables();
		}
		~GridMatrixData()
		{
		}
		template <typename Matrix>
		bool readMatrix(std::string name, int grid_row, int grid_col, Matrix& m)
		{
			_matrix_entry entry = { 0 };
			if (!_findMatrixEntry(name, grid_row, grid_col, entry))
				return false;
			_readMatrix(entry, m);
			return true;
		}
		template <typename Matrix>
		bool readMatrix(std::string name, Matrix& m)
		{
			return readMatrix(name, 0, 0, m);
		}
		template <typename Matrix>
		void writeMatrix(std::string name, int grid_row, int grid_col, Matrix& m)
		{
			_matrix_entry entry;
			if (_findMatrixEntry(name, grid_row, grid_col, entry))
			{
				if (_checkMatrixEntry(m, entry) == false)
					throw MonjuException();
			}
			else
			{
				_makeMatrixEntry(grid_row, grid_col, m, entry);
				_addMatrixEntry(name, entry, m);
			}
			_writeMatrix(entry, m);
		}
		template <typename Matrix>
		void writeMatrix(std::string name, Matrix& m)
		{
			writeMatrix(name, 0, 0, m);
		}


	private:
		template <typename Matrix>
		void _makeMatrixEntry(int grid_row, int grid_col, Matrix& m, _matrix_entry& entry)
		{
			entry.grid_row = grid_row;
			entry.grid_col = grid_col;
			entry.rows = static_cast<int32_t>(m.rows());
			entry.cols = static_cast<int32_t>(m.cols());
			entry.coeff_size = static_cast<int32_t>(sizeof(typename Matrix::Scalar));
			entry.major = m.IsRowMajor ? 1 : 0;
		}
		template <typename Matrix>
		bool _checkMatrixEntry(Matrix& m, _matrix_entry& entry)
		{
			if (entry.rows == static_cast<int32_t>(m.rows()) &&
				entry.cols == static_cast<int32_t>(m.cols()) &&
				entry.major == m.IsRowMajor ? 1 : 0 &&
				entry.coeff_size = static_cast<int32_t>(sizeof(typename Matrix::Scalar))))
				return true;
			return false;
		}
		void _createTables()
		{
			std::string sql = R"(
				create table if not exists matrix_data (
					name text,
					grid_row integer,
					grid_col integer,
					rows integer,
					cols integer,
					major integer,
					coeff_size integer,
					matrix blob
				)
				constraint pk_matrix_data primary key (name, grid_row, grid_col)
				;
			)";
			_db->exec(sql);
		}
		bool _findMatrixEntry(std::string& name, int grid_row, int grid_col, _matrix_entry& entry)
		{
			// name = @name and grid_row = 0 and grid_col = 0
			std::string sql = R"(select rowid, rows, cols, major, coeff_size from matrix_data where name = ? and grid_row = ? and grid_col = ?;)";
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramString(1, name);
				stmt.paramInt32(2, grid_row);
				stmt.paramInt32(3, grid_col);
				if (stmt.step())
				{
					entry.rowid = stmt.column_int64(0);
					entry.grid_row = 0;
					entry.grid_col = 0;
					entry.rows = stmt.column_int32(1);
					entry.cols = stmt.column_int32(2);
					entry.major = stmt.column_int32(3);
					entry.coeff_size = stmt.column_int32(4);
					return true;
				}
			}
			return false;
		}
		template <typename Matrix>
		void _addMatrixEntry(std::string& name, _matrix_entry& entry, Matrix& r)
		{
			std::string sql = "insert into matrix_data(name, grid_row, grid_col, rows, cols, major, coeff_size, matrix_data) values (?, ?, ?, ?, ?, ?, ?, ?);";
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramString(1, name);
				stmt.paramInt32(2, entry.grid_row);
				stmt.paramInt32(3, entry.grid_col);
				stmt.paramInt32(4, entry.rows);
				stmt.paramInt32(5, entry.cols);
				stmt.paramInt32(6, entry.major);
				stmt.paramInt32(7, entry.coeff_size);
				stmt.paramZeroBlob(8, entry.rows * entry.cols * entry.coeff_size);
				stmt.step();
			}
			entry.rowid = _db->lastRowId();
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
		void _readMatrix(_matrix_entry& entry, Matrix& r)
		{
			sqlite::Blob blob(_db, _table, "matrix_data", entry.rowid);
			int assumedSize = entry.rows * entry.cols * entry.coeff_size;
			int realSize = blob.bytes();
			r.resize(entry.rows, entry.cols);
			if (realSize == 0)
			{
				r.setZero();
				return;
			}
			if (assumedSize != realSize)
				throw MonjuException("");
			sqlite::BlobReader reader(blob);
			for (int col = 0; col < entry.cols; col++)
			{
				for (int row = 0; row < entry.rows; row++)
					reader.read(r.coeffRef(row, col));
			}
		}
		template <typename Matrix>
		void _writeMatrix(_matrix_entry& entry, Matrix& m)
		{
			sqlite::Blob blob(_db, _table, "matrix_data", entry.rowid);
			sqlite::BlobWriter writer(blob);
			for (int col = 0; col < entry.cols; col++)
			{
				for (int row = 0; row < entry.rows; row++)
					writer.write(m.coeffRef(row, col));
			}
		}
	};
}

#endif // !_MONJU_GRID_MATRIX_DATA_H__
