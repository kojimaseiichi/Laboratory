#pragma once
#ifndef _MONJU_BEL_LAYER_H__
#define _MONJU_BEL_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"
#include "Extent.h"
#include "Environment.h"

namespace monju {

	// 基底のデータをファイルに保存
	// 保持するデータ
	// ・ベイズ計算的の変数（OOBP）
	// ・（予定）予測符号化（PredNet的な）
	class BelLayer : public Synchronizable
	{
	private: // ストレージ

		std::shared_ptr<Environment> _env;
		std::string _id;
		LayerShape _shape;
		std::shared_ptr<MatrixRm<float_t>>
			_lambda,	// λ_X(x) shape:(nodes, units)
			_pi,		// π_X(x) shape:(nodes, units)
			_rho,		// ρ_X(x) shape:(nodes, units)
			_r,			// ペナルティc項_X(x)  shape:(nodes, units)
			_bel;		// BEL_X(x)  shape:(nodes, units)
		std::shared_ptr<MatrixRm<int32_t>>
			_win;		// 勝者ユニット  shape:(nodes, 1)

	public:	// コンストラクタ
		BelLayer(std::weak_ptr<Environment> env, std::string id, LayerShape shape);
		~BelLayer();

	public: // 公開メンバ
		void initVariables();
		void store();
		void load();
		void findWinner();
		bool containsNan();

	private: // ヘルパ
		std::string _dataFileName() const;
		void _prepareStorage(GridMatrixStorage& storage);
		void _initHostMemory();
		void _setRandomProb(std::shared_ptr<MatrixRm<float_t>> m);

	public: // プロパティ
		std::string id() const { return _id; }
		LayerShape shape() const { return _shape; }
		std::weak_ptr<MatrixRm<float_t>> lambda() const { return _lambda; }
		std::weak_ptr<MatrixRm<float_t>> pi() const { return _pi; };
		std::weak_ptr<MatrixRm<float_t>> rho() const { return _rho; }
		std::weak_ptr<MatrixRm<float_t>> r() const { return _r; }
		std::weak_ptr<MatrixRm<float_t>> bel() const { return _bel; }
		std::weak_ptr<MatrixRm<int32_t>> win() const { return _win; }

		// コピー禁止・ムーブ禁止
	public:
		BelLayer(const BelLayer&) = delete;
		BelLayer(BelLayer&&) = delete;
		BelLayer& operator =(const BelLayer&) = delete;
		BelLayer& operator =(BelLayer&&) = delete;
	};

}

#endif // !_MONJU_BEL_LAYER_H__
