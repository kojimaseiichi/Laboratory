#include "PenaltyCalcTask.h"

monju::PenaltyCalcTask::PenaltyCalcTask(int num_of_nodes, int num_of_units, int active_num_of_nodes, std::weak_ptr<TriangularGridMatrixStorage<int32_t>> pStorage, float coefficient_win_penalty, float coefficient_lat_penalty)
	:
	kNumNodes(num_of_nodes),
	kNumUnits(num_of_units),
	kNumActiveNodes(active_num_of_nodes)
{
	_pStorage = pStorage;
	_coeWinPenalty = coefficient_win_penalty;
	_coeLatPenalty = coefficient_lat_penalty;

}

bool monju::PenaltyCalcTask::calcPenalty(std::weak_ptr<MatrixRm<float_t>> winRateRestriction, std::weak_ptr<MatrixRm<float_t>> lateralInhibition, std::weak_ptr<MatrixRm<float_t>> penalty)
{
	// カウンティングデータから勝率ペナルティと側抑制ペナルティを計算
	// 勝率ペナルティ ∝ KL距離
	// 側抑制ペナルティ ∝ -相互情報量

	MatrixRm<int32_t> mpe_count;
	mpe_count.resize(kNumNodes, kNumUnits);

	// MPEカウントをファイルから取得
	_loadWinCount(/*out*/mpe_count);

	// 勝率計算→KL距離計算→勝率ペナルティ計算（ハイパーパラメタ）
	if (auto p = winRateRestriction.lock())
		_calcWinKLDistance(mpe_count, /*out*/*p);
	else
		throw MonjuException(ErrorCode::WeakPointerExpired);

	// 同時勝率計算→相互情報量計算→側抑制ペナルティ計算（ハイパーパラメタ）
	if (auto p = lateralInhibition.lock())
		_calcLateralMutualInfo(/*out*/*p);
	else
		return false;

	// ペナルティを計算
	_calcExponentialPenalty(mpe_count, winRateRestriction, lateralInhibition, penalty);
	//{
	//	auto
	//		win = winRateRestriction.lock(),
	//		lat = lateralInhibition.lock(),
	//		pe = penalty.lock();
	//	if (win != nullptr && lat != nullptr && pe != nullptr)
	//		pe->array() = (-(win->array() * kCoeWinPenalty + lat->array() * kCoeLatPenalty) / (mpe_count.cast<float_t>().array() + 1.f)).exp();
	//	else
	//		return false;
	//}	

	return true;
}

void monju::PenaltyCalcTask::_calcExponentialPenalty(MatrixRm<int32_t>& mpe_count, std::weak_ptr<MatrixRm<float_t>>& winRateRestriction, std::weak_ptr<MatrixRm<float_t>>& lateralInhibition, std::weak_ptr<MatrixRm<float_t>>& penalty)
{
	// KL距離と総合情報量から指数関数の値を計算
	// 指数関数の発散を防止するためにe^xのx<=20にクリップ

	auto
		win = winRateRestriction.lock(),
		lat = lateralInhibition.lock(),
		pe = penalty.lock();
	if (win != nullptr && lat != nullptr && pe != nullptr)
		pe->array() =
		(
			(win->array() * _coeWinPenalty -/*マイナス*/lat->array() * _coeLatPenalty) / (mpe_count.cast<float_t>().array() + 1.f)
		)
		.min(20.f)	// expが発散しないようにクリップ
		.exp();
	else
		throw MonjuException(ErrorCode::WeakPointerExpired);
}

void monju::PenaltyCalcTask::_loadWinCount(MatrixRm<int32_t>& mpe_count)
{
	// ユニットの勝った回数をファイルから取得する
	// mpe_countの形状　行=ノード、列=ユニット

	if (auto p = _pStorage.lock())
	{
		for (int node = 0; node < kNumNodes; node++)
		{
			for (int unit = 0; unit < kNumUnits; unit++)
				mpe_count.coeffRef(node, unit) = p->readElement(node, node, unit, unit);
		}
	}
	else
		throw MonjuException(ErrorCode::WeakPointerExpired);
	// 0除算防止で一律加算
	mpe_count.array() += 1;
}

