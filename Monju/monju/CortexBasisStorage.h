#pragma once
#ifndef _MONJU_CORTEX_BASIS_STORAGE_H__
#define _MONJU_CORTEX_BASIS_STORAGE_H__
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"

namespace monju {

	// 基底のデータをファイルに保存
	// 保持するデータ
	// ・ベイズ計算的の変数（OOBP）
	// ・（予定）予測符号化（PredNet的な）
	class CortexBasisStorage : public Synchronizable
	{
	private:

		std::string _id;
		int 
			_nodes,				// 基底のノード数
			_units_per_node;	// ノード当たりのユニット数
		MatrixRm<float_t>
			_lambda,	// λ_X(x)
			_pi,		// π_X(x)
			_rho,		// ρ_X(x)
			_r,			// 制約項_X(x)
			_bel;		// BEL_X(x)
		MatrixRm<int32_t> 
			_win;		// 勝者ユニット

	public:
		std::string id() const { return _id; }
		int nodes() const { return _nodes; }
		int unitsPerNode() const { return _units_per_node; }
		MatrixRm<float_t> lambda() const { return _lambda; }
		MatrixRm<float_t> pi() const { return _pi; };
		MatrixRm<float_t> rho() const { return _rho; }
		MatrixRm<float_t> r() const { return _r; }
		MatrixRm<float_t> bel() const { return _bel; }
		MatrixRm<int32_t> win() const { return _win; }

	public:
		CortexBasisStorage(std::string id, int nodes, int units_per_node);

		~CortexBasisStorage();

		void store(std::string dir);

		void load(std::string dir);

	};

}

#endif // !_MONJU_CORTEX_BASIS_STORAGE_H__
