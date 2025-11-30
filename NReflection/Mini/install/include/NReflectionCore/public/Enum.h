#pragma once
#include "Type.h"
#include "EnumBase.h"
#include "Enumerator.h"

namespace NLEngine
{
	template<typename T>
	class TEnum final : public CEnumBase
	{
		using EnumT = typename T;
		using UnderlyingTypeT = typename std::underlying_type<EnumT>::type;
	public:
		TEnum() : CEnumBase(&GetType<UnderlyingTypeT>(), TTypeName<EnumT>::Name, typeid(EnumT).name(), typeid(EnumT), sizeof(EnumT)) {}
		virtual ~TEnum()
		{
			for (auto i : Enumerators)
			{
				delete i;
			}
		}
		TEnum& ApplyEnumerator(const T& Var, const std::string& Name, const std::vector<CEnumeratorMetaBase*>& IN_Metas = {})
		{
			auto* NewEnumerator = new TEnumerator<T>(Var, Name,*this,IN_Metas);
			Enumerators.push_back(NewEnumerator);
			return *this;
		}
	};
}