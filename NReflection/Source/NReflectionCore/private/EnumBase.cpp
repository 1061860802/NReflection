#include "EnumBase.h"
#include "Enumerator.h"

NLEngine::CEnumBase::CEnumBase(CType* underlyingType, const std::string& Name, const std::string& realname, const type_info& type_info_, size_t size) : CType(Name, realname, type_info_, size), UnderlyingType(underlyingType)
{
}

NLEngine::CEnumBase::~CEnumBase()
{
	for (auto i : Enumerators)
	{
		delete i;
	}
}

NLEngine::CEnumeratorBase* NLEngine::CEnumBase::GetEnumerator(const std::string& Name) const
{
	for (auto i : Enumerators)
	{
		if (i->GetName() == Name)
		{
			return i;
		}
	}
	return nullptr;
}
