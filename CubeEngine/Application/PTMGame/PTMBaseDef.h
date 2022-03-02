#pragma once

//property
#define PTM_PROPERTY(PROP, PROP_TYPE, DEFAULT_VAL, DESC) \
public: \
void set##PROP(PROP_TYPE val) { m_##PROP = val; }\
PTM_PROPERTY_BASE(PROP, PROP_TYPE, DEFAULT_VAL, DESC)

#define PTM_PROPERTY_WITH_NOTIFY(PROP, PROP_TYPE, DEFAULT_VAL, DESC, NOTIFYCB) \
public: \
void set##PROP(PROP_TYPE val) { if( val != m_##PROP ) { NOTIFYCB(val); } m_##PROP = val; }\
PTM_PROPERTY_BASE(PROP, PROP_TYPE, DEFAULT_VAL, DESC)

#define PTM_PROPERTY_BASE(PROP, PROP_TYPE, DEFAULT_VAL, DESC) \
public: \
PROP_TYPE get##PROP() {return m_##PROP;}\
constexpr const char * get##PROP##DescString() {return DESC;}\
constexpr const char * get##PROP##String() {return #PROP;}\
protected:\
PROP_TYPE m_##PROP {DEFAULT_VAL};

//property with capacity eg. Food-> Food (getter & setter), FoodCapacityBase (getter & setter)
#define PTM_PROPERTY_WITH_CAPACITY(PROP, PROP_TYPE, DEFAULT_VAL, DEFAULT_CAPACITY_VALUE, DESC) \
public: \
void set##PROP(PROP_TYPE val) { m_##PROP = val; }\
PROP_TYPE get##PROP() {return m_##PROP;}\
constexpr const char * get##PROP##DescString() {return DESC;}\
constexpr const char * get##PROP##String() {return #PROP;}\
void set##PROP##Capacity##Base(PROP_TYPE val) { m_##PROP##Capacity##Base = val; }\
PROP_TYPE get##PROP##Capacity##Base() {return m_##PROP##Capacity##Base;}\
protected:\
PROP_TYPE m_##PROP {DEFAULT_VAL};\
PROP_TYPE m_##PROP##Capacity##Base {DEFAULT_CAPACITY_VALUE};

namespace tzw
{

struct PTMFiveElement
{
	int ElementMetal = 0;
	int ElementWood = 0;
	int ElementWater = 0;
	int ElementFire = 0;
	int ElementEarth = 0;

	void reset()
	{
		ElementMetal = 0;
		ElementWood = 0;
		ElementWater = 0;
		ElementFire = 0;
		ElementEarth = 0;
	}
	PTMFiveElement & operator +=(const PTMFiveElement & R)
	{
		ElementMetal += R.ElementMetal;
		ElementWood += R.ElementWood;
		ElementWater += R.ElementWater;
		ElementFire += R.ElementFire;
		ElementEarth += R.ElementEarth;
		return (*this);
	}

	PTMFiveElement operator +(const PTMFiveElement & R)
	{
		PTMFiveElement result;
		result.ElementMetal = ElementMetal + R.ElementMetal;
		result.ElementWood = ElementWood + R.ElementWood;
		result.ElementWater = ElementWater + R.ElementWater;
		result.ElementFire = ElementFire + R.ElementFire;
		result.ElementEarth = ElementEarth + R.ElementEarth;
		return result;
	}
	PTMFiveElement operator * (const float R) const
	{
		PTMFiveElement result;
		result.ElementMetal = ElementMetal * R;
		result.ElementWood = ElementWood * R;
		result.ElementWater = ElementWater * R;
		result.ElementFire = ElementFire * R;
		result.ElementEarth = ElementEarth * R;
		return result;
	}
};
}