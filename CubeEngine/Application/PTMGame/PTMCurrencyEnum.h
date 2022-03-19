#pragma once

namespace tzw
{
	enum class PTMCurrencyEnum
	{
		//Raw 
		Gold = 0,
		Herb,
		Minerals,
		//
		Dan,

		//End Mark
		CurrencyMax
	};

	struct PTMCurrencySet
	{
		float m_currency[(int)PTMCurrencyEnum::CurrencyMax] {};
	};
}