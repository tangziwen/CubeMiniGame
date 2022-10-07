#include "RLShopMgr.h"
#include "RLPerk.h"
#include "RLWorld.h"
#include "2D/GUISystem.h"
#include "Engine/Engine.h"
#include "RLPlayerState.h"
#include "RLDirector.h"
#include <algorithm>
#include "Utility/math/TbaseMath.h"
namespace tzw
{
std::vector<std::string> tier0 = {"SMGBasic", "RifleBasic", "ShotGunBasic"};
std::vector<std::string> tier1 = {"SMGAdvance", "RifleAdvance", "ShotGunAdvance"};
std::vector<std::string> tier2 = {"SMGElite", "RifleElite", "ShotGunElite"};
RLShopMgr::RLShopMgr()
{
}

void RLShopMgr::init()
{
	auto& re = TbaseMath::getRandomEngine();
	std::shuffle(tier0.begin(), tier0.end(), re);
	std::shuffle(tier1.begin(), tier1.end(), re);
	std::shuffle(tier2.begin(), tier2.end(), re);
	std::vector<RLWeaponData*> tmp0;
	for(int i = 0; i < 2; i++) tmp0.push_back(RLWeaponCollection::shared()->getWeaponData( tier0[i]));
	std::vector<RLWeaponData*> tmp1;
	for(int i = 0; i < 2; i++) tmp1.push_back(RLWeaponCollection::shared()->getWeaponData( tier1[i]));
	std::vector<RLWeaponData*> tmp2;
	for(int i = 0; i < 2; i++) tmp2.push_back(RLWeaponCollection::shared()->getWeaponData( tier2[i]));
	m_weaponTierList.push_back(tmp0);
	m_weaponTierList.push_back(tmp1);
	m_weaponTierList.push_back(tmp2);

	m_weaponSold.push_back(0);
	m_weaponSold.push_back(0);
	m_weaponSold.push_back(0);

}

void RLShopMgr::open(int subwaveIdx)
{
	if(!m_isOpen)
	{
		m_currWave = subwaveIdx;
		RLWorld::shared()->goToPerk();
		RLPerkMgr::shared()->generatePerkCandidates();
		m_isOpen = true;
	}
}

bool RLShopMgr::isOpen()
{
	return m_isOpen;
}
void RLShopMgr::close()
{
	m_isOpen = false;
}

#define IF_IN_RANGE(A, MIN_VAL, MAX_VAL) if( A >= MIN_VAL && A < MAX_VAL)

#pragma optimize("", off)
void RLShopMgr::draw()
{

	if(!RLShopMgr::shared()->isOpen()) return;
	auto screenSize = Engine::shared()->winSize();
	ImGui::SetNextWindowPos(ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImVec2(0.5, 0.5));

	bool isFinishedBuy = false;
	ImGui::SetNextWindowSize(ImVec2(400, 600));
	if (ImGui::Begin("PerkSelect!!!",0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize  | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{
		int weaponTierIdx = -1;
		ImGui::Text("Weapon Sell");
		//if(m_currWave  >=1 && m_currWave <5)
		IF_IN_RANGE(m_currWave, 0, 5)
		{
			weaponTierIdx = 0;
		}
		else 
		IF_IN_RANGE(m_currWave, 5, 9)
		{
			weaponTierIdx = 1;
		}
		else 
		IF_IN_RANGE(m_currWave, 9, 14)
		{
			weaponTierIdx = 1;
		}
		if(weaponTierIdx >= 0)
		{
			bool isSoldOut = m_weaponSold[weaponTierIdx];
			if(!isSoldOut)
			{
				for(int i = 0; i < m_weaponTierList[weaponTierIdx].size(); i ++)
				{
					RLWeaponData * weapon = m_weaponTierList[weaponTierIdx][i];
					ImGui::PushID(weapon);

					char tmpStr[128];
					sprintf(tmpStr, "%s",weapon->m_name.c_str());

					if(ImGui::RadioButton(tmpStr, currSelect == weapon))
					{
						currSelect = weapon;
						selectType = 0;
					}
					ImGui::SameLine();

					ImGui::PopID();
				}
			}
			else
			{
				ImGui::Text("Sold Out");
			}
		}
		else
		{
			ImGui::Text("NO Weapon Sell Not");
		}


		ImGui::Spacing();
		ImGui::Text("Perk Sell");
		RLPerkInstContainer * perkContainer = RLPlayerState::shared()->getPerkContainer();
		std::vector<RLPerk *> &perkList =  RLPerkMgr::shared()->getPerkCandidates();

		for(int i = 0; i < perkList.size(); i ++)
		{
			RLPerk * perk = perkList[i];
			ImGui::PushID(perk);
			RLPerkInstance * perkInstance = perkContainer->hasPerk(perk);
			char tmpStr[128];
			if(perkInstance)
			{
				sprintf(tmpStr, "%s(Lv:%d)",perk->title.c_str(), perkInstance->level + 1);
			}
			else
			{
				sprintf(tmpStr, "%s",perk->title.c_str());
			}
			
			if(ImGui::RadioButton(tmpStr, currSelect == perk))
			{
				currSelect = perk;
				selectType = 1;
			}
			if(i%2 == 0) ImGui::SameLine();
			ImGui::PopID();
		}


		if(ImGui::Button("Buy"))
		{
			if(currSelect)
			{
				isFinishedBuy = true;
				if(selectType == 0)
				{
					m_weaponSold[weaponTierIdx] = 1;
				}
				else if(selectType == 1)
				{
					RLPlayerState::shared()->getPerkContainer()->addPerk(static_cast<RLPerk*>(currSelect));
				}
				
			}

		}

		ImGui::SameLine();
		if(ImGui::Button("Recover HP"))
		{
			isFinishedBuy = true;
		}
		if(currSelect)
		{
			if(selectType == 0)
			{
				ImGui::Text("Desc: Weapon %s", static_cast<RLWeaponData*>(currSelect)->m_name.c_str());
			}
			else if(selectType == 1)
			{
				ImGui::Text("Desc: %s", static_cast<RLPerk*>(currSelect)->desc.c_str());
			}
			
		}
		
		if(isFinishedBuy)
		{
			close();
			RLDirector::shared()->startNextSubWave();
		}
		ImGui::End();
	}
}
}