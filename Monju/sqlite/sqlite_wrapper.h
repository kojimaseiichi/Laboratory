#pragma once
#ifndef _MONJU_SQLITE_DATABASE_H__
#define _MONJU_SQLITE_DATABASE_H__

#include "sqlite3.h"
#include <string>
#include <stdint.h>
#include <memory>

namespace monju
{
	namespace sqlite {

		class Database;
		class Statement;
		class Blob;
		class BlobReader;
		class BlobWriter;
		class SQLiteException;

		class SQLiteException
		{
		private: // フィールド
			int _code;

		public: // コンストラクタ
			SQLiteException(int code);

		public: // プロパティ
			int code() const;
		};

		void throwExceptionIfNotEquals(int result, int assumed);

		class Database
		{
		private:
			friend class Statement;
			friend class Blob;
			sqlite3* _db;

		public: // コンストラクタ
			Database(std::string path);
			~Database();

		public:
			void exec(const std::string& sql);
			sqlite3_int64 lastRowId();
			int changes();
		};

		class Statement
		{
		private:
			std::shared_ptr<Database> _db;
			sqlite3_stmt* _stmt;

		public:
			Statement(std::weak_ptr<Database> db, std::string sql);
			~Statement();

		public:
			void paramString(int position, const std::string& text);
			void paramInt32(int position, int32_t value);
			void paramInt64(int position, int64_t value);
			void paramZeroBlob(int position, int size);
			bool step();
			int column_int32(int nCol);
			int column_int64(int nCol);
			std::string column_text(int nCol);

		private:
		};

		class Blob
		{
		private:
			std::shared_ptr<Database> _db;
			sqlite3_blob* _blob;

		public:	// コンストラクタ
			Blob(std::weak_ptr<Database> db, std::string table, std::string column, int64_t iRow);
			~Blob();

		public:
			int bytes() const;
			void read(void* buffer, int size, int offset);
			void write(const void* buffer, int size, int offset);
			template <typename T>
			void read(T& val, int offset);
			template <typename T>
			void write(T& val, int offset);
		};

		class BlobReader
		{
		private:
			Blob* _blob;
			int _position;

		public: // コンストラクタ
			BlobReader(Blob& blob);
			~BlobReader();
			void read(void* p, int size);
			template <typename T>
			void read(T& t);
		};

		class BlobWriter
		{
		private:
			Blob* _blob;
			int _position;

		public: // コンストラクタ
			BlobWriter(Blob& blob);
			~BlobWriter();

		public:
			void write(const void* p, int size);
			template <typename T>
			void write(T value);
		};

		template<typename T>
		void Blob::read(T& val, int offset)
		{
			int result = sqlite3_blob_read(_blob, &val, sizeof(T), offset * sizeof(T));
			throwExceptionIfNotEquals(result, SQLITE_OK);
		}
		template<typename T>
		void Blob::write(T& val, int offset)
		{
			int result = sqlite3_blob_write(_blob, &val, sizeof(T), offset * sizeof(T));
			throwExceptionIfNotEquals(result, SQLITE_OK);
		}
		template<typename T>
		void BlobReader::read(T& t)
		{
			read(&t, sizeof(T));
		}
		template<typename T>
		void BlobWriter::write(T value)
		{
			write(&value, sizeof(T));
		}
}
}

#endif // !_MONJU_SQLITE_DATABASE_H__
