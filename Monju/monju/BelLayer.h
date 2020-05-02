#pragma once
#ifndef _MONJU_BEL_LAYER_H__
#define _MONJU_BEL_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"

namespace monju {

	// 基底のデータをファイルに保存
	// 保持するデータ
	// ・ベイズ計算的の変数（OOBP）
	// ・（予定）予測符号化（PredNet的な）
	class BelLayer : public Synchronizable
	{
	private: // ストレージ

		std::string _id;
		UniformBasisShape _shape;
		std::shared_ptr<MatrixRm<float_t>>
			_lambda,	// λ_X(x)
			_pi,		// π_X(x)
			_rho,		// ρ_X(x)
			_r,			// ペナルティc項_X(x)
			_bel;		// BEL_X(x)
		std::shared_ptr<MatrixRm<int32_t>>
			_win;		// 勝者ユニット

	public:	// コンストラクタ
		BelLayer(std::string id, UniformBasisShape shape);
		~BelLayer();

	public: // 公開メンバ
		void initVariables();
		void store(std::string dir);
		void load(std::string dir);
		void findWinner();
		bool containsNan();

	private: // ヘルパ
		void _initHostMemory();
		void _setRandomProb(std::shared_ptr<MatrixRm<float_t>> m);

	public: // プロパティ
		std::string id() const { return _id; }
		UniformBasisShape shape() const { return _shape; }
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
