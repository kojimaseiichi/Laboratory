#include "pch.h"
#include "CppUnitTest.h"
#include "monju/GridMatrixStorage.h"
#include <boost/filesystem.hpp>

#include "monju/BelLayer.h"
#include "monju/BelLayerFmc.h"
#include "monju/BelLayerUpdaterFmc.h"
#include "monju/FullBayesianMatrixLayer.h"
#include "monju/FullBayesianMatrixLayerUpdaterFmc.h"
#include "monju/ClMachine.h"
#include "monju/ClEventJoiner.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace monju
{
	namespace test
	{
		TEST_CLASS(FullBayesianMatrixLayerTest)
		{
		private:
			std::shared_ptr<Environment> env;

			TEST_METHOD_INITIALIZE(Init)
			{
				env = std::make_shared<Environment>(R"(C:\monju\test\FullBayesianMatrixLayer)");
			}

			TEST_METHOD(TestHostMemory)
			{
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\x.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\y.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\m.dbm)");

				LayerShape shapeX(2, 2, 2, 2), shapeY(3, 3, 3, 3);
				GridExtent gextCpt(shapeX, shapeY), gextLambda(shapeX, shapeY), gextKappa(shapeX, shapeY);
				gextLambda.matrix.rows = 1;
				gextKappa.matrix.cols = 1;

				std::shared_ptr<BelLayer> lx = std::make_shared<BelLayer>(env, "x", shapeX);
				std::shared_ptr<BelLayer> ly = std::make_shared<BelLayer>(env, "y", shapeX);

				auto fextCpt = gextCpt.flattenCm();
				auto fextLambda = gextLambda.flattenCm();
				auto fextKappa = gextKappa.flattenCm();
				MatrixCm<float_t> cpt, lambda, kappa;
				cpt.resize(fextCpt.rows, fextCpt.cols);
				lambda.resize(fextLambda.rows, fextLambda.cols);
				kappa.resize(fextKappa.rows, fextKappa.cols);

				cpt.setRandom();
				lambda.setRandom();
				kappa.setRandom();

				{
					std::shared_ptr<FullBayesianMatrixLayer> ml = std::make_shared<FullBayesianMatrixLayer>(env, "m", shapeX, shapeY);
					{
						auto p = ml->cpt().lock();
						*p = cpt;
					}
					{
						auto p = ml->lambda().lock();
						*p = lambda;
					}
					{
						auto p = ml->kappa().lock();
						*p = kappa;
					}
					ml->store();
				}
				{
					std::shared_ptr<FullBayesianMatrixLayer> ml = std::make_shared<FullBayesianMatrixLayer>(env, "m", shapeX, shapeY);
					float prec = 0.001f;
					ml->load();
					{
						auto p = ml->cpt().lock();
						Assert::IsTrue(p->isApprox(cpt, prec));
					}
					{
						auto p = ml->lambda().lock();
						Assert::IsTrue(p->isApprox(lambda, prec));
					}
					{
						auto p = ml->kappa().lock();
						Assert::IsTrue(p->isApprox(kappa, prec));
					}
				}
			}
			TEST_METHOD(TestHostMemoryProbability)
			{
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\m.dbm)");

				LayerShape shapeX(2, 2, 2, 2), shapeY(3, 3, 3, 3);
				GridExtent gextCpt(shapeX, shapeY), gextLambda(shapeX, shapeY), gextKappa(shapeX, shapeY);
				gextLambda.matrix.rows = 1;
				gextKappa.matrix.cols = 1;
				
				std::shared_ptr<FullBayesianMatrixLayer> ml = std::make_shared<FullBayesianMatrixLayer>(env, "m", shapeX, shapeY);
				ml->initVariables();

				float prec = 0.001f;
				for (int col = 0; col < gextCpt.grid.cols; col++)
				{
					for (int row = 0; row < gextCpt.grid.rows; row++)
					{
						{
							auto p = ml->cpt().lock();
							auto b = p->block(row * gextCpt.matrix.size(), col, gextCpt.matrix.size(), 1).reshaped(gextCpt.matrix.rows, gextCpt.matrix.cols);
							float one = b.sum();
							Assert::IsTrue(std::abs(1.f - one) < prec);
						}
						{
							auto p = ml->lambda().lock();
							auto b = p->block(row * gextLambda.matrix.size(), col, gextLambda.matrix.size(), 1).reshaped(gextLambda.matrix.rows, gextLambda.matrix.cols);
							float one = b.sum();
							Assert::IsTrue(std::abs(1.f - one) < prec);
						}
						{
							auto p = ml->kappa().lock();
							auto b = p->block(row * gextKappa.matrix.size(), col, gextKappa.matrix.size(), 1).reshaped(gextKappa.matrix.rows, gextKappa.matrix.cols);
							float one = b.sum();
							Assert::IsTrue(std::abs(1.f - one) < prec);
						}
					}
				}
			}

			TEST_METHOD(TestDeviceMemory)
			{
				// CPUとGPUの計算結果を比較
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\x.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\y.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\m.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\x2.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\y2.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\FullBayesianMatrixLayer\m2.dbm)");

				LayerShape shapeX(2, 2, 1, 2), shapeY(3, 2, 1, 3);
				GridExtent gextCpt(shapeX, shapeY), gextLambda(shapeX, shapeY), gextKappa(shapeX, shapeY);
				gextLambda.matrix.rows = 1;
				gextKappa.matrix.cols = 1;
				const int platformId = 1;

				std::shared_ptr<ClMachine> machine = std::make_shared<ClMachine>(platformId);
				std::shared_ptr<ClDeviceContext> dc = std::make_shared<ClDeviceContext>(machine, machine->defaultDeviceId());
				std::shared_ptr<ClEventJoiner> join = std::make_shared<ClEventJoiner>();

				// ホストメモリ
				std::shared_ptr<BelLayer> lx = std::make_shared<BelLayer>(env, "x", shapeX);
				std::shared_ptr<BelLayer> ly = std::make_shared<BelLayer>(env, "y", shapeY);
				std::shared_ptr<FullBayesianMatrixLayer> m = std::make_shared<FullBayesianMatrixLayer>(env, "m", shapeX, shapeY);

				std::shared_ptr<BelLayer> lx2 = std::make_shared<BelLayer>(env, "x2", shapeX);
				std::shared_ptr<BelLayer> ly2 = std::make_shared<BelLayer>(env, "y2", shapeY);
				std::shared_ptr<FullBayesianMatrixLayer> m2 = std::make_shared<FullBayesianMatrixLayer>(env, "m2", shapeX, shapeY);
				// デバイスメモリ
				std::shared_ptr<FullBayesianMatrixLayerFmc> fmc_m = std::make_shared<FullBayesianMatrixLayerFmc>(machine, m);
				std::shared_ptr<BelLayerFmc> fmc_lx = std::make_shared<BelLayerFmc>(machine, lx);
				std::shared_ptr<BelLayerFmc> fmc_ly = std::make_shared<BelLayerFmc>(machine, ly);
				// カーネル
				std::shared_ptr<FullBayesianMatrixLayerUpdaterFmc> update = std::make_shared<FullBayesianMatrixLayerUpdaterFmc>(env, shapeX, shapeY, machine);
				std::shared_ptr<BelLayerUpdaterFmc> update_x = std::make_shared<BelLayerUpdaterFmc>(env, shapeX, machine);
				std::shared_ptr<BelLayerUpdaterFmc> update_y = std::make_shared<BelLayerUpdaterFmc>(env, shapeY, machine);

				// 変数を乱数的に初期化
				lx->initVariables();
				ly->initVariables();
				m->initVariables();

				// コピー
				lx2->copyData(*lx);
				ly2->copyData(*ly);
				m2->copyData(*m);

				float prec = 0.001f;

				// UP
				// ホスト
				m2->performUp(*lx2, *ly2);
				lx2->performBel();
				// デバイス
				fmc_m->clVariableSet().enqueueWriteAll(dc, join);
				fmc_lx->clVariableSet().enqueueWriteAll(dc, join);
				fmc_ly->clVariableSet().enqueueWriteAll(dc, join);
				update->up(dc, fmc_lx, fmc_ly, fmc_m, join);
				update_x->bel(dc, fmc_lx, join);
				fmc_m->clVariableSet().enqueueReadAll(dc, join);
				fmc_lx->clVariableSet().enqueueReadAll(dc, join);
				fmc_ly->clVariableSet().enqueueReadAll(dc, join);
				join->join();
				// 検証
				{
					auto a = m->lambda().lock();
					auto b = m2->lambda().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = lx->lambda().lock();
					auto b = lx2->lambda().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = lx->rho().lock();
					auto b = lx2->rho().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = lx->bel().lock();
					auto b = lx2->bel().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = lx->win().lock();
					auto b = lx2->win().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}

				// DOWN
				// ホスト
				m2->performDown(*lx2, *ly2);
				ly2->performBel();
				// デバイス
				update->down(dc, fmc_lx, fmc_ly, fmc_m, join);
				update_y->bel(dc, fmc_ly, join);
				fmc_m->clVariableSet().enqueueReadAll(dc, join);
				fmc_lx->clVariableSet().enqueueReadAll(dc, join);
				fmc_ly->clVariableSet().enqueueReadAll(dc, join);

				// 検証
				{
					auto a = m->kappa().lock();
					auto b = m2->kappa().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = ly->lambda().lock();
					auto b = ly2->lambda().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = ly->pi().lock();
					auto b = ly2->pi().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = ly->rho().lock();
					auto b = ly2->rho().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = ly->bel().lock();
					auto b = ly2->bel().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
				{
					auto a = ly->win().lock();
					auto b = ly2->win().lock();
					Assert::IsTrue(a->isApprox(*b, prec));
				}
			}
		};
	}
}