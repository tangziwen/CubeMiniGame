#pragma once
#include "Math/vec2.h"
#include "2D/SpriteInstanceMgr.h"
namespace tzw
{
	class RLInteraction
	{
	public:
		RLInteraction(vec2 pos);
		virtual ~RLInteraction();
		virtual void use();
		virtual void tick(float dt);
		virtual void initGraphics();
		bool isInteractiveable();
		bool getIsAlive() {return m_isAlive;}
	protected:
		SpriteInstanceInfo * m_sprite = nullptr;
		vec2 m_pos;
		bool m_isInteractiveable = false;
		bool m_isAlive = true;
	};

	class RLStair : public RLInteraction
	{
	public:
		RLStair(vec2 pos):RLInteraction(pos){};
		virtual void use() override;
	};

	struct RLRewardItemData
	{
		enum class ItemType
		{
			StatsMod,
			Weapon
		};
		std::string m_name;
		ItemType m_type;
		std::string m_res;
		int m_stats[3] = {0, 0, 0};
	};

	class RLRewardItemMgr : public Singleton<RLRewardItemMgr>
	{
	public:
		RLRewardItemMgr() = default;
		void loadConfig();
		RLRewardItemData * getNextItem();
	private:
		std::vector<RLRewardItemData> m_data;
	};


	class RLReward : public RLInteraction
	{
	public:
		RLReward(RLRewardItemData * itemData, vec2 pos):RLInteraction(pos),m_data(itemData){};
		virtual void initGraphics() override;
		virtual void use() override;

	private:
		RLRewardItemData * m_data;
	};

}