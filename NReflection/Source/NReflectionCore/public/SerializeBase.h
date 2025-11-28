#pragma once
#include <iostream>
#include <string>

namespace NLEngine
{

	template<typename T, typename = void>
	struct has_tostring
	{
		static constexpr bool tostring = false;
	};

	template<typename T>
	struct has_tostring<T, std::void_t<decltype(std::to_string(std::declval<T>()))>>
	{
		static constexpr bool tostring = true;
	};

	template<typename T, typename = void>
	struct UseCustomPrint
	{
		static constexpr bool bUseCustomPrint = false;
	};

	template<typename T>
	struct UseCustomPrint<T, std::void_t<decltype(ToString(std::declval<T>()))>>
	{
		static constexpr bool bUseCustomPrint = true;
	};

}