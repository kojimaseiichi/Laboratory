#pragma once
#ifndef _MONJU_GRID_MATRIX_STORAGE__H__
#define _MONJU_GRID_MATRIX_STORAGE__H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "GridMatrixAccessor.h"
#include "MatrixIterator.h"

namespace monju {
	namespace inner {

		/// <summary>�O���b�h�s��̉i����</summary>
		class _GridMatrixStorage : public Synchronizable
		{
			// �萔
		protected:
			const char*			kFileExtJson = "json";
			const char*			kFileExtData = "data";
			const float			kBufferRatio = 0.1f;			// �o�b�t�@�T�C�Y��(�t�@�C���T�C�Y�ɑ΂���j

			// ���\�[�X
		protected:
			std::fstream	_stream;
			std::string		_file_name;

			std::unique_ptr<inner::_StorageGeometory> _p_geo;	// �t�@�C���ւ̊i�[���@���

			// �������E����
		protected: // �����֎~
			// �h���N���X�Ő��������C���X�^���X��n���i���L���̈ړ��j
			_GridMatrixStorage(inner::_StorageGeometory* p_geo);
			virtual ~_GridMatrixStorage();

			// �⏕�֐�
		protected:
			// �t�@�C���I�[�Ƀ|�C���^���ړ����ăt�@�C���T�C�Y���擾���A�t�@�C���|�C���^��߂��B
			int			_currentFileSize();
			void		_saveStorageGeometory(std::string file_name);
			void		_restoreStorageGeometory(std::string file_name);
			template<typename T>
			void		_checkGridAccessor(GridMatrixAccessor<T>* p);
			std::string	_jsonFileName(std::string file_name);
			void		_create(std::string file_name, inner::_StorageGeometory* p_geo);
			void		_open(std::string file_name);
			void		_openNewFile(std::string file_name);
			void		_openExistingFile(std::string file_name);
			void		_readCell(int grow, int gcol, void* p);
			template<typename T>
			T			_readElement(int grow, int gcol, int row, int col);
			void		_writeCell(int grow, int gcol, void* p);
			template<typename T>
			void		_writeElement(int grow, int gcol, int row, int col, T t);
			template<typename T>
			T			_addElement(int grow, int gcol, int row, int col, T addent);
			void		_createNew(std::string file_name, int grid_rows, int grid_cols, int mat_rows, int mat_cols);
			void		_format(std::string file_name);
			void		_openExisting(std::string file_name);
			void		_openVerify(std::string file_name, int grid_rows, int grid_cols, int mat_rows, int mat_cols);
			void		_destroy();
			template <typename T>
			void		_readCell(GridMatrixAccessor<T>* p, int grow, int gcol);
			template <typename T>
			void		_readCell(MatrixAccessor<T>* p, int grow, int gcol);
			template <typename T>
			void		_writeCell(GridMatrixAccessor<T>* p, int grow, int gcol);
			template <typename T>
			void		_writeCell(MatrixAccessor<T>* p, int grow, int gcol);
			template <typename T>
			void		_load(GridMatrixAccessor<T>* p);
			template <typename T>
			void		_store(GridMatrixAccessor<T>* p);

			// ���J
		public:
			void		close();

			// ��
		public:
			GridCellIteratorContainer iterateCells();
			MatrixElementIteratorContainer iterateElements();
			// �v���p�e�B
		public:
			GridShape		gridShape() const
			{
				return _p_geo->gridShape();
			}
			int				gridRows() const
			{
				return _p_geo->gridRows();
			}
			int				gridCols() const
			{
				return _p_geo->gridCols();
			}
			int				matRows() const
			{
				return _p_geo->matRows();
			}
			int				matCols() const
			{
				return _p_geo->matCols();
			}
			int				elemBytes() const
			{
				return _p_geo->elemBytes();
			}
			int				cellStrideBytes() const
			{
				return _p_geo->cellStrideBytes();
			}
			int				cellBytes() const
			{
				return _p_geo->cellStrideBytes();
			}

		};

		template<typename T>
		void _GridMatrixStorage::_checkGridAccessor(GridMatrixAccessor<T>* p)
		{
			if (p->getGridRows() != _p_geo->gridRows() || p->getGridCols() != _p_geo->gridCols())
				throw MonjuException();
			if (p->getMatRows() != _p_geo->matRows() || p->getMatCols() != _p_geo->matCols())
				throw MonjuException();
		}

