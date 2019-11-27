#include "_GridMatrixStorage.h"
#include "boost/filesystem.hpp"

monju::inner::_GridMatrixStorage::_GridMatrixStorage(monju::inner::_StorageGeometory* p_geo)
{
	_p_geo.reset(p_geo);
}


monju::inner::_GridMatrixStorage::~_GridMatrixStorage()
{
	_destroy();
}

// ファイル終端にポインタを移動してファイルサイズを取得し、ファイルポインタを戻す。

int monju::inner::_GridMatrixStorage::_currentFileSize()
{
	std::streampos cur = _stream.tellg();
	_stream.seekg(0, std::fstream::beg);
	std::streampos fsize = _stream.tellg();
	_stream.seekg(0, std::fstream::end);
	fsize = _stream.tellg() - fsize;
	_stream.seekg(cur);
	return static_cast<int>(fsize);
}

void monju::inner::_GridMatrixStorage::_saveStorageGeometory(std::string file_name)
{
	json jdoc;
	_p_geo->packJson(jdoc);
	std::ofstream out;
	out.open(file_name, std::ofstream::out | std::ofstream::binary);
	out << jdoc;
	out.flush();
}

void monju::inner::_GridMatrixStorage::_restoreStorageGeometory(std::string file_name)
{
	json jdoc;
	{
		std::ifstream in;
		in.open(file_name, std::ifstream::in | std::ifstream::binary);
		in >> jdoc;
	}
	_p_geo->restoreFromJson(jdoc);
}

std::string monju::inner::_GridMatrixStorage::_jsonFileName(std::string file_name)
{
	return file_name + "." + kFileExtJson;
}

void monju::inner::_GridMatrixStorage::_create(std::string file_name, inner::_StorageGeometory* p_geo)
{
	WriteGuard guard(this);
	_p_geo.reset(p_geo);
	if (util_file::existsFile(file_name) == true)
		_openVerify(file_name, p_geo->gridRows(), p_geo->gridCols(), p_geo->matRows(), p_geo->matCols());
	else
		_createNew(file_name, p_geo->gridRows(), p_geo->gridCols(), p_geo->matRows(), p_geo->matCols());
}

void monju::inner::_GridMatrixStorage::_open(std::string file_name)
{
	WriteGuard guard(this);
	_openExisting(file_name);
}

void monju::inner::_GridMatrixStorage::_openNewFile(std::string file_name)
{
	if (util_file::existsFile(file_name) == true)
		throw MonjuException();
	_format(file_name);
	_stream.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
	_file_name = file_name;
}

void monju::inner::_GridMatrixStorage::_openExistingFile(std::string file_name)
{
	if (util_file::existsFile(file_name) == false)
		throw MonjuException();
	_stream.open(file_name, std::fstream::in | std::fstream::out | std::fstream::binary);
	_file_name = file_name;
	_restoreStorageGeometory(_jsonFileName(file_name));
}

void monju::inner::_GridMatrixStorage::_readCell(int grow, int gcol, void * p)
{
	const int offset = _p_geo->calcGlobalByteOffset(grow, gcol, 0, 0);
	_stream.clear();
	_stream.seekg(offset, std::fstream::beg);
	_stream.read(reinterpret_cast<char*>(p), _p_geo->cellBytes());
}

void monju::inner::_GridMatrixStorage::_writeCell(int grow, int gcol, void * p)
{
	const int offset = _p_geo->calcGlobalByteOffset(grow, gcol, 0, 0);
	_stream.clear();
	_stream.seekg(offset, std::fstream::beg);
	_stream.write(reinterpret_cast<char*>(p), _p_geo->cellBytes());
	_stream.flush();
}

void monju::inner::_GridMatrixStorage::_destroy()
{
	if (_stream.is_open())
		_stream.close();
	_file_name = "";
}

void monju::inner::_GridMatrixStorage::_createNew(std::string file_name, int grid_rows, int grid_cols, int mat_rows, int mat_cols)
{
	_saveStorageGeometory(_jsonFileName(file_name));
	_openNewFile(file_name);
}

inline void monju::inner::_GridMatrixStorage::_format(std::string file_name)
{
	std::fstream out(file_name, std::fstream::out | std::fstream::binary | std::fstream::app);
	_p_geo->format(&out);
}

void monju::inner::_GridMatrixStorage::_openExisting(std::string file_name)
{
	_openExistingFile(file_name);
	_p_geo->checkFile(&_stream);
}

void monju::inner::_GridMatrixStorage::_openVerify(std::string file_name, int grid_rows, int grid_cols, int mat_rows, int mat_cols)
{
	_openExistingFile(file_name);
	_restoreStorageGeometory(_jsonFileName(file_name));
	if (_p_geo->verifyAttributes(grid_rows, grid_cols, mat_rows, mat_cols) == false)
		throw MonjuException();
	_p_geo->checkFile(&_stream);
}

void monju::inner::_GridMatrixStorage::close()
{
	WriteGuard guard(this);
	_destroy();
}

