#include "sqlite_wrapper.h"

void monju::sqlite::throwExceptionIfNotEquals(int result, int assumed)
{
	if (result != assumed)
		throw SQLiteException(result);
}

// コンストラクタ

monju::sqlite::SQLiteException::SQLiteException(int code)
{
	_code = code;
}

// プロパティ

int monju::sqlite::SQLiteException::code() const { return _code; }

// コンストラクタ

monju::sqlite::Database::Database(std::string path)
{
	int result = sqlite3_open(path.c_str(), &_db);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

monju::sqlite::Database::~Database()
{
	if (_db != nullptr)
	{
		int result = sqlite3_close(_db);
		throwExceptionIfNotEquals(result, SQLITE_OK);
	}
}

void monju::sqlite::Database::exec(const std::string& sql)
{
	int result = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, nullptr);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

sqlite3_int64 monju::sqlite::Database::lastRowId()
{
	return sqlite3_last_insert_rowid(_db);
}

int monju::sqlite::Database::changes()
{
	return sqlite3_changes(_db);
}

monju::sqlite::Statement::Statement(std::weak_ptr<Database> db, std::string sql)
{
	_db = db.lock();
	int result = sqlite3_prepare(_db->_db, sql.c_str(), sql.size(), &_stmt, nullptr);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

monju::sqlite::Statement::~Statement()
{
	int result = sqlite3_finalize(_stmt);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

void monju::sqlite::Statement::paramString(int position, const std::string& text)
{
	int result = sqlite3_bind_text(_stmt, position, text.c_str(), text.size(), nullptr);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

void monju::sqlite::Statement::paramInt32(int position, int32_t value)
{
	int result = sqlite3_bind_int(_stmt, position, value);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

void monju::sqlite::Statement::paramInt64(int position, int64_t value)
{
	int result = sqlite3_bind_int64(_stmt, position, value);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

void monju::sqlite::Statement::paramZeroBlob(int position, int size)
{
	int result = sqlite3_bind_zeroblob(_stmt, position, size);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

bool monju::sqlite::Statement::step()
{
	int result = sqlite3_step(_stmt);
	if (result == SQLITE_ROW)
		return true;
	if (result == SQLITE_DONE)
		return false;
	throw SQLiteException(result);
}

int monju::sqlite::Statement::column_int32(int nCol)
{
	return sqlite3_column_int(_stmt, nCol);
}

int monju::sqlite::Statement::column_int64(int nCol)
{
	return sqlite3_column_int64(_stmt, nCol);
}

std::string monju::sqlite::Statement::column_text(int nCol)
{
	auto p = sqlite3_column_text(_stmt, nCol);
	std::string val(reinterpret_cast<const char*>(p));
	return val;
}

// コンストラクタ

monju::sqlite::Blob::Blob(std::weak_ptr<Database> db, std::string table, std::string column, int64_t iRow)
{
	_db = db.lock();
	int result = sqlite3_blob_open(_db->_db, "main", table.c_str(), column.c_str(), iRow, 1, &_blob);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

monju::sqlite::Blob::~Blob()
{
	if (_blob != nullptr)
	{
		int result = sqlite3_blob_close(_blob);
		throwExceptionIfNotEquals(result, SQLITE_OK);
	}
}

int monju::sqlite::Blob::bytes() const
{
	return sqlite3_blob_bytes(_blob);
}

void monju::sqlite::Blob::read(void* buffer, int size, int offset)
{
	int result = sqlite3_blob_read(_blob, buffer, size, offset);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

void monju::sqlite::Blob::write(const void* buffer, int size, int offset)
{
	int result = sqlite3_blob_write(_blob, buffer, size, offset);
	throwExceptionIfNotEquals(result, SQLITE_OK);
}

// コンストラクタ

monju::sqlite::BlobReader::BlobReader(Blob& blob)
{
	_blob = &blob;
	_position = 0;
}

monju::sqlite::BlobReader::~BlobReader()
{
}

void monju::sqlite::BlobReader::read(void* p, int size)
{
	_blob->read(p, size, _position);
	_position += size;
}

// コンストラクタ

monju::sqlite::BlobWriter::BlobWriter(Blob& blob)
{
	_blob = &blob;
	_position = 0;
}

monju::sqlite::BlobWriter::~BlobWriter()
{
}

void monju::sqlite::BlobWriter::write(const void* p, int size)
{
	_blob->write(p, size, _position);
	_position += size;
}
