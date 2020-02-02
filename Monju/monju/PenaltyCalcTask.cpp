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
	// �J�E���e�B���O�f�[�^���珟���y�i���e�B�Ƒ��}���y�i���e�B���v�Z
	// �����y�i���e�B �� KL����
	// ���}���y�i���e�B �� -���ݏ���

	MatrixRm<int32_t> mpe_count;
	mpe_count.resize(kNumNodes, kNumUnits);

	// MPE�J�E���g���t�@�C������擾
	_loadWinCount(/*out*/mpe_count);

	// �����v�Z��KL�����v�Z�������y�i���e�B�v�Z�i�n�C�p�[�p�����^�j
	if (auto p = winRateRestriction.lock())
		_calcWinKLDistance(mpe_count, /*out*/*p);
	else
		throw MonjuException(ErrorCode::WeakPointerExpired);

	// ���������v�Z�����ݏ��ʌv�Z�����}���y�i���e�B�v�Z�i�n�C�p�[�p�����^�j
	if (auto p = lateralInhibition.lock())
		_calcLateralMutualInfo(/*out*/*p);
	else
		return false;

	// �y�i���e�B���v�Z
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
	// KL�����Ƒ������ʂ���w���֐��̒l���v�Z
	// �w���֐��̔��U��h�~���邽�߂�e^x��x<=20�ɃN���b�v

	auto
		win = winRateRestriction.lock(),
		lat = lateralInhibition.lock(),
		pe = penalty.lock();
	if (win != nullptr && lat != nullptr && pe != nullptr)
		pe->array() =
		(
			(win->array() * _coeWinPenalty -/*�}�C�i�X*/lat->array() * _coeLatPenalty) / (mpe_count.cast<float_t>().array() + 1.f)
		)
		.min(20.f)	// exp�����U���Ȃ��悤�ɃN���b�v
		.exp();
	else
		throw MonjuException(ErrorCode::WeakPointerExpired);
}

void monju::PenaltyCalcTask::_loadWinCount(MatrixRm<int32_t>& mpe_count)
{
	// ���j�b�g�̏������񐔂��t�@�C������擾����
	// mpe_count�̌`��@�s=�m�[�h�A��=���j�b�g

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
	// 0���Z�h�~�ňꗥ���Z
	mpe_count.array() += 1;
}

void monju::PenaltyCalcTask::_calcWinRateDistribution(MatrixRm<int32_t>& mpe_count, MatrixRm<float_t>& prob)
{
	// �J�E���e�B���O���番�z�𐄒肷��B
	// mpe_count �s=�m�[�hI�A��=���j�b�gJ
	// ��_i P(i) = 1
	// P(i,j) = C(i,j) / ��_i C(i,j))

	prob = mpe_count.cast<float_t>();
	prob.array() += 0.001f;
	prob.array().colwise() /= prob.rowwise().sum().array(); // �����Ă�̂��H�����ł�prob.colwise().array()...�����Ă�H
}

void monju::PenaltyCalcTask::_calcWinKLDistance(MatrixRm<int32_t>& mpe_count, MatrixRm<float_t>& message)
{
	// �J�E���e�B���O�f�[�^����e�m�[�h��KL�������v�Z
	// ��̕��z�͑S���j�b�g�̏���������

	// �m�[�h���ƂɊe���j�b�g�̏������v�Z
	MatrixRm<float_t> win_prob;
	_calcWinRateDistribution(mpe_count, /*out*/win_prob);

	// KL�������v�Z
	// �ڕW����
	const float_t Q = static_cast<float_t>(kNumActiveNodes) / static_cast<float_t>(kNumNodes * kNumUnits);
	message = win_prob;
	message.array() = Q / message.array();
	message.array() = message.array() * message.array().log() * kNumUnits;
}

void monju::PenaltyCalcTask::_calcLateralMutualInfo(MatrixRm<float_t>& restriction)
{
	// ���ݏ��ʂ��v�Z

	MatrixRm<int32_t> count(kNumNodes, kNumUnits);
	restriction.resizeLike(count);
	restriction.setZero();

	// �m�[�h�̂��ׂĂ̑g�ݍ��킹�ɑ΂��đ��ݏ��ʂ��v�Z
	for (int U = 0; U < kNumNodes; U++)
	{
		for (int V = 0; V < U; V++)
			_sumLateralMutualInfo(U, V, count, /*out*/restriction);
	}
	// restriction.array() = coefficient_lat_penalty * restriction.array() / (count.array() + 1).cast<float_t>();
}

// �m�[�hU�AV�̑��}���y�i���e�B���v�Z

void monju::PenaltyCalcTask::_sumLateralMutualInfo(int U, int V, MatrixRm<int32_t>& count, MatrixRm<float>& restriction)
{
	// ���ݏ��ʂ��v�Z���ĐώZ

	// �J�E���e�B���O�f�[�^�擾
	if (auto p = _pStorage.lock())
	{
		MatrixRowMajorAccessor<int32_t> a(count);
		p->readCell(a, U, V);
	}
	else
		throw MonjuException(ErrorCode::WeakPointerExpired);
	// log�̌v�Z��-inf�ւ̔��U�h�~
	count.array() += 1;

	// �J�E���e�B���O�f�[�^���瓯���m�����z���v�Z
	MatrixRm<float_t> mui = count.cast<float_t>();
	mui = mui / mui.sum();
	// ���ӊm�����z���v�Z
	auto margin_prob_U = mui.rowwise().sum(); // P(u)=��_v P(u,v)
	auto margin_prob_V = mui.colwise().sum(); // P(v)=��_u P(u,v)
											  // �����m�����z�𑊌ݏ��ʂŏ㏑�� I(U=u,V=v) �̍s����v�Z
											  //   s^2(P(u|v)/P(u))(log(P(u|v)/P(u))
											  // = s^2(P(u,v)/P(u)P(v))(log(P(u,v)/P(u)P(v))
	mui.array() /= (margin_prob_U * margin_prob_V).array(); // P(u,v)/P(u)P(v)
	mui.array() *= mui.array().log() * std::pow(static_cast<float>(kNumUnits), 2.f);
	restriction.row(U) += mui.rowwise().sum(); // I(U=u,V)
	restriction.row(V) += mui.colwise().sum(); // I(U,V=v)
}
