#pragma once
#ifndef _MONJU_MATRIX_H__
#define _MONJU_MATRIX_H__

#include "Eigen/Dense"
#include "nlohmann/json.hpp"
#include <map>

namespace monju {

	/// <summary>�s�x�N�g���i�������j</summary>
	template <typename T>
	using VectorRm = Eigen::Matrix<T, 1, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>��x�N�g���i�c�����j</summary>
	template <typename T>
	using VectorCm = Eigen::Matrix<T, Eigen::Dynamic, 1, Eigen::ColMajor>;

	/// <summary>�s�D��̍s��</summary>
	template <typename T>
	using MatrixRm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

	/// <summary>��D��̍s��</summary>
	template <typename T>
	using MatrixCm = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;

	/// <summary>nlohmann��JSON�N���X</summary>
	using json = nlohmann::json;

	using params = std::map<std::string, std::string>;

	// �j���\��
	union BasisWorkItemCalcPlan
	{
		int64_t n;
		struct {
			uint32_t adjacent_index; // �e��ʊ��m�[�h�i��j�̗אڂ��鉺�ʊ��m�[�h�i�s�j�̃C���f�b�N�X�iID�j
			uint32_t index; // ���[�J�����[�N�A�C�e���̃C���f�b�N�X
		} plan;
	};
}

#endif // !_MONJU_MATRIX_H__

