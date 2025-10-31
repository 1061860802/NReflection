#pragma once
#include "Type.h"

class ReferenceBase : public Type
{
public:
	ReferenceBase(const std::string& name, const std::string& realname, const type_info& type_info_,size_t size, const Variable_Type_Container<Type> Inner) : Type(name, realname,type_info_, size), Inner(Inner)
	{
		return;
	};
	const Variable_Type_Container<Type> Inner;
	virtual bool InputRequire(bool SelfConst, const Type& other, bool OtherConst) const
	{
		if (!other.bUseful())
			return false;
		if (other == *this)
		{
			return true;
		}
		if (Inner.GetTypeConst() == other)
		{
			if (!Inner.bConst() && OtherConst)
				return false;
			return true;
		}
		if (const ReferenceBase* RefBasePtr = dynamic_cast<const ReferenceBase*>(&other))
		{
			const ReferenceBase& RefBase = *RefBasePtr;
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
class Reference;