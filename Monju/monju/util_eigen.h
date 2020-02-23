// Eigen向けユーティリティ
#pragma once
#ifndef _MONJU_EIGEN_UTIL_H__
#define _MONJU_EIGEN_UTIL_H__
#include <string>
#include <fstream>
#include "util_file.h"

namespace monju {
	namespace util_eigen {

		/// <summary>行列サイズを指定して、要素を０で初期化</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
		}

		/// <summary>ベクトルのサイズを指定して、要素を０で初期化</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int dim)
		{
			m.resize(dim);
			m.setZero();
		}

		/// <summary>行の列のサイズを指定して、要素を指定したスカラーで初期化</summary>
		template <typename Matrix>
		void init_matrix_set(typename Matrix::Scalar s, Matrix& m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
			m += s;
		}

		/// <summary>ベクトルのサイズを指定して、要素を指定したスカラーで初期化</summary>
		template <typename Matrix>
		void init_matrix_set(typename Matrix::Scalar s, Matrix& m, int dim)
		{
			m.resize(dim);
			m.setZero();
			m += s;
		}

		template<class Matrix>
		void write_binary(std::string filename, const Matrix& matrix)
		{
			std::ofstream out(filename, std::ios::out | std::ios::binary | std::ios::trunc);
			typename Matrix::Index rows = matrix.rows(), cols = matrix.cols();
			out.write((char*)(&rows), sizeof(typename Matrix::Index));
			out.write((char*)(&cols), sizeof(typename Matrix::Index));
			out.write((char*)matrix.data(), rows * cols * sizeof(typename Matrix::Scalar));
			out.close();
		}

		template<class Matrix>
		void write_binary(std::string dir, std::string id, std::string name, std::string extension, const Matrix& matrix)
		{
			write_binary(
				util_file::combine(dir, id + "_" + name, extension),
				matrix
			);
		}

		template<class Matrix>
		bool read_binary(std::string filename, Matrix& matrix)
		{
			if (util_file::existsFile(filename) == false)
				return false;
			std::ifstream in(filename, std::ios::in | std::ios::binary);
			typename Matrix::Index rows = 0, cols = 0;
			in.read((char*)(&rows), sizeof(typename Matrix::Index));
			in.read((char*)(&cols), sizeof(typename Matrix::Index));
			matrix.resize(rows, cols);
			in.read((char*)matrix.data(), rows * cols * sizeof(typename Matrix::Scalar));
			in.close();
			return true;
		}

		template <class Matrix>
		bool read_binary(std::string dir, std::string id, std::string name, std::string extension, Matrix& matrix)
		{
			return read_binary(
				util_file::combine(dir, id + "_" + name, extension),
				matrix
			);
		}

		template <typename Matrix>
		bool restore_binary(std::string filename, Matrix& matrix, size_t dim)
		{
			if (read_binary(filename, matrix))
				return true;
			matrix.resize(dim);
			return false;
		}

		template <typename Matrix>
		bool restore_binary(std::string filename, Matrix& matrix, size_t rows, size_t cols)
		{
			if (read_binary(filename, matrix))
				return false;
			matrix.resize(rows, cols);
			return false;
		}

		template <typename Matrix>
		bool restore_binary(std::string dir, std::string id, std::string name, std::string extension, Matrix& matrix, size_t dim)
		{
			if (read_binary(dir, id, name, extension, matrix))
				return false;
			matrix.resize(dim);
			return true;
		}

		template <typename Matrix>
		bool restore_binary(std::string dir, std::string id, std::string name, std::string extension, Matrix& matrix, size_t rows, size_t cols)
		{
			if (read_binary(dir, id, name, extension, matrix))
				return false;
			matrix.resize(rows, cols);
			return true;
		}

		template <typename Matrix>
		void copy(std::weak_ptr<Matrix> src, std::weak_ptr<Matrix> dest)
		{
			auto p1 = src.lock();
			auto p2 = dest.lock();
			*p2 = *p1;
		}

		template <typename Matrix>
		bool contains_nan(std::weak_ptr<Matrix> m)
		{
			auto p = m.lock();
			return !(p->array() == p->array()).all();
		}
	}
}

#endif // !_MONJU_EIGEN_UTIL_H__
