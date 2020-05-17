#pragma once
#ifndef _MONJU_SQLITE_DATABASE_H__
#define _MONJU_SQLITE_DATABASE_H__

#include "sqlite3.h"
#include <string>
#include <stdint.h>

namespace monju
{
	namespace sqlite {

		class Database;
		class Statement;
		class Blob;
		class BlobReader;
		class BlobWriter;
		class SQLiteException;

		void throwExceptionIfNotEquals(int result, int assumed)
		{
			if (result != assumed)
				throw SQLiteException(result);
		}

		class SQLiteException
		{
		private: // フィールド
			int _code;

		public: // コンストラクタ
			SQLiteException(int code)
			{
				_code = code;
			}

		public: // プロパティ
			int code() const { return _code; }
		};


		class Database
		{
		private:
			friend class Statement;
			friend class Blob;
			sqlite3* _db;

		public: // コンストラクタ
			Database(std::string path)
			{
				int result = sqlite3_open(path.c_str(), &_db);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			~Database()
			{
				if (_db != nullptr)
				{
					int result = sqlite3_close(_db);
					throwExceptionIfNotEquals(result, SQLITE_OK);
				}
			}

		public:
			void exec(std::string& sql)
			{
				int result = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, nullptr);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			sqlite3_int64 lastRowId()
			{
				return sqlite3_last_insert_rowid(_db);
			}
		};

		class Statement
		{
		private:
			std::shared_ptr<Database> _db;
			sqlite3_stmt* _stmt;

		public:
			Statement(std::weak_ptr<Database> db, std::string sql)
			{
				_db = db.lock();
				int result = sqlite3_prepare(_db->_db, sql.c_str(), sql.size(), &_stmt, nullptr);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			~Statement()
			{
				int result = sqlite3_finalize(_stmt);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}

		public:
			void paramString(int position, const std::string& text)
			{
				int result = sqlite3_bind_text(_stmt, 1, text.c_str(), text.size(), nullptr);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			void paramInt32(int position, int32_t value)
			{
				int result = sqlite3_bind_int(_stmt, 1, value);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			void paramInt64(int position, int64_t value)
			{
				int result = sqlite3_bind_int64(_stmt, 1, value);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			void paramZeroBlob(int position, int size)
			{
				int result = sqlite3_bind_zeroblob(_stmt, position, size);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			bool step()
			{
				int result = sqlite3_step(_stmt);
				if (result == SQLITE_ROW)
					return true;
				if (result == SQLITE_DONE)
					return false;
				throw SQLiteException(result);
			}
			int column_int32(int nCol)
			{
				return sqlite3_column_int(_stmt, nCol);
			}
			int column_int64(int nCol)
			{
				return sqlite3_column_int64(_stmt, nCol);
			}

		private:
		};

		class Blob
		{
		private:
			std::shared_ptr<Database> _db;
			sqlite3_blob* _blob;

		public:	// コンストラクタ
			Blob(std::weak_ptr<Database> db, std::string table, std::string column, int64_t iRow)
			{
				_db = db.lock();
				int result = sqlite3_blob_open(_db->_db, "main", table.c_str(), column.c_str(), iRow, 1, &_blob);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			~Blob()
			{
				if (_blob != nullptr)
				{
					int result = sqlite3_blob_close(_blob);
					throwExceptionIfNotEquals(result, SQLITE_OK);
				}
			}
			
		public:
			int bytes() const
			{
				return sqlite3_blob_bytes(_blob);
			}
			void read(void* buffer, int size, int offset)
			{
				int result = sqlite3_blob_read(_blob, buffer, size, offset);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}
			void write(const void* buffer, int size, int offset)
			{
				int result = sqlite3_blob_write(_blob, buffer, size, offset);
				throwExceptionIfNotEquals(result, SQLITE_OK);
			}			
		};

		class BlobReader
		{
		private:
			Blob* _blob;
			int _position;

		public: // コンストラクタ
			BlobReader(Blob& blob)
			{
				_blob = &blob;
				_position = 0;
			}
			~BlobReader()
			{
			}
			void read(void* p, int size)
			{
				_blob->read(p, size, _position);
				_position += size;
			}
			template <typename T>
			void read(T& t)
			{
				read(&t, sizeof(T));
			}
		};

		class BlobWriter
		{
		private:
			Blob* _blob;
			int _position;

		public: // コンストラクタ
			BlobWriter(Blob& blob)
			{
				_blob = &blob;
				_position = 0;
			}
			~BlobWriter()
			{
			}

		public:
			void write(const void* p, int size)
			{
				_blob->write(p, size, _position);
				_position += size;
			}
			template <typename T>
			void write(T& value)
			{
				_write(&value, sizeof(T));
			}
		};
	}
}

#endif // !_MONJU_SQLITE_DATABASE_H__
