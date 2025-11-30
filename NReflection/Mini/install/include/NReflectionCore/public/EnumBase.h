#pragma once

#include "Type.h"
#include "Void.h"
#include <vector>
#include "NReflectionCoreAPI.h"

namespace NLEngine
{
	class CEnumeratorBase;

	class NREFLECTIONCORE_API CEnumBase : public CType
	{
	public:
		CEnumBase(CType* underlyingType, const std::string& Name, const std::string& realname, const type_info& type_info_, size_t size);
	protected:
		CType* UnderlyingType;
	};
}