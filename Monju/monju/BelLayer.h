#pragma once
#ifndef _MONJU_BEL_LAYER_H__
#define _MONJU_BEL_LAYER_H__

#include <memory>
#include <string>
#include <stdint.h>
#include "eigentypes.h"
#include "Synchronizable.h"
#include "Extent.h"


namespace monju
{
	// 前方宣言
	class Environment;

	// BELレイヤーのホスト側メモリ
	class BelLayer : public Synchronizable
	{
	private: // ストレージ

		std::shared_ptr<Environment> _env;
		std::string _id;
		LayerShape _shape;
		std::shared_ptr<MatrixRm<float>>
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

	public: // 公開プロパティ
		std::string							id() const;
		LayerShape							shape() const;
		std::weak_ptr<MatrixRm<float>>		lambda() const;
		std::weak_ptr<MatrixRm<float>>		pi() const;;
		std::weak_ptr<MatrixRm<float>>		rho() const;
		std::weak_ptr<MatrixRm<float>>		r() const;
		std::weak_ptr<MatrixRm<float>>		bel() const;
		std::weak_ptr<MatrixRm<int32_t>>	win() const;

	public: // 公開メンバ
		void InitializeVariables();
		void ComputeBel();
		void ComputeToFindWinners();
		void store();
		void load();
		void CopyFrom(const BelLayer& o);
		bool TestContainsNan();

	private: // プライベートプロパティ
		std::string _data_file_name() const;

	private: // ヘルパ
		void _PersistStorage(bool storing);
		void _InitHostMemory();
		void _SetRandomProb(std::shared_ptr<MatrixRm<float>> m);

		// コピー禁止・ムーブ禁止
	public:
		BelLayer(const BelLayer&) = delete;
		BelLayer(BelLayer&&) = delete;
		BelLayer& operator =(const BelLayer&) = delete;
		BelLayer& operator =(BelLayer&&) = delete;
	};

}

#endif // !_MONJU_BEL_LAYER_H__
