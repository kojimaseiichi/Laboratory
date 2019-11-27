#pragma once
#include <string>

#ifndef _MONJU_VARIABLES_BASE_H__
#define _MONJU_VARIABLES_BASE_H__

namespace monju {

	// �ϐ��f�[�^��ێ�
	// �f�o�C�X�������ɓ]������ɂ�BufferBase�𗘗p
	class VariablesBase
	{
		// �i����
	protected:

		// �t�@�C�������w�肵�āA�s����t�@�C���ɏ�������
		template<class Matrix>
		void _writeMatrix(std::string file_name, const Matrix& matrix)
		{
			std::ofstream out(file_name, std::ios::out | std::ios::binary | std::ios::trunc);
			typename Matrix::Index rows = matrix.rows(), cols = matrix.cols();
			out.write(reinterpret_cast<char*>(&rows), sizeof(typename Matrix::Index));
			out.write(reinterpret_cast<char*>(&cols), sizeof(typename Matrix::Index));
			out.write(reinterpret_cast<const char*>(matrix.data()), rows* cols * sizeof(typename Matrix::Scalar));
			out.close();
		}
		// �t�@�C�������w�肵�āA�s����t�@�C������ǂݍ���
		template<class Matrix>
		void _readMatrix(std::string file_name, Matrix& matrix)
		{
			std::ifstream in(file_name, std::ios::in | std::ios::binary);
			typename Matrix::Index rows = 0, cols = 0;
			in.read(reinterpret_cast<char*>(&rows), sizeof(typename Matrix::Index));
			in.read(reinterpret_cast<char*>(&cols), sizeof(typename Matrix::Index));
			matrix.resize(rows, cols);
			in.read(reinterpret_cast<char*>(matrix.data()), rows* cols * sizeof(typename Matrix::Scalar));
			in.close();
		}
		// �s��̕ۑ��i_writeMatrix�A_readMatrix���D��I�Ɏg�p����j
		template <class Matrix>
		void _storeMatrix(std::string workspace, std::string id, std::string name, std::string ext, Matrix& matrix, bool storing)
		{
			std::string file_name = util_file::combine(workspace, id + "_" + name, ext);
			if (storing)
			{
				_writeMatrix(file_name, matrix);
			}
			else
			{
				_readMatrix(file_name, matrix);
			}
		}

		// ���z�֐�
	public:
		// ���\�[�X�̉i�����i�I�u�W�F�N�g�̃��C�t�T�C�N���Ǘ��̋��ʊ֐��j
		virtual void	persist(bool storing) = 0;


	};
}
#endif // !_MONJU_VARIABLES_BASE_H__
