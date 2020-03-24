// ConsoleTest.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#include <set>
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/PenaltyCalculation.h"
#include "monju/util_math.h"
#include "monju/BayesianNodeStat.h"
#include "monju/BayesianFullConnectEdgeStat.h"
#include "monju/BayesianNode.h"
#include "monju//BayesianFullConnectEdge.h"
#include "monju/FullConnectedGridCpt.h"
#include "monju/BayesianFullConnectEdgeDevice.h"
#include "monju/BayesianNodeDevice.h"
#include "monju/BayesianFullConnectEdgeFmc.h"
#include "monju/BayesianNodeFmc.h"
#include "monju/Environment.h"
#include "monju/VariableKind.h"

void test1();
void test2();
void test3();

int main()
{
	test3();
}

void test1()
{
	auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
	storage->create(R"(C:\dev\test\cout_test.mat)", 2, 2, 2);
	monju::MatrixRm<int32_t> m(2, 2);
	m.setConstant(1);
	monju::GridMatrixRowMajorAccessor<int32_t> acc(2, 2, 2, 2, m);
	storage->writeCell(acc, 0, 0);
	storage->writeCell(acc, 1, 0);
	storage->writeCell(acc, 1, 1);

	monju::PenaltyCalculation task(2, 2, 2, storage, 100.f, 100.f);

	auto win = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
	auto lat = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
	auto penalty = std::make_shared <monju::MatrixRm<float_t>>(2, 2);

	task.calcPenalty(win, lat, penalty);
}

void test2()
{
	monju::MatrixRm<float_t> cpt(4,4);
	cpt << 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16;
	std::cout << cpt << std::endl << "==========" << std::endl;
	for (auto row : cpt.rowwise())
	{
		monju::MatrixRm<float_t>::Index rowIndex, maxarg;
		row.maxCoeff(&rowIndex, &maxarg);
		std::cout << rowIndex << ", " << maxarg << std::endl;
	}
}

template <typename Matrix>
void console_out(std::string name, std::weak_ptr<Matrix> wp)
{
	auto p = wp.lock();
	std::cout << name << std::endl;
	std::cout << *p << std::endl;
}

void test3()
{
	std::vector<monju::UniformBasisShape> network{
	{{4, 1}, 4, 4},
	{{4, 1}, 4, 4},
	{{4, 1}, 4, 4}
	};
	monju::Environment env(R"(C:\dev\test)");

	// CPUの記憶域確保
	monju::BayesianNode inputLayer("input-layer", network[0]);
	monju::BayesianNode layer1("layer1", network[1]);
	monju::BayesianNode layer2("layer2", network[2]);
	inputLayer.initRandom();
	layer1.initRandom();
	layer2.initRandom();

	monju::BayesianFullConnectEdge inputEdge("input-edge", network[1], network[0], 4.0f, 4.0f);
	monju::BayesianFullConnectEdge edge1("edge1", network[2], network[1], 4.0f, 4.0f);
	inputEdge.initRandom();
	edge1.initRandom();

	// 統計情報
	monju::BayesianNodeStat nodeStat1("stat1", network[1], 5.0f, 10.f);
	monju::BayesianNodeStat nodeStat2("stat2", network[2], 5.0f, 10.f);
	//monju::BayesianFullConnectEdgeStat edgeStat();

	nodeStat1.create(env.info().workFolder);
	nodeStat2.create(env.info().workFolder);
	{
		// GPU使用準備
		std::shared_ptr<monju::ClMachine> clMachine = std::make_shared<monju::ClMachine>(env.info().platformId);

		// GPUの記憶域確保
		monju::BayesianNodeDevice inputLayerDevice(clMachine, inputLayer);
		monju::BayesianNodeDevice layerDevice1(clMachine, layer1);
		monju::BayesianNodeDevice layerDevice2(clMachine, layer2);
		monju::BayesianFullConnectEdgeDevice inputEdgeDevice(clMachine, inputEdge);
		monju::BayesianFullConnectEdgeDevice edgeDevice1(clMachine, edge1);

		// GPUの計算資源
		monju::BayesianFullConnectEdgeFmc interNodeCmp1(network[1], network[0], env, clMachine);
		monju::BayesianFullConnectEdgeFmc interNodeCmp2(network[2], network[1], env, clMachine);
		monju::BayesianNodeFmc nodeInput(network[0], env, clMachine);
		monju::BayesianNodeFmc nodeCmp1(network[1], env, clMachine);
		monju::BayesianNodeFmc nodeCmp2(network[2], env, clMachine);

		// デバイス選択
		auto deviceId = clMachine->deviceIds().at(0);
		// GPUの割り当て
		std::shared_ptr<monju::ClDeviceContext> dc = std::make_shared<monju::ClDeviceContext>(clMachine, deviceId);

		// 非同期処理
		monju::ClEventJoiner joiner;

		// GPUメモリ転送
		inputLayerDevice.clVariableSet().enqueueWriteAll(dc, &joiner);
		layerDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);
		layerDevice2.clVariableSet().enqueueWriteAll(dc, &joiner);
		inputEdgeDevice.clVariableSet().enqueueWriteAll(dc, &joiner);
		edgeDevice1.clVariableSet().enqueueWriteAll(dc, &joiner);

		console_out("pi-1", inputLayer.pi());
		console_out("lambda-1", inputLayer.lambda());
		console_out("rho-1", inputLayer.rho());
		console_out("BEL-1", inputLayer.bel());
		console_out("win-1", inputLayer.win());

		console_out("pi-2", layer1.pi());
		console_out("lambda-2", layer1.lambda());
		console_out("rho-2", layer1.rho());
		console_out("BEL-2", layer1.bel());
		console_out("win-2", layer1.win());

		console_out("kappa-e1", edge1.kappa());
		console_out("lambda-e1", edge1.lambda());

		std::cout << "=========================================================================" << std::endl;

		joiner.join();
		for (int n = 0; n < 100; n++)
		{
			interNodeCmp1.both(dc, layerDevice1, inputLayerDevice, inputEdgeDevice, &joiner);
			//interNodeCmp2.both(dc, layerDevice2, layerDevice1, edgeDevice1, &joiner);
			nodeInput.bel(dc, inputLayerDevice, &joiner);
			nodeCmp1.bel(dc, layerDevice1, &joiner);
			//nodeCmp2.bel(dc, layerDevice2, &joiner);

			joiner.join();

			// 確認
			inputLayerDevice.clVariableSet().enqueueReadAll(dc, &joiner);
			layerDevice1.clVariableSet().enqueueReadAll(dc, &joiner);
			//layerDevice2.clVariableSet().enqueueReadAll(dc, &joiner);
			edgeDevice1.clVariableSet().enqueueReadAll(dc, &joiner);

			console_out("pi-1", inputLayer.pi());
			console_out("lambda-1", inputLayer.lambda());
			console_out("r-1", inputLayer.r());
			console_out("rho-1", inputLayer.rho());
			console_out("BEL-1", inputLayer.bel());
			console_out("win-1", inputLayer.win());

			console_out("pi-2", layer1.pi());
			console_out("lambda-2", layer1.lambda());
			console_out("r-2", layer1.r());
			console_out("rho-2", layer1.rho());
			console_out("BEL-2", layer1.bel());
			console_out("win-2", layer1.win());

			console_out("kappa-e1", edge1.kappa());
			console_out("lambda-e1", edge1.lambda());

			std::cout << "=========================================================================" << std::endl;

			joiner.join();

			auto win1 = layer1.win();
			//auto win2 = layer2.win();
			auto future1 = nodeStat1.accumulate(win1);
			//auto future2 = nodeStat2.accumulate(win2);

			// TODO 入力層と接続するCPTの学習
			bool a = layer1.containsNan();
			bool b = layer2.containsNan();
			bool c = edge1.containsNan();
			edge1.cpt().train(layer2.win(), layer1.win(), edge1.lambda(), 0.1);	// 学習

			if (n % 10 == 0)
			{
				future1.wait();
				//future2.wait();
				auto f1 = nodeStat1.calcPenalty();
				//auto f2 = nodeStat2.calcPenalty();
				f1.wait();
				//f2.wait();

				monju::util_eigen::copy(nodeStat1.penalty(), layer1.r());
				//monju::util_eigen::copy(nodeStat2.penalty(), layer2.r());

				layerDevice1.clVariableSet().enqueueWrite(dc, &joiner, monju::VariableKind::R);
				//layerDevice2.clVariableSet().enqueueWrite(dc, &joiner, monju::VariableKind::R);

				joiner.join();
			}
		}
	}

}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
