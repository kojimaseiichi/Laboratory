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
#include "Extent.h"

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

		void close()
		{
			_db.reset();
		}

		template <typename T>
		void prepare(const char* pzName, GridExtent grid_extent, int grid_density, int grid_major, int mat_major)
		{
			std::string name(pzName);
			prepare<T>(name, grid_extent, grid_density, grid_major, mat_major);
		}

		template <typename T>
		void prepare(const std::string& name, GridExtent grid_extent, int grid_density, int grid_major, int mat_major)
		{
			_grid_matrix_t entry = { 0 };
			if (_findGridMatrix(name, entry))
			{
				if (!_checkGridMatrix(entry, grid_extent, grid_density, grid_major, mat_major, sizeof(T))) throw MonjuException();
			}
			else
			{
				_makeMatrixEntry(grid_extent, grid_density, grid_major, mat_major, sizeof(T), entry);
				_addGridMatrix(name, entry);
			}
		}

		bool getExtent(const char* pzName, GridExtent& grid_extent)
		{
			std::string name(pzName);
			return getExtent(name, grid_extent);
		}

		bool getExtent(const std::string& name, GridExtent& grid_extent)
		{
			_grid_matrix_t entry = { 0 };
			if (_findGridMatrix(name, entry))
			{
				grid_extent = entry.grid_extent;
				return true;
			}
			return false;
		}

		template <typename T>
		void prepare(const char* pzName, Extent matrix_extent, int mat_major)
		{
			std::string name(pzName);
			prepare<T>(name, matrix_extent, mat_major);
		}

		template <typename T>
		void prepare(const std::string& name, Extent matrix_extent, int mat_major)
		{
			Extent grid(1, 1);
			GridExtent gridExtent(grid, matrix_extent);
			prepare<T>(name, gridExtent, kDensityRectangular, kRowMajor, mat_major);
		}

		template <typename Grid>
		void readGrid(const char* pzName, Grid& g)
		{
			std::string name(pzName);
			readGrid(name, g);
		}

		template <typename Grid>
		void readGrid(const std::string name, Grid& g)
		{
			// 注意：Gridが下三角行列はスパースな正方行列を想定
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (entry.grid_density == kDensityRectangular)
				_readGridRect(entry, g);
			else if (entry.grid_density == kDensityLowerTriangular)
				_readGridTri(entry, g);
		}

		template <typename Grid>
		void writeGrid(const char* pzName, const Grid& g)
		{
			std::string name(pzName);
			writeGrid(name, g);
		}

		template <typename Grid>
		void writeGrid(const std::string name, const Grid& g)
		{
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (entry.grid_density == kDensityRectangular)
				_writeGridRect(entry, g);
			else if (entry.grid_density == kDensityLowerTriangular)
				_writeGridTri(entry, g);
		}

		template <typename Matrix>
		void readMatrix(const char* pzName, int grid_row, int grid_col, Matrix& m)
		{
			std::string name(pzName);
			readMatrix(name, grid_row, grid_col, m);
		}

		template <typename Matrix>
		void readMatrix(std::string name, int grid_row, int grid_col, Matrix& m)
		{
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (!_checkMatrixShape(m, entry)) throw MonjuException();
			_readMatrixCell(entry, grid_row, grid_col, m);
		}

		template <typename Matrix>
		void readMatrix(const char* pzName, Matrix& m)
		{
			std::string name(pzName);
			readMatrix(name, m);
		}

		template <typename Matrix>
		void readMatrix(std::string name, Matrix& m)
		{
			readMatrix(name, 0, 0, m);
		}

		template <typename Matrix>
		void writeMatrix(const char* pzName, int grid_row, int grid_col, Matrix& m)
		{
			std::string name(pzName);
			writeMatrix(name, grid_row, grid_col, m);
		}

		template <typename Matrix>
		void writeMatrix(std::string name, int grid_row, int grid_col, Matrix& m)
		{
			_grid_matrix_t entry;
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (!_checkMatrixEntry(m, entry)) throw MonjuException();

			_cell_data_t cell = { 0 };
			_getCellData(entry, grid_row, grid_col, cell);
			_writeMatrixData(entry, cell, m);
		}

		template <typename Matrix>
		void writeMatrix(const char* pzName, Matrix& m)
		{
			std::string name(pzName);
			writeMatrix(name, m);
		}

		template <typename Matrix>
		void writeMatrix(std::string name, Matrix& m)
		{
			writeMatrix(name, 0, 0, m);
		}

		template <typename T>
		void coeffOp(const char* pzName, const int grid_row, const int grid_col, const int row, const int col, const std::function<T(T)> f)
		{
			std::string name(pzName);
			coeffOp(name, grid_row, grid_col, row, col, f);
		}

		template <typename T>
		void coeffOp(const std::string name, const int grid_row, const int grid_col, const int row, const int col, const std::function<T(T)> f)
		{
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (!_checkMatrixEntry(entry, grid_row, grid_col, row, col)) throw MonjuException();
			_cell_data_t cell = { 0 };
			_getCellData(entry, grid_row, grid_col, cell);
			_manipulateMatrixCoeff<T>(entry, cell, row, col, f);
		}

		// TODO
		// Matrix単位でデータセット間で変換する
		template <typename MatrixSrc, typename MatrixDest>
		void _convMatrix(std::string nameSrc, std::string nameDest,
			const std::function<void(MatrixSrc&, MatrixDest&)> init,
			const std::function<void(MatrixSrc&, MatrixDest&)> conv)
		{
			_grid_matrix_t entrySrc = { 0 }, entryDest = { 0 };
			if (!_findGridMatrix(nameSrc, entrySrc)) throw MonjuException();
			if (!_findGridMatrix(nameDest, entryDest)) throw MonjuException();
			if (entrySrc.grid_extent != entryDest.grid_extent) throw MonjuException();
			// マトリックス初期化
			MatrixSrc src;
			MatrixDest dest;
			init(src, dest);

			_cell_data_t cellSrc, cellDest;
			auto gext = entrySrc.grid_extent.grid;
			for (int gcol = 0; gcol < gext.cols; gcol++)
			{
				for (int grow = 0; grow < gext.rows; grow++)
				{
					_getCellData(entrySrc, grow, gcol, cellSrc);
					_getCellData(entryDest, grow, gcol, cellDest);
					_readMatrixData(entrySrc, cellSrc, src);
					conv(src, dest);
					_writeMatrixData(entryDest, cellDest, dest);
				}
			}
		}

		template <typename T>
		void coeffOp(const char* pzName, std::vector<int32_t> rowInfo, std::vector<int32_t> colInfo, const std::function<T(T)> f)
		{
			std::string name(pzName);
			coeffOp(name, rowInfo, colInfo, f);
		}

		template <typename T>
		void coeffOp(const std::string name, std::vector<int32_t> rowInfo, std::vector<int32_t> colInfo, const std::function<T(T)> f)
		{
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry))
				throw MonjuException();
			if (rowInfo.size() != entry.grid_extent.grid.rows || colInfo.size() != entry.grid_extent.grid.cols)
				throw MonjuException();
			for (int grow = 0; grow < rowInfo.size(); grow++)
			{
				for (int gcol = 0; gcol < rowInfo.size(); gcol++)
				{
					if (!_checkMatrixEntry(entry, grow, gcol, rowInfo[grow], colInfo[gcol])) throw MonjuException();
					_cell_data_t cell = { 0 };
					if (!_findCellData(entry.rowid, grow, gcol, cell))
					{
						_makeCellData(entry, grow, gcol, cell);
						_addCellData(entry, cell);
					}
					_manipulateMatrixCoeff<T>(entry, cell, rowInfo[grow], colInfo[gcol], f);
				}
			}
		}

		template <typename Func>
		void coeffOp(const char* pzName, GridEntry at, Func f)
		{
			std::string name(pzName);
			coeffOp(name, at, f);
		}

		template <typename T>
		void coeffOp(const std::string name, GridEntry at, std::function<T(const T)> f)
		{
			coeffOp(name, at.grid.row, at.grid.col, at.matrix.row, at.matrix.col, f);
		}

		template <typename T>
		T readCoeff(const char* pzName, int grid_row, int grid_col, int row, int col)
		{
			std::string name(pzName);
			return readCoeff<T>(name, grid_row, grid_col, row, col);
		}

		template <typename T>
		T readCoeff(const std::string name, int grid_row, int grid_col, int row, int col)
		{
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (!_checkMatrixEntry(entry, grid_row, grid_col, row, col)) throw MonjuException();
			_cell_data_t cell = { 0 };
			if (!_findCellData(entry.rowid, grid_row, grid_col, cell))
				return 0;
			return _readCoeffData<T>(entry, cell, row, col);
		}

		template <typename T>
		void writeCoeff(const char* pzName, int grid_row, int grid_col, int row, int col, T val)
		{
			std::string name(pzName);
			writeCoeff(name, grid_row, grid_col, row, col, val);
		}

		template <typename T>
		void writeCoeff(const std::string name, int grid_row, int grid_col, int row, int col, T val)
		{
			_grid_matrix_t entry = { 0 };
			if (!_findGridMatrix(name, entry)) throw MonjuException();
			if (!_checkMatrixEntry(entry, grid_row, grid_col, row, col)) throw MonjuException();
			_cell_data_t cell = { 0 };
			if (!_findCellData(entry.rowid, grid_row, grid_col, cell))
			{
				_makeCellData(entry, grid_row, grid_col, cell);
				_addCellData(entry, cell);
			}
			_writeCoeffData(entry, cell, row, col);
		}

	protected:
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

		void _makeCellData(const _grid_matrix_t& entry, int grid_row, int grid_col, _cell_data_t& cell)
		{
			cell.grid_id = entry.rowid;
			cell.grid_row = grid_row;
			cell.grid_col = grid_col;
		}

		bool _checkGridMatrix(_grid_matrix_t& entry, const GridExtent& grid_extent, const int grid_density, const int grid_major, const int mat_major, const int coeff_size)
		{
			return
				entry.grid_extent == grid_extent &&
				entry.grid_density == grid_density &&
				entry.grid_major == grid_major &&
				entry.mat_major == mat_major &&
				entry.coeff_size == coeff_size;
		}

		template <typename Matrix>
		bool _checkMatrixEntry(Matrix& m, _grid_matrix_t& entry)
		{
			if (entry.grid_extent.matrix.rows == static_cast<int32_t>(m.rows()) &&
				entry.grid_extent.matrix.cols == static_cast<int32_t>(m.cols()) &&
				entry.mat_major == (m.IsRowMajor ? kRowMajor : kColMajor) &&
				entry.coeff_size == static_cast<int32_t>(sizeof(typename Matrix::Scalar)))
				return true;
			return false;
		}

		bool _checkMatrixEntry(const _grid_matrix_t& entry, const int grid_row, const int grid_col, const int row, const int col)
		{
			return entry.grid_extent.contains(grid_row, grid_col, row, col);
		}

		void _createTables()
		{
			const std::string ddlGridMatrix =
				"create table if not exists grid_matrix ( "
					"name text, "
					"grid_rows integer not null, "
					"grid_cols integer not null, "
					"grid_density integer not null, "
					"mat_rows integer not null, "
					"mat_cols integer not null, "
					"coeff_size integer not null, "
					"grid_major integer not null, "
					"mat_major integer not null, "
					"constraint pk_grid_matrix primary key (name)"
				");"
				;
			const std::string ddlCellData =
				"create table if not exists cell_data ( "
					"grid_id integer, "
					"grid_row integer, "
					"grid_col integer, "
					"data blob, "
					"constraint pk_cell_data primary key (grid_id, grid_row, grid_col) "
				");"
				;
			_db->exec(ddlGridMatrix);
			_db->exec(ddlCellData);
		}

		void _addGridMatrix(const std::string& name, _grid_matrix_t& entry)
		{
			const std::string sql =
				"insert into grid_matrix(name, grid_rows, grid_cols, grid_density, mat_rows, mat_cols, coeff_size, grid_major, mat_major) "
				"values (?, ?, ?, ?, ?, ?, ?, ?, ?);";
			{
				sqlite::Statement stmt(_db, sql);
				int pa = 1;
				stmt.paramString(pa++, name);
				stmt.paramInt32(pa++, entry.grid_extent.grid.rows);
				stmt.paramInt32(pa++, entry.grid_extent.grid.cols);
				stmt.paramInt32(pa++, entry.grid_density);
				stmt.paramInt32(pa++, entry.grid_extent.matrix.rows);
				stmt.paramInt32(pa++, entry.grid_extent.matrix.cols);
				stmt.paramInt32(pa++, entry.coeff_size);
				stmt.paramInt32(pa++, entry.grid_major);
				stmt.paramInt32(pa++, entry.mat_major);
				stmt.step();
			}
			entry.rowid = _db->lastRowId();
		}

		void  _getCellData(_grid_matrix_t& entry, const int grid_row, const int grid_col, _cell_data_t cell)
		{
			if (!_findCellData(entry.rowid, grid_row, grid_col, cell))
			{
				_makeCellData(entry, grid_row, grid_col, cell);
				_addCellData(entry, cell);
			}
		}

		void _addCellData(const _grid_matrix_t& entry, _cell_data_t& cell)
		{
			const std::string sql =
				"insert into cell_data(grid_id, grid_row, grid_col, data) "
				"values (?, ?, ?, ?)"
				;
			{
				sqlite::Statement stmt(_db, sql);
				int pa = 1;
				stmt.paramInt64(pa++, cell.grid_id);
				stmt.paramInt32(pa++, cell.grid_row);
				stmt.paramInt32(pa++, cell.grid_col);
				stmt.paramZeroBlob(pa++, entry.grid_extent.matrix.size() * entry.coeff_size);
				stmt.step();
			}
			cell.rowid = _db->lastRowId();
		}

		bool _findGridMatrix(const std::string& name, _grid_matrix_t& entry)
		{
			const std::string sql =
				"select rowid, grid_rows, grid_cols, grid_density, mat_rows, mat_cols, coeff_size, grid_major, mat_major "
				"from grid_matrix "
				"where name = ?"
				;
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramString(1, name);
				if (stmt.step())
				{
					int col = 0;
					entry.rowid = stmt.column_int64(col++);
					entry.grid_extent.grid.rows = stmt.column_int32(col++);
					entry.grid_extent.grid.cols = stmt.column_int32(col++);
					entry.grid_density = stmt.column_int32(col++);
					entry.grid_extent.matrix.rows = stmt.column_int32(col++);
					entry.grid_extent.matrix.cols = stmt.column_int32(col++);
					entry.coeff_size = stmt.column_int32(col++);
					entry.grid_major = stmt.column_int32(col++);
					entry.mat_major = stmt.column_int32(col++);
					return true;
				}
			}
			return false;
		}

		bool _findCellData(int64_t grid_id, int grid_row, int grid_col, _cell_data_t& entry)
		{
			const std::string sql =
				"select rowid, grid_id, grid_row, grid_col "
				"from cell_data "
				"where grid_id = ? and grid_row = ? and grid_col = ?"
				;
			{
				sqlite::Statement stmt(_db, sql);
				stmt.paramInt64(1, grid_id);
				stmt.paramInt32(2, grid_row);
				stmt.paramInt32(3, grid_col);
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

		void _setBlobZero(std::string& name, const int grid_row, const int grid_col, const int bytes)
		{
			std::string sql = R"(update matrix_data set matrix = ? where name = ? and grid_row = ? and grid_col = ?;)";
			{
				sqlite::Statement stmt(_db, sql);
				int pa = 1;
				stmt.paramZeroBlob(pa++, bytes);
				stmt.paramString(pa++, name);
				stmt.paramInt32(pa++, grid_row);
				stmt.paramInt32(pa++, grid_col);
				stmt.step();
			}
		}

		template <typename Matrix>
		bool _ckeckMatrixShape(const _grid_matrix_t& entry, const Matrix& m)
		{
			return 
				entry.grid_extent.matrix.rows == m.rows() &&
				entry.grid_extent.matrix.cols == m.cols();
		}

		int _calcMatrixOffset(const _grid_matrix_t& entry, const int row, const int col)
		{
			if (entry.mat_major == kRowMajor)
				return row * entry.grid_extent.matrix.cols + col;
			if (entry.mat_major == kColMajor)
				return row + col * entry.grid_extent.matrix.rows;
			return -1;
		}

		template <typename Grid>
		void _readGridRect(const _grid_matrix_t& entry, Grid& g)
		{
			if (entry.grid_major == kRowMajor)
				_readGridRectRowMajor(entry, g);
			else if (entry.grid_major == kColMajor)
				_readGridRectColMajor(entry, g);
		}

		template <typename Grid>
		void _readGridRectRowMajor(const _grid_matrix_t& entry, Grid& g)
		{
			const int mat_size = entry.grid_extent.matrix.size();
			for (int row = 0; row < entry.grid_extent.grid.rows; row++)
			{
				for (int col = 0; col < entry.grid_extent.grid.cols; col++)
				{
					_readMatrixCell(
						entry,
						row,
						col,
						g.block(
							row,
							col * mat_size,
							1,
							mat_size)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Grid>
		void _readGridRectColMajor(const _grid_matrix_t& entry, Grid& g)
		{
			const int mat_size = entry.grid_extent.matrix.size();
			for (int col = 0; col < entry.grid_extent.grid.cols; col++)
			{
				for (int row = 0; row < entry.grid_extent.grid.rows; row++)
				{
					_readMatrixCell(
						entry,
						row,
						col,
						g.block(
							row * mat_size,
							col,
							mat_size,
							1)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Grid>
		void _readGridTri(const _grid_matrix_t& entry, Grid& g)
		{
			if (entry.grid_major == kRowMajor)
				_readGridLTriRowMajor(entry, g);
			else if (entry.grid_major == kColMajor)
				_readGridLTriColMajor(entry, g);
		}

		template <typename Grid>
		void _readGridLTriRowMajor(const _grid_matrix_t& entry, Grid& g)
		{
			if (!entry.grid_extent.grid.isSquare()) throw MonjuException();
			const int mat_size = entry.grid_extent.matrix.size();
			for (int row = 0; row < entry.grid_extent.grid.rows; row++)
			{
				for (int col = 0; col <= row; col++) // 対角成分含む
				{
					_readMatrixCell(
						entry,
						row,
						col,
						g.block(
							row,
							col * mat_size,
							1,
							mat_size)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Grid>
		void _readGridLTriColMajor(const _grid_matrix_t& entry, Grid& g)
		{
			if (!entry.grid_extent.grid.isSquare()) throw MonjuException();
			const int mat_size = entry.grid_extent.matrix.size();
			for (int col = 0; col < entry.grid_extent.grid.cols; col++)
			{
				for (int row = 0; row >= col; row++) // 対角成分含む
				{
					_readMatrixCell(
						entry,
						row,
						col,
						g.block(
							row * mat_size,
							col,
							mat_size,
							1)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Grid>
		void _writeGridRect(const _grid_matrix_t& entry, const Grid& g)
		{
			if (entry.grid_major == kRowMajor)
				_writeGridRectRowMajor(entry, g);
			else if (entry.grid_major == kColMajor)
				_writeGridRectColMajor(entry, g);
		}

		template <typename Grid>
		void _writeGridRectRowMajor(const _grid_matrix_t& entry, const Grid& g)
		{
			const int mat_size = entry.grid_extent.matrix.size();
			for (int row = 0; row < entry.grid_extent.grid.rows; row++)
			{
				for (int col = 0; col < entry.grid_extent.grid.cols; col++)
				{
					_writeMatrixCell(
						entry,
						row,
						col,
						g.block(
							row,
							col * mat_size,
							1,
							mat_size)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Grid>
		void _writeGridRectColMajor(const _grid_matrix_t& entry, const Grid& g)
		{
			const int mat_size = entry.grid_extent.matrix.size();
			for (int col = 0; col < entry.grid_extent.grid.cols; col++)
			{
				for (int row = 0; row < entry.grid_extent.grid.rows; row++)
				{
					_writeMatrixCell(
						entry,
						row,
						col,
						g.block(
							row * mat_size,
							col,
							mat_size,
							1)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Grid>
		void _writeGridTri(const _grid_matrix_t& entry, const Grid& g)
		{
			if (entry.grid_major == kRowMajor)
				_writeGridLTriRowMajor(entry, g);
			else if (entry.grid_major == kColMajor)
				_writeGridLTriColMajor(entry, g);
		}

		template <typename Grid>
		void _writeGridLTriRowMajor(const _grid_matrix_t& entry, const Grid& g)
		{
			if (!entry.grid_extent.grid.isSquare()) throw MonjuException();
			const int mat_size = entry.grid_extent.matrix.size();
			for (int row = 0; row < entry.grid_extent.grid.rows; row++)
			{
				for (int col = 0; col <= row; col++) // 対角成分含む
				{
					_writeMatrixCell(
						entry,
						row,
						col,
						g.block(
							row,
							col * mat_size,
							1,
							mat_size)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Grid>
		void _writeGridLTriColMajor(const _grid_matrix_t& entry, const Grid& g)
		{
			if (!entry.grid_extent.grid.isSquare()) throw MonjuException();
			const int mat_size = entry.grid_extent.matrix.size();
			for (int col = 0; col < entry.grid_extent.grid.cols; col++)
			{
				for (int row = 0; row >= col; row++) // 対角成分含む
				{
					_writeMatrixCell(
						entry,
						row,
						col,
						g.block(
							row * mat_size,
							col,
							mat_size,
							1)
						.reshaped(
							entry.grid_extent.matrix.rows,
							entry.grid_extent.matrix.cols));
				}
			}
		}

		template <typename Matrix>
		void _writeMatrixCell(const _grid_matrix_t& entry, const int grid_row, const int grid_col, const Matrix& m)
		{
			_cell_data_t cell_data = { 0 };
			if (!_findCellData(entry.rowid, grid_row, grid_col, cell_data))
			{
				_makeCellData(entry, grid_row, grid_col, cell_data);
				_addCellData(entry, cell_data);
			}
			_writeMatrixData(entry, cell_data, m);
		}

		template <typename Matrix>
		void _readMatrixCell(const _grid_matrix_t& entry, const int grid_row, const int grid_col, Matrix& m)
		{
			_cell_data_t cellData = { 0 };
			if (!_findCellData(entry.rowid, grid_row, grid_col, cellData)) m.setZero();
			else _readMatrixData<Matrix>(entry, cellData, m);
		}

		template <typename Matrix>
		void _readMatrixData(const _grid_matrix_t& entry, _cell_data_t& cell, Matrix& r)
		{
			if (!_checkMatrixShape(r, entry)) throw MonjuException("");

			sqlite::Blob blob(_db, "cell_data", "data", cell.rowid);
			int dataSize = blob.bytes();
			if (dataSize == 0)
			{
				r.setZero();
				return;
			}
			if (dataSize != entry.grid_extent.matrix.size() * entry.coeff_size) throw MonjuException("");
			sqlite::BlobReader reader(blob);
			_readMatrixDataFromBlob(entry, reader, r);
		}

		template <typename Matrix>
		void _readMatrixDataFromBlob(const _grid_matrix_t& entry, sqlite::BlobReader& blob, Matrix& m)
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
		void _writeMatrixData(const _grid_matrix_t& entry, _cell_data_t& cell, const Matrix& m)
		{
			if (!_checkMatrixShape(m, entry)) throw MonjuException("");

			sqlite::Blob blob(_db, "cell_data", "data", cell.rowid);
			sqlite::BlobWriter writer(blob);
			_writeMatrixDataToBlob(entry, writer, m);
		}

		template <typename Matrix>
		void _writeMatrixDataToBlob(const _grid_matrix_t& entry, sqlite::BlobWriter& blob, const Matrix& m)
		{
			if (entry.mat_major == kRowMajor)
			{
				for (int row = 0; row < entry.grid_extent.matrix.rows; row++)
				{
					for (int col = 0; col < entry.grid_extent.matrix.cols; col++)
						blob.write(m.coeff(row, col));
				}
			}
			else
			{
				for (int col = 0; col < entry.grid_extent.matrix.cols; col++)
				{
					for (int row = 0; row < entry.grid_extent.matrix.rows; row++)
						blob.write(m.coeff(row, col));
				}
			}
		}

		template <typename T>
		void _manipulateMatrixCoeff(const _grid_matrix_t& entry, const _cell_data_t& cell, const int row, const int col, const std::function<T(T)>& f)
		{
			sqlite::Blob blob(_db, "cell_data", "data", cell.rowid);
			T t;
			const int offset = _calcMatrixOffset(entry, row, col);
			blob.read(t, offset);
			t = f(t);
			blob.write(t, offset);
		}

		template <typename T>
		T _readCoeffData(const _grid_matrix_t& entry, const _cell_data_t& cell, const int row, const int col)
		{
			sqlite::Blob blob(_db, "cell_data", "data", cell.rowid);
			T t;
			const int offset = _calcMatrixOffset(entry, row, col);
			blob.read(t, offset);
			return t;
		}

		template <typename T>
		void _writeCoeffData(const _grid_matrix_t& entry, const _cell_data_t& cell, const int row, const int col, T val)
		{
			sqlite::Blob blob(_db, "cell_data", "data", cell.rowid);
			const int offset = _calcMatrixOffset(entry, row, col);
			blob.write(val, offset);
		}
	};
}

#endif // !_MONJU_GRID_MATRIX_STORAGE_H__
