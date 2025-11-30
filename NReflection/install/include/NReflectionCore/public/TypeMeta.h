#pragma once

#include "NReflectionCoreAPI.h"
#include <type_traits>

namespace NLEngine
{

	class CType;

	class NREFLECTIONCORE_API CTypeMetaBase
	{
	public:
		CTypeMetaBase() {};
		virtual ~CTypeMetaBase() = default;
		friend class CType;
	protected:
		virtual void AttachedtoType(CType* OP_Type)
		{
			return;
		}
		virtual void DetachFromType(CType* OP_Type)
		{
			return;
		}
	};

}