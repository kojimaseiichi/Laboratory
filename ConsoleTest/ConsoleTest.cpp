// ConsoleTest.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#include <iostream>
#include "monju/MonjuTypes.h"
#include "monju/GridMatrixStorage.h"
#include "monju/TaskPenaltyCalc.h"

int main()
{
	auto storage = std::make_shared<monju::TriangularGridMatrixStorage<int32_t>>();
	storage->create(R"(C:\dev\test\cout_test.mat)", 2, 2, 2);
	monju::MatrixRm<int32_t> m(2, 2);
	m.setConstant(1);
	monju::GridMatrixRowMajorAccessor<int32_t> acc(2, 2, 2, 2, m);
	storage->writeCell(acc, 0, 0);
	storage->writeCell(acc, 1, 0);
	storage->writeCell(acc, 1, 1);

	monju::TaskPenaltyCalc task(2, 2, 2, storage, 100.f, 100.f);

	auto win = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
	auto lat = std::make_shared <monju::MatrixRm<float_t>>(2, 2);
	auto penalty = std::make_shared <monju::MatrixRm<float_t>>(2, 2);

	task.calcPenalty(win, lat, penalty);
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
