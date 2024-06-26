#pragma once
#ifndef _MONJU_DEVICE_PROGRAM_H__
#define _MONJU_DEVICE_PROGRAM_H__

#include <vector>
#include <map>
#include <fstream>
#include <CL/cl.h>
#include <boost/regex.hpp>
#include "DeviceContext.h"
#include "OpenClException.h"

namespace monju {

	// OpenCLプログラム
	class DeviceProgram
	{
	private:

		DeviceContext*		_p_context;
		std::vector<Device*> _device_set;
		cl_program			_program;
		std::string			_source_path;	// CLファイル名

	private:
		// プログラムビルド時の情報を取得
		std::string			_getProgramBuildInfo(
			cl_program program,
			std::vector<cl_device_id> device_id_set);

		// カーネルソースの内容を文字列で取得
		std::string			_getSource(
			std::string souce_path);

		// プレースホルダ置換済みソースコードを取得
		std::string			_getEditedSource(
			std::string file_path,
			param_map params);

		// プログラムを作成
		cl_program			_create_program(
			cl_context context,
			std::string& edited_source);

		// カーネルコンパイルしてカーネルプログラムを生成（この状態では実行できない）
		// OpenCLカーネルソースを読み込み、テンプレート変数の具体化、カーネルコンパイル
		cl_program			_compileProgram(
			cl_context context,
			std::vector<cl_device_id>& device_id_set,
			std::string file_path,
			param_map& params);

		// プログラムコンパイル(CLファイル)、カーネル生成
		void				_create(
			DeviceContext& context,
			std::vector<Device*>& device_set,
			std::string source_path,
			param_map& params);

		// コピー禁止・ムーブ禁止
	public:
		DeviceProgram(const DeviceProgram&) = delete;
		DeviceProgram(DeviceProgram&&) = delete;
		DeviceProgram& operator=(const DeviceProgram&) = delete;
		DeviceProgram& operator=(DeviceProgram&&) = delete;

	public:

		DeviceProgram();
		virtual ~DeviceProgram();

		// プログラムを初期化
		void	create(
			DeviceContext& context,
			std::vector<Device*>& device_set,
			std::string cl_file_path,
			param_map& params);

		void	release();

	public: // プロパティ
		DeviceContext&	deviceContext() const { return *_p_context; }
		cl_program		program() const { return _program; }
		std::string		clFileName() const { return _source_path; }
		std::vector<Device*>	deviceSet() const { return _device_set; }
	};

} // namespace monju

#endif // !_MONJU_DEVICE_PROGRAM_H__