		template<typename T>
		T _GridMatrixStorage::_readElement(int grow, int gcol, int row, int col)
		{
			const int offset = _p_geo->calcGlobalByteOffset(grow, gcol, row, col);
			T t;
			_stream.clear();
			_stream.seekg(offset, std::fstream::beg);
			_stream.read(reinterpret_cast<char*>(&t), _p_geo->elemBytes());
			return t;
		}
		template<typename T>
		void _GridMatrixStorage::_writeElement(int grow, int gcol, int row, int col, T t)
		{
			const int offset = _p_geo->calcGlobalByteOffset(grow, gcol, row, col);
			_stream.clear();
			_stream.seekp(offset, std::fstream::beg);
			_stream.write(reinterpret_cast<char*>(&t), _p_geo->elemBytes());
			_stream.flush();
		}
		template<typename T>
		T _GridMatrixStorage::_addElement(int grow, int gcol, int row, int col, T addent)
		{
			const int offset = _p_geo->calcGlobalByteOffset(grow, gcol, row, col);
			T t;
			_stream.clear();
			_stream.seekg(offset, std::fstream::beg);
			_stream.read(reinterpret_cast<char*>(&t), _p_geo->elemBytes());
			t += addent;
			_stream.seekp(offset, std::fstream::beg);
			_stream.write(reinterpret_cast<char*>(&t), _p_geo->elemBytes());
			_stream.flush();
			return t;
		}
		template<typename T>
		void _GridMatrixStorage::_readCell(GridMatrixAccessor<T>* p, int grow, int gcol)
		{
			ReadGuard guard(this);

			std::vector<char> buff(_p_geo->cellBytes(), static_cast<char>(0));
			_readCell(grow, gcol, buff.data());
			p->writeCell(grow, gcol, reinterpret_cast<T*>(buff.data()));
		}
		template<typename T>
		void _GridMatrixStorage::_readCell(MatrixAccessor<T>* p, int grow, int gcol)
		{
			ReadGuard guard(this);

			std::vector<char> buff(_p_geo->cellBytes(), static_cast<char>(0));
			_readCell(grow, gcol, buff.data());
			p->writeCell(reinterpret_cast<T*>(buff.data()));
		}
		template<typename T>
		void _GridMatrixStorage::_writeCell(GridMatrixAccessor<T>* p, int grow, int gcol)
		{
			WriteGuard guard(this);

			std::vector<char> buff(_p_geo->cellBytes(), static_cast<char>(0));
			p->readCell(grow, gcol, reinterpret_cast<T*>(buff.data()));
			_writeCell(grow, gcol, buff.data());
		}
		template<typename T>
		void _GridMatrixStorage::_writeCell(MatrixAccessor<T>* p, int grow, int gcol)
		{
			WriteGuard guard(this);

			std::vector<char> buff(_p_geo->cellBytes(), static_cast<char>(0));
			p->readCell(reinterpret_cast<T*>(buff.data()));
			_writeCell(grow, gcol, buff.data());
		}
		template<typename T>
		void _GridMatrixStorage::_load(GridMatrixAccessor<T>* p)
		{
			ReadGuard guard(this);

			_checkGridAccessor(p);
			std::vector<T> buff(_p_geo->cellBytes() / sizeof(T), static_cast<T>(0));
			for (const auto& idx : iterateCells())
			{
				_readCell(idx.row, idx.col, buff.data());
				p->writeCell(idx.row, idx.col, buff.data());
			}
		}
		template<typename T>
		void _GridMatrixStorage::_store(GridMatrixAccessor<T>* p)
		{
			WriteGuard guard(this);

			_checkGridAccessor(p);
			std::vector<T> buff(_p_geo->cellBytes() / sizeof(T), static_cast<T>(0));
			for (const auto& idx : iterateCells())
			{
				p->readCell(idx.row, idx.col, buff.data());
				_writeCell(idx.row, idx.col, buff.data());
			}
		}
	}
}

#endif // _MONJU_GRID_MATRIX_STORAGE__H__
