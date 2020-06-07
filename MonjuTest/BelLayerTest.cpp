#include "pch.h"
#include "CppUnitTest.h"
#include "monju/GridMatrixStorage.h"
#include <boost/filesystem.hpp>

#include "monju/BelLayer.h"
#include "monju/BelLayerFmc.h"
#include "monju/BelLayerUpdaterFmc.h"
#include "monju/ClEventJoiner.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace monju
{
	namespace test
	{
		TEST_CLASS(BelLayerTest)
		{
		private:
			std::shared_ptr<Environment> env;

			TEST_METHOD_INITIALIZE(Init)
			{
				env = std::make_shared<Environment>(R"(C:\monju\test\BelLayer)");
			}

			TEST_METHOD(TestHostMemory)
			{
				boost::filesystem::remove(R"(C:\monju\test\BelLayer\t1.dbm)");
				
				std::string id = "t1";
				MatrixRm<float_t> lambda, pi, rho, r, bel;
				MatrixRm<int32_t> win;

				LayerShape shape(30, 30, 30, 30);
				Extent ext = shape.flatten();
				lambda.resize(ext.rows, ext.cols);
				pi.resize(ext.rows, ext.cols);
				rho.resize(ext.rows, ext.cols);
				r.resize(ext.rows, ext.cols);
				bel.resize(ext.rows, ext.cols);
				win.resize(ext.rows, 1);

				lambda.setRandom();
				pi.setRandom();
				rho.setRandom();
				r.setRandom();
				bel.setRandom();
				win.setRandom();
				{
					BelLayer belLayer(env, id, shape);
					belLayer.initVariables();
					{
						auto p = belLayer.lambda().lock();
						*p = lambda;
					}
					{
						auto p = belLayer.pi().lock();
						*p = pi;
					}
					{
						auto p = belLayer.rho().lock();
						*p = rho;
					}
					{
						auto p = belLayer.r().lock();
						*p = r;
					}
					{
						auto p = belLayer.bel().lock();
						*p = bel;
					}
					{
						auto p = belLayer.win().lock();
						*p = win;
					}
					belLayer.store();
				}

				{
					BelLayer belLayer(env, id, shape);
					belLayer.load();

					float spec = 0.001f;
					{
						auto pLambda = belLayer.lambda().lock();
						Assert::IsTrue(lambda.isApprox(*pLambda, spec));
					}
					{
						auto p = belLayer.pi().lock();
						Assert::IsTrue(pi.isApprox(*p, spec));
					}
					{
						auto p = belLayer.rho().lock();
						Assert::IsTrue(rho.isApprox(*p, spec));
					}
					{
						auto p = belLayer.r().lock();
						Assert::IsTrue(r.isApprox(*p, spec));
					}
					{
						auto p = belLayer.bel().lock();
						Assert::IsTrue(bel.isApprox(*p, spec));
					}
					{
						auto p = belLayer.win().lock();
						Assert::IsTrue(win.isApprox(*p, spec));
					}
				}
			}
			TEST_METHOD(TestHostMemoryProbability)
			{
				boost::filesystem::remove(R"(C:\monju\test\BelLayer\t1.dbm)");
				std::string id = "t1";
				LayerShape shape(5, 10, 5, 10);
				Extent ext = shape.flatten();
				BelLayer belLayer(env, id, shape);
				belLayer.initVariables();
				MatrixRm<float_t> one(ext.rows, 1);
				one.setOnes();
				float_t prec = 0.001f;
				{
					auto p = belLayer.lambda().lock();
					Assert::IsTrue(one.isApprox(p->rowwise().sum(), prec));
				}
				{
					auto p = belLayer.pi().lock();
					Assert::IsTrue(one.isApprox(p->rowwise().sum(), prec));
				}
				{
					auto p = belLayer.bel().lock();
					Assert::IsTrue(one.isApprox(p->rowwise().sum(), prec));
				}
			}

			TEST_METHOD(TestDeviceMemory)
			{
				// CPU‚ÆGPU‚ÌŒvŽZŒ‹‰Ê‚ð”äŠr

				boost::filesystem::remove(R"(C:\monju\test\BelLayer\t1.dbm)");
				boost::filesystem::remove(R"(C:\monju\test\BelLayer\t2.dbm)");
				std::string id = "t1";
				std::string id2 = "t2";
				LayerShape shape(5, 10, 5, 10);
				Extent ext = shape.flatten();
				const int platformId = 1;
				
				std::shared_ptr<ClMachine> machine = std::make_shared<ClMachine>(platformId);
				std::shared_ptr<ClDeviceContext> dc = std::make_shared<ClDeviceContext>(machine, machine->defaultDeviceId());
				std::shared_ptr<ClEventJoiner> join = std::make_shared<ClEventJoiner>();

				std::shared_ptr<BelLayer> layer = std::make_shared<BelLayer>(env, id, shape);
				std::shared_ptr<BelLayer> layer2 = std::make_shared<BelLayer>(env, id2, shape);
				std::shared_ptr<BelLayerFmc> fmc = std::make_shared<BelLayerFmc>(machine, layer);
				std::shared_ptr<BelLayerUpdaterFmc> update = std::make_shared<BelLayerUpdaterFmc>(env, shape, machine);

				
				layer->initVariables();
				layer2->copyData(*layer);
				layer2->performBel();
				layer2->findWinner();
				
				fmc->clVariableSet().enqueueWriteAll(dc, join);
				update->bel(dc, fmc, join);
				fmc->clVariableSet().enqueueReadAll(dc, join);

				float prec = 0.001f;
				{
					auto p1 = layer->lambda().lock();
					auto p2 = layer2->lambda().lock();
					Assert::IsTrue(p1->isApprox(*p2, prec));
				}
				{
					auto p1 = layer->pi().lock();
					auto p2 = layer2->pi().lock();
					Assert::IsTrue(p1->isApprox(*p2, prec));
				}
				{
					auto p1 = layer->r().lock();
					auto p2 = layer2->r().lock();
					Assert::IsTrue(p1->isApprox(*p2, prec));
				}
				{
					auto p1 = layer->rho().lock();
					auto p2 = layer2->rho().lock();
					Assert::IsTrue(p1->isApprox(*p2, prec));
				}
				{
					auto p1 = layer->bel().lock();
					auto p2 = layer2->bel().lock();
					Assert::IsTrue(p1->isApprox(*p2, prec));
				}
				{
					auto p1 = layer->win().lock();
					auto p2 = layer2->win().lock();
					auto v = (*p1)(0, 0);
					Assert::IsTrue(p1->isApprox(*p2, prec));
				}
			}
		};
	}
}