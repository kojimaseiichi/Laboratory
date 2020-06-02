#include "pch.h"
#include "CppUnitTest.h"
#include "monju/GridMatrixStorage.h"
#include <boost/filesystem.hpp>

#include "monju/BelLayer.h"

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

			TEST_METHOD(Instantiate)
			{
				boost::filesystem::remove(R"(C:\monju\test\BelLayer\t1.dbm)");
				
				std::string id = "t1";
				MatrixRm<float_t> lambda, pi, rho, r, bel;
				MatrixRm<int32_t> win;

				LayerStruct shape(30, 30, 30, 30);
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
			TEST_METHOD(Probability)
			{
				boost::filesystem::remove(R"(C:\monju\test\BelLayer\t1.dbm)");
				std::string id = "t1";
				LayerStruct shape(5, 10, 5, 10);
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
		};
	}
}