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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>


#include <CL/cl.h>
#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "Eigen/Geometry"
#include "nlohmann/json.hpp"

#include "mtrebi/ThreadPool.h"

#include "KernelData.h"

namespace monju {

	const std::string EIGEN_BINARY_FILE_EXT = "ebi";

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

	// 行列　種類
	const int kDensityRectangular = 1;	// 行列
	const int kDensityLowerTriangular = 2;	// 下三角行列（正方行列）

	// 行列　優先方向
	const int kRowMajor = 1;
	const int kColMajor = 2;

	// 前方宣言
	class Synchronizable;
	class OpenClException;
	class MonjuException;

	class Extent;
	class Entry;
	class GridExtent;
	class GridEntry;


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

	class GridMatrixStorage;

	class ThreadPool;

	enum class ErrorCode;
	enum class VariableKind;
}

#endif // !_MONJU_TYPES_H__

