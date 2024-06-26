#pragma once
#include <string>

#include "PlatformContext.h"

#ifndef _MONJU_VARIABLES_BASE_H__
#define _MONJU_VARIABLES_BASE_H__

namespace monju {

	// 変数データを保持
	// デバイスメモリに転送するにはBufferBaseを利用
	class VariablesBase
	{
		// 変数
	protected:
		PlatformContext* _p_context;
		std::string _id;

		// 永続化
	protected:

		// ファイル名を指定して、行列をファイルに書き込み
		template<class Matrix>
		void _writeMatrix(std::string file_name, const Matrix& matrix);
		// ファイル名を指定して、行列をファイルから読み込み
		template<class Matrix>
		void _readMatrix(std::string file_name, Matrix& matrix);
		// 行列の保存（_writeMatrix、_readMatrixより優先的に使用する）
		template <class Matrix>
		void _storeMatrix(std::string name, std::string ext, Matrix& matrix, bool storing);

		// 公開関数
	public:
		// 初期化
		void create(PlatformContext& context, std::string id);

		// 仮想関数
	public:
		// リソースの永続化（オブジェクトのライフサイクル管理の共通関数）
		virtual void	persist(bool storing) = 0;
		// リソース開放
		virtual void	release();


	};

	// ファイル名を指定して、行列をファイルに書き込み

	template<class Matrix>
	inline void VariablesBase::_writeMatrix(std::string file_name, const Matrix& matrix)
	{
		std::ofstream out(file_name, std::ios::out | std::ios::binary | std::ios::trunc);
		typename Matrix::Index rows = matrix.rows(), cols = matrix.cols();
		out.write(reinterpret_cast<char*>(&rows), sizeof(typename Matrix::Index));
		out.write(reinterpret_cast<char*>(&cols), sizeof(typename Matrix::Index));
		out.write(reinterpret_cast<const char*>(matrix.data()), rows* cols * sizeof(typename Matrix::Scalar));
		out.close();
	}

	// ファイル名を指定して、行列をファイルから読み込み

	template<class Matrix>
	inline void VariablesBase::_readMatrix(std::string file_name, Matrix& matrix)
	{
		std::ifstream in(file_name, std::ios::in | std::ios::binary);
		typename Matrix::Index rows = 0, cols = 0;
		in.read(reinterpret_cast<char*>(&rows), sizeof(typename Matrix::Index));
		in.read(reinterpret_cast<char*>(&cols), sizeof(typename Matrix::Index));
		matrix.resize(rows, cols);
		in.read(reinterpret_cast<char*>(matrix.data()), rows* cols * sizeof(typename Matrix::Scalar));
		in.close();
	}

	// 行列の保存（_writeMatrix、_readMatrixより優先的に使用する）

	template<class Matrix>
	inline void VariablesBase::_storeMatrix(std::string name, std::string ext, Matrix& matrix, bool storing)
	{
		std::string file_name = util_file::combine(_p_context->workspaceDir(), _id + "_" + name, ext);
		if (storing)
			_writeMatrix(file_name, matrix);
		else
			_readMatrix(file_name, matrix);
	}

	// 行列の保存（_writeMatrix、_readMatrixより優先的に使用する）

}
#endif // !_MONJU_VARIABLES_BASE_H__
