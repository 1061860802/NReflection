#pragma once
#include "Type.h"

namespace NLEngine
{

	class CReferenceBase : public CType
	{
	public:
		CReferenceBase(const std::string& IN_Name, const std::string& IN_RealName, const type_info& IN_TypeInfo, size_t IN_size, const TVariableTypeContainer<CType>& IN_Inner) : CType(IN_Name, IN_RealName, IN_TypeInfo, IN_size), Inner(IN_Inner)
		{
			return;
		};
		const TVariableTypeContainer<CType> Inner;
		virtual bool InputRequire(bool IN_SelfConst, const CType& IN_Other, bool IN_otherConst) const
		{
			if (!IN_Other.Useful())
				return false;
			if (IN_Other == *this)
			{
				return true;
			}
			if (Inner.GetTypeConst() == IN_Other)
			{
				if (!Inner.bConst() && IN_otherConst)
					return false;
				return true;
			}
			if (const CReferenceBase* RefBasePtr = dynamic_cast<const CReferenceBase*>(&IN_Other))
			{
				const CReferenceBase& RefBase = *RefBasePtr;
				if (!Inner.bConst() && RefBase.Inner.bConst())
					return false;
				if (Inner.GetTypeConst() == RefBase.Inner.GetTypeConst())
					return true;
			}
			return false;
		}
		virtual bool ConstRequire() const { return true; };
	};

	template<typename _InType_>
	class TReference;

}