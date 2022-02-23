#pragma once

//property
#define PTM_PROPERTY(PROP, PROP_TYPE, DEFAULT_VAL, DESC) \
public: \
void set##PROP(PROP_TYPE val) { m_##PROP = val; }\
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

}