#pragma once

namespace tzw
{
	struct PTMTaxPack
	{
		float m_gold = 0.f;
		float m_adm = 0.f;
		void reset()
		{
			m_gold = 0.f;
			m_adm = 0.f;
		}
	};
}
