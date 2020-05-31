#pragma once
#ifndef _MONJU_FULL_MATRIX_LAYER_UPDATER_FMC_H__
#define _MONJU_FULL_MATRIX_LAYER_UPDATER_FMC_H__

#include "MonjuTypes.h"
#include <boost/lexical_cast.hpp>
#include "BelLayerFmc.h"
#include "FullBayesianMatrixLayerFmc.h"
#include "util_file.h"
#include "ClMachine.h"
#include "ClFunc.h"
#include "Environment.h"
#include "Extent.h"

namespace monju {
	// fast massive compute
	class FullMatrixLayerUpdaterFmc
	{
	private: // OpenCL�̃t�@�C���ƃJ�[�l����
		const std::string
			_kSrcOobpUp1 = "oobp\\oobp3_full_up_1.cl",
			_kSrcOobpUp2 = "oobp\\oobp3_full_up_2.cl",
			_kSrcOobpDown1 = "oobp\\oobp3_full_down_1.cl",
			_kSrcOobpDown2 = "oobp\\oobp3_full_down_2.cl",
			_kKernelOobpUp1 = "oobp3_full_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpUp2 = "oobp3_full_up_2_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown1 = "oobp3_full_down_1_X${X}_Y${Y}_XU${XU}_YU${YU}",
			_kKernelOobpDown2 = "oobp3_full_down_2_X${X}_Y${Y}_XU${XU}_YU${YU}";

	private: // �X�g���[�W
		LayerStruct
			_shapeX,
			_shapeY;
		std::shared_ptr<Environment> _env;
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel>
			_kernelOobpUp1,
			_kernelOobpUp2,
			_kernelOobpDown1,
			_kernelOobpDown2;

	public: // �R���X�g���N�^
		FullMatrixLayerUpdaterFmc(
			LayerStruct shapeX,
			LayerStruct shapeY,
			std::weak_ptr<Environment> env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_captureExternalResources(shapeX, shapeY, env, clMachine);
			_createKernel();
		}
		~FullMatrixLayerUpdaterFmc()
		{
		}

	public: // ���J�����o
		void up(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerX,
			BelLayerFmc& layerY,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join)
		{
			// �S�����t����(up)�m���`�d(oobp)
			_up(clDeviceContext, layerX, layerY, matrixLayer, join);
			auto pDeviceContext = clDeviceContext.lock();
			pDeviceContext->flush();
		}

		void down(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& nodeX,
			BelLayerFmc& nodeY,
			FullBayesianMatrixLayerFmc& edge,
			std::weak_ptr<ClEventJoiner> join)
		{
			// �S����������(down)�m���`�d(oobp)
			_down(clDeviceContext, nodeX, nodeY, edge, join);
			auto pDeviceContext = clDeviceContext.lock();
			pDeviceContext->flush();
		}

		void both(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& nodeX,
			BelLayerFmc& nodeY,
			FullBayesianMatrixLayerFmc& edge,
			std::weak_ptr<ClEventJoiner> join)
		{
			// �S�����o�����m���`�d
			_up(clDeviceContext, nodeX, nodeY, edge, join);
			_down(clDeviceContext, nodeX, nodeY, edge, join);
			auto pDeviceContext = clDeviceContext.lock();
			pDeviceContext->flush();
		}

	private: // �w���p�[
		void _captureExternalResources(
			LayerStruct shapeX,
			LayerStruct shapeY,
			std::weak_ptr<Environment> env,
			std::weak_ptr<ClMachine> clMachine)
		{
			_shapeX = shapeX;
			_shapeY = shapeY;
			_env = env.lock();
			_clMachine = clMachine.lock();
		}
		void _createKernel()
		{
			param_map param_map; // �J�[�l���̃R���p�C�����e���v���[�g�ϐ�
			param_map["X"] = boost::lexical_cast<std::string>(_shapeX.nodes.size());
			param_map["Y"] = boost::lexical_cast<std::string>(_shapeY.nodes.size());
			param_map["XU"] = boost::lexical_cast<std::string>(_shapeX.units.size());
			param_map["YU"] = boost::lexical_cast<std::string>(_shapeY.units.size());

			std::filesystem::path kernelPathBase = _env->info().kernelFolder; // �J�[�l���t�@�C���̃t�H���_

			_kernelOobpUp1 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpUp1,
				_kKernelOobpUp1,
				param_map);

