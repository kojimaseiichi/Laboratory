#pragma once
#ifndef _MONJU_BEL_LAYER_STORAGE_H__
#define _MONJU_BEL_LAYER_STORAGE_H__

#include "Extent.h"

namespace monju
{
	class GridMatrixStorage;
	class BelLayerStorage
	{
	private:
		LayerShape _shape;
		std::unique_ptr<GridMatrixStorage> _ps;
		const std::string _VAR_LAMBDA = "lambda";
		const std::string _VAR_PI= "pi";
		const std::string _VAR_RHO = "rho";
		const std::string _VAR_R = "r";
		const std::string _VAR_BEL = "bel";
		const std::string _VAR_WIN = "win";

#pragma region Constructor
	public:
		BelLayerStorage(std::string fileName, LayerShape shape);
		~BelLayerStorage();
#pragma endregion Constructor
#pragma region Public Method
	public:
		void prepareAll();
#pragma endregion Public Method
#pragma region Public Properties
	public:
		void persistLambda(bool storing, MatrixRm<float_t>& lambda);
		void persistPi(bool storing, MatrixRm<float_t>& pi);
		void persistRho(bool storing, MatrixRm<float_t>& rho);
		void persistR(bool storing, MatrixRm<float_t>& r);
		void persistBEL(bool storing, MatrixRm<float_t>& bel);
		void persistWin(bool storing, MatrixRm<int32_t>& win);

#pragma endregion Public Properties
#pragma region Helper Method
	private:
		void _prepareStorage();

#pragma endregion Helper Method

	};
}

#endif // !_MONJU_BEL_LAYER_STORAGE_H__
