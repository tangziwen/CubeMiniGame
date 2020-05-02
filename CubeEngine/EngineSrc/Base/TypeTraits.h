#pragma once
namespace tzw
{
#define TO_STR(SYMBOL_NAME) (#SYMBOL_NAME)
	
	template<typename T>
	const char * getCStr(T t)
	{
          return TO_STR(t);
	}
	template<>
	inline const char * getCStr(char * c)
	{
		return c;
	}
	template<>
	inline const char * getCStr(std::string str)
	{
		return str.c_str();
	}
	
	template <typename t>
	struct isBool
	{
		const static bool value = false;
	};
	template <>
	struct isBool<bool>
	{
		const static bool value = true;
	};

	template <typename t>
	struct isFloat
	{
		const static bool value = false;
	};
	template <>
	struct isFloat<float>
	{
		const static bool value = true;
	};

	template <typename t>
	struct isInt
	{
		const static bool value = false;
	};
	template <>
	struct isInt<int>
	{
		const static bool value = true;
	};

	template <typename t>
	struct isString
	{
		const static bool value = false;
	};
	template <>
	struct isString<const char *>
	{
		const static bool value = true;
	};
	template <>
	struct isString<char *>
	{
		const static bool value = true;
	};
	template <>
	struct isString<std::string>
	{
		const static bool value = true;
	};
	
	template <typename t>
	struct isCstring
	{
		const static bool value = false;
	};

	template <>
	struct isCstring<char *>
	{
		const static bool value = true;
	};

	template <>
	struct isCstring<const char *>
	{
		const static bool value = true;
	};
	template <>
	struct isCstring<unsigned char *>
	{
		const static bool value = true;
	};

	template <>
	struct isCstring<const unsigned char *>
	{
		const static bool value = true;
	};
}
