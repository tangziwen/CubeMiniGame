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

namespace tzw
{

}