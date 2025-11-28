#include "Type.h"
#include "Reference.h"
#include "Any.h"

using namespace NLEngine;

CType::~CType()
{
	for (auto i : Metas)
	{
		i->DetachFromType(this);
	}
}

void CType::ApplyMeta(CTypeMetaBase* IN_Meta)
{
	Metas.push_back(IN_Meta); 
	IN_Meta->AttachedtoType(this);
}

void CType::ApplyMetas(const std::vector<CTypeMetaBase*>& IN_Metas)
{
	for (auto i : IN_Metas)
	{
		Metas.push_back(i); 
		i->AttachedtoType(this);
	}
}

bool CType::InputRequire(bool SelfConst, const CType& other, bool OtherConst) const
{
	if (!other.Useful()) return false;
	if (this == &other) return true;
	if (const CReferenceBase* RefBasePtr = dynamic_cast<const CReferenceBase*>(&other))
	{
		const CReferenceBase& RefBase = *RefBasePtr;
		if (RefBase.Inner.GetTypeConst() == *this) return true;
	}
	return false;
}