#include "Type.h"
#include "Reference.h"
#include "Any.h"

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