#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__

#include "MonjuTypes.h"
#include "FullBayesianMatrixLayer.h"
#include "ClVariableSet.h"

namespace monju {

	// �}�g���b�N�X�w�̃f�o�C�X�̃�����������
	class FullBayesianMatrixLayerFmc
	{
	/*�t�B�[���h*/
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr< FullBayesianMatrixLayer> _edge;
		std::shared_ptr<ClMemory> 
			_clLambda, 
			_clKappa, 
			_clCpt;
		ClVariableSet _clVariableSet;

	/*�R���X�g���N�^*/
	public:
		FullBayesianMatrixLayerFmc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge);
		~FullBayesianMatrixLayerFmc();

	/*�v���p�e�B*/
	public:
		ClVariableSet& clVariableSet();

	/*�w���p�[*/
	private:
		void _lockOuterResources(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<FullBayesianMatrixLayer> edge);
		void _allocClMemory();

	/*�R�s�[�֎~�E���[�u�֎~*/
	public:
		FullBayesianMatrixLayerFmc(const FullBayesianMatrixLayerFmc&) = delete;
		FullBayesianMatrixLayerFmc(FullBayesianMatrixLayerFmc&&) = delete;
		FullBayesianMatrixLayerFmc& operator=(const FullBayesianMatrixLayerFmc&) = delete;
		FullBayesianMatrixLayerFmc& operator=(FullBayesianMatrixLayerFmc&&) = delete;
	};
}

#endif // !_MONJU_FULL_BAYESIAN_MATRIX_LAYER_FMC_H__