void monju::PenaltyCalcTask::_calcWinRateDistribution(MatrixRm<int32_t>& mpe_count, MatrixRm<float_t>& prob)
{
	// カウンティングから分布を推定する。
	// mpe_count 行=ノードI、列=ユニットJ
	// Σ_i P(i) = 1
	// P(i,j) = C(i,j) / Σ_i C(i,j))

	prob = mpe_count.cast<float_t>();
	prob.array() += 0.001f;
	prob.array().colwise() /= prob.rowwise().sum().array(); // 合ってるのか？直感ではprob.colwise().array()...あってる？
}

void monju::PenaltyCalcTask::_calcWinKLDistance(MatrixRm<int32_t>& mpe_count, MatrixRm<float_t>& message)
{
	// カウンティングデータから各ノードのKL距離を計算
	// 基準の分布は全ユニットの勝率が同じ

	// ノードごとに各ユニットの勝率を計算
	MatrixRm<float_t> win_prob;
	_calcWinRateDistribution(mpe_count, /*out*/win_prob);

	// KL距離を計算
	// 目標勝率
	const float_t Q = static_cast<float_t>(kNumActiveNodes) / static_cast<float_t>(kNumNodes * kNumUnits);
	message = win_prob;
	message.array() = Q / message.array();
	message.array() = message.array() * message.array().log() * kNumUnits;
}

void monju::PenaltyCalcTask::_calcLateralMutualInfo(MatrixRm<float_t>& restriction)
{
	// 相互情報量を計算

	MatrixRm<int32_t> count(kNumNodes, kNumUnits);
	restriction.resizeLike(count);
	restriction.setZero();

	// ノードのすべての組み合わせに対して相互情報量を計算
	for (int U = 0; U < kNumNodes; U++)
	{
		for (int V = 0; V < U; V++)
			_sumLateralMutualInfo(U, V, count, /*out*/restriction);
	}
	// restriction.array() = coefficient_lat_penalty * restriction.array() / (count.array() + 1).cast<float_t>();
}

// ノードU、Vの側抑制ペナルティを計算

void monju::PenaltyCalcTask::_sumLateralMutualInfo(int U, int V, MatrixRm<int32_t>& count, MatrixRm<float>& restriction)
{
	// 相互情報量を計算して積算

	// カウンティングデータ取得
	if (auto p = _pStorage.lock())
	{
		MatrixRowMajorAccessor<int32_t> a(count);
		p->readCell(a, U, V);
	}
	else
		throw MonjuException(ErrorCode::WeakPointerExpired);
	// logの計算で-infへの発散防止
	count.array() += 1;

	// カウンティングデータから同時確率分布を計算
	MatrixRm<float_t> mui = count.cast<float_t>();
	mui = mui / mui.sum();
	// 周辺確率分布を計算
	auto margin_prob_U = mui.rowwise().sum(); // P(u)=Σ_v P(u,v)
	auto margin_prob_V = mui.colwise().sum(); // P(v)=Σ_u P(u,v)
											  // 同時確率分布を相互情報量で上書き I(U=u,V=v) の行列を計算
											  //   s^2(P(u|v)/P(u))(log(P(u|v)/P(u))
											  // = s^2(P(u,v)/P(u)P(v))(log(P(u,v)/P(u)P(v))
	mui.array() /= (margin_prob_U * margin_prob_V).array(); // P(u,v)/P(u)P(v)
	mui.array() *= mui.array().log() * std::pow(static_cast<float>(kNumUnits), 2.f);
	restriction.row(U) += mui.rowwise().sum(); // I(U=u,V)
	restriction.row(V) += mui.colwise().sum(); // I(U,V=v)
}
