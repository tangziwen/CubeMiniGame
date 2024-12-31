#pragma once
#include <unordered_map>
#include <string>
namespace tzw
{
class BlackBoard
{

public:


	bool hasKey(const std::string & key)
	{
		return m_data.find(key) != m_data.end();
	}

	void add(const std::string & key, void * data)
	{
		m_data[key] = data;
	}

	void * get(const std::string & key)
	{
		if(!hasKey(key))
		{
			return nullptr;
		}
		return m_data[key];

	}

	template<typename T> 
	void writeData(const std::string & key, const T& val)
	{
		if(!hasKey(key))
		{
			T* newVal = new T();
			add(key, newVal);
		}
		*(T *)(get(key)) = val;
	}

	template<typename T> 
	T getData(const std::string & key, T defaultValue)
	{
		if(!hasKey(key))
		{
			return defaultValue;
		}
		T val = *((T*)get(key));
		return val;
	}

private:

	std::unordered_map<std::string, void *> m_data;

};


}