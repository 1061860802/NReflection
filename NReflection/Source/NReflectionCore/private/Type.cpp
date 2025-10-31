#include "Type.h"
#include "Reference.h"
#include "Any.h"

bool Type::InputRequire(bool SelfConst, const Type& other, bool OtherConst) const
{
	if (!other.bUseful()) return false;
	if (this == &other) return true;
	if (const ReferenceBase* RefBasePtr = dynamic_cast<const ReferenceBase*>(&other))
	{
		const ReferenceBase& RefBase = *RefBasePtr;
		if (RefBase.Inner.GetTypeConst() == *this) return true;
	}
	return false;
}