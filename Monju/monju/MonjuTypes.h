#pragma once
#ifndef _MONJU_TYPES_H__
#define _MONJU_TYPES_H__

#include <map>
#include <memory>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <functional>
#include <tuple>
#include <atomic>
#include <iostream>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <iterator>

#include <crtdbg.h>
#include <stdint.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <CL/cl.h>
#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "Eigen/Geometry"
#include "nlohmann/json.hpp"

#include "mtrebi/ThreadPool.h"

namespace monju {

	/// <summary>行ベクトル（横方向）</summary>
	template <typename T>
	using VectorRm = Eigen::Matrix<T, 1, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>列ベクトル（縦方向）</summary>
	template <typename T>
	using VectorCm = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;

	/// <summary>行優先の行列</summary>
	template <typename T>
	using MatrixRm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>列優先の行列</summary>
	template <typename T>
	using MatrixCm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;

	/// <summary>行優先のスパース行列</summary>
	template <typename T>
	using SparseMatrixRm = Eigen::SparseMatrix<T, Eigen::RowMajor>;

	/// <summary>列優先のスパース行列</summary>
	template <typename T>
	using SparseMatrixCm = Eigen::SparseMatrix<T, Eigen::ColMajor>;

	/// <summary>nlohmannのJSONクラス</summary>
	using json = nlohmann::json;

	using param_map = std::map<std::string, std::string>;

	struct Extent
	{
		int32_t rows;
		int32_t cols;

		bool operator ==(const Extent& o)
		{
			return rows == o.rows && cols == o.cols;
		}
		int32_t size() const
		{
			return rows * cols;
		}
		int32_t linearId(int32_t row, int32_t col) const
		{
			return row * cols + col;
		}
	};

	struct UniformBasisShape
	{
		Extent extent;
		uint32_t nodes;
		uint32_t units;

		bool checkExtent()
		{
			return nodes == extent.size();
		}
	};

	enum class GridShape
	{
		None = 0,
		Rectangular = 1,	// 普通の行列
		Trianglar = 2		// 三角行列 U/L区別しない
	};

	// OpenCLカーネルの引数
#pragma pack(push, 1)
	typedef struct _cell_addr
	{
		cl_int grid_row;
		cl_int grid_col;
		cl_int cell_index;
	} cell_addr;
#pragma pack(pop)


	// 前方宣言
	class Synchronizable;
	class OpenClException;
	class MonjuException;

	class _ClCommandQueue;
	class _ClContext;
	class _ClProgram;
	class _ClEvent;
	class _ClProgram;
	class _ClKernel;
	class _ClBuffer;
	class _ClPlatformId;

	class ClMachine;
	class ClMemory;
	class ClVariableSet;
	class ClKernel;
	class ClFunc;
	class ClDeviceContext;
	class ClEventJoiner;
	
	class Environment;
	class Device;
	class DeviceContext;
	class DeviceProgram;
	class DeviceKernel;
	class DeviceKernelArguments;
	class DeviceMemory;
	class DriverBase;
	class PlatformContext;

	class BelLayer;
	class BelLayerFmc;
	class BelLayerUpdaterFmc;

	class FullBayesianMatrixLayer;
	class FullBayesianMatrixLayerFmc;


	class ConvLambdaInput;

	namespace inner
	{
		class _GridMatrixStorage;
		class _StorageGeometory;
		class _RectangularStorageGeometory;
		class _TriangularStorageGeometory;

	}

	class ThreadPool;

	enum class ErrorCode;
	enum class VariableKind;
}

#endif // !_MONJU_TYPES_H__