			_kernelOobpUp2 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpUp2,
				_kKernelOobpUp2,
				param_map);
			
			_kernelOobpDown1 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpDown1,
				_kKernelOobpDown1,
				param_map);
			
			_kernelOobpDown2 = std::make_shared<ClKernel>(
				_clMachine,
				kernelPathBase / _kSrcOobpDown2,
				_kKernelOobpDown2,
				param_map);
		}

		void _up(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerX,
			BelLayerFmc& layerY,
			FullBayesianMatrixLayerFmc& edge,
			std::weak_ptr<ClEventJoiner> join)
		{
			_upOobp1(clDeviceContext, layerY, edge, join);
			_upOobp2(clDeviceContext, layerX, edge, join);
		}

		void _upOobp1(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerY,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join)
		{
			ClFunc func1(_clMachine, _kernelOobpUp1);

			func1.pushArgument(layerY.clVariableSet().getClMemory(VariableKind::lambda));
			func1.pushArgument(layerY.clVariableSet().getClMemory(VariableKind::pi));
			func1.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::W));
			func1.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::kappa));
			func1.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::lambda));

			std::vector<size_t> global_work_size1 = { static_cast<size_t>(_shapeY.nodes.size()), static_cast<size_t>(_shapeX.nodes.size()) };
			std::vector<size_t> local_work_size1 = { 1, static_cast<size_t>(_shapeX.nodes.size()) };

			func1.execute(clDeviceContext, global_work_size1, local_work_size1, join);
			//edge.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::lambda);
		}

		void _upOobp2(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerX,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join)
		{
			ClFunc func2(_clMachine, _kernelOobpUp2);

			func2.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::lambda));
			func2.pushArgument(layerX.clVariableSet().getClMemory(VariableKind::R));
			func2.pushArgument(layerX.clVariableSet().getClMemory(VariableKind::lambda));

			std::vector<size_t> global_work_size2 = { 1, static_cast<size_t>(_shapeX.nodes.size()) };

			func2.execute(clDeviceContext, global_work_size2, join);
			//nodeX.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::lambda);
		}

		void _down(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& nodeX,
			BelLayerFmc& nodeY,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join)
		{
			_downOobp1(clDeviceContext, nodeX, matrixLayer, join);
			_downOobp2(clDeviceContext, nodeY, matrixLayer, join);
		}
		void _downOobp1(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerX,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join)
		{
			ClFunc func1(_clMachine, _kernelOobpDown1);

			func1.pushArgument(layerX.clVariableSet().getClMemory(VariableKind::rho));
			func1.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::W));
			func1.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::lambda));
			func1.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::kappa));

			std::vector<size_t> global_work_size1 = { static_cast<size_t>(_shapeY.nodes.size()) , static_cast<size_t>(_shapeX.nodes.size()) };
			std::vector<size_t> local_work_size1 = { static_cast<size_t>(_shapeY.nodes.size()), 1 };

			func1.execute(clDeviceContext, global_work_size1, local_work_size1, join);
			//edge.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::kappa);
		}
		void _downOobp2(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			BelLayerFmc& layerY,
			FullBayesianMatrixLayerFmc& matrixLayer,
			std::weak_ptr<ClEventJoiner> join)
		{
			ClFunc func2(_clMachine, _kernelOobpDown2);

			func2.pushArgument(matrixLayer.clVariableSet().getClMemory(VariableKind::kappa));
			func2.pushArgument(layerY.clVariableSet().getClMemory(VariableKind::pi));

			std::vector<size_t> global_work_size2 = { static_cast<size_t>(_shapeY.nodes.size()), 1 };

			func2.execute(clDeviceContext, global_work_size2, join);
			//nodeX.clVariableSet().enqueueRead(clDeviceContext, pJoin, VariableKind::pi);
		}


		// �R�s�[�֎~�E���[�u�֎~
	public:
		FullMatrixLayerUpdaterFmc(const FullMatrixLayerUpdaterFmc&) = delete;
		FullMatrixLayerUpdaterFmc(FullMatrixLayerUpdaterFmc&&) = delete;
		FullMatrixLayerUpdaterFmc& operator =(const FullMatrixLayerUpdaterFmc&) = delete;
		FullMatrixLayerUpdaterFmc& operator =(FullMatrixLayerUpdaterFmc&&) = delete;
	};
}

#endif // !_MONJU_FULL_MATRIX_LAYER_UPDATER_FMC_H__
