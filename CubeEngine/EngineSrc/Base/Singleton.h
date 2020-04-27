#pragma once
namespace  tzw
{
	template<typename T>
	class Singleton
	{
		public:
		static T* shared()
		{
			static T * m_instance = nullptr;
			if (!m_instance) 
			{
				m_instance = new T();
			}
			return m_instance;
		}
	};

}
