// Eigen�������[�e�B���e�B
#pragma once
#ifndef _MONJU_UTIL_EIGEN_H__
#define _MONJU_UTIL_EIGEN_H__
#include "MonjuTypes.h"
#include "util_file.h"


namespace monju {
	namespace util_eigen {

		/// <summary>�s��T�C�Y���w�肵�āA�v�f���O�ŏ�����</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
		}

		/// <summary>�x�N�g���̃T�C�Y���w�肵�āA�v�f���O�ŏ�����</summary>
		template <typename Matrix>
		void init_matrix_zero(Matrix &m, int dim)
		{
			m.resize(dim);
			m.setZero();
		}

		/// <summary>�s�̗�̃T�C�Y���w�肵�āA�v�f���w�肵���X�J���[�ŏ�����</summary>
		template <typename Matrix>
		void init_matrix_set(typename Matrix::Scalar s, Matrix& m, int row, int col)
		{
			m.resize(row, col);
			m.setZero();
			m += s;
		}

		/// <summary>�x�N�g���̃T�C�Y���w�肵�āA�v�f���w�肵���X�J���[�ŏ�����</summary>
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

		template <typename T>
		void set_stratum_prob_randmom(MatrixRm<T>* p)
		{
			// BEL�w�̃ɂ�΂̏�����
			if (p == nullptr)
				return;
			p->setRandom();
			*p = p->array().abs();
			p->array().colwise() /= p->array().rowwise().sum();
		}

		template <typename T>
		void set_stratum_prob_ramdom(MatrixCm<T>* p, int interval)
		{
			// �}�g���b�N�X�w�̃ɂ�Ȃ̏�����
			if (p == nullptr)
				return;
			int rows = static_cast<int>(p->rows()) / interval;
			for (Eigen::Index col = 0; col < p->cols(); col++)
			{
				auto m = p->col(col).reshaped(rows, interval);
				m.setRandom();
				m.array().colwise() /= p->array().rowwise().sum();
			}
		}

		template <typename T>
		void set_cpt_random(MatrixCm<T>* p, int cell_size, int cell_rows, int cell_cols)
		{
			if (p == nullptr)
				return;
			for (Eigen::Index col = 0; col < p->cols(); col++)
			{
				for (Eigen::Index row = 0; row < p->rows(); row += cell_size)
				{
					auto cell = p->block(row, col, cell_size, 1).reshaped(cell_rows, cell_cols);
					cell.setRandom();
					cell.array() /= cell.sum();
				}
			}
		}
	}
}

#endif // !_MONJU_UTIL_EIGEN_H__
