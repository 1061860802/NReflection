#pragma once
#include "NReflectionCoreAPI.h"
#include "EnumeratorMeta.h"
#include "EnumBase.h"
#include <vector>
#include <string>
#include <type_traits>

namespace NLEngine
{
	class CEnumeratorBase
	{
	public:
		virtual ~CEnumeratorBase() {};
		void ApplyMeta(CEnumeratorMetaBase* IN_Meta)
		{
			Metas.push_back(IN_Meta);
			IN_Meta->AttachedtoType(this);
		}
		void ApplyMetas(const std::vector<CEnumeratorMetaBase*>& IN_Metas)
		{
			for (auto i : IN_Metas)
			{
				Metas.push_back(i);
				i->AttachedtoType(this);
			}
		}
		const std::string& GetName() const { return name_; };
		const CEnumBase& EnumClass() const { return EnumClass_; };
		virtual const void* GetEnumeratorPtr() const = 0;
	protected:
		const std::string name_;
		const CEnumBase& EnumClass_;
		std::vector<CEnumeratorMetaBase*> Metas;
		CEnumeratorBase(const std::string& name,const CEnumBase& EnumClass, const std::vector<CEnumeratorMetaBase*>& IN_Metas) :
			name_(name), EnumClass_(EnumClass), Metas(IN_Metas)
		{
			for (auto i : Metas)
			{
				i->AttachedtoType(this);
			}
		};
	};

	template<typename T>
	class TEnumerator : public CEnumeratorBase
	{
	public:
		TEnumerator(const T& EnumeratorValue,const std::string& name, const CEnumBase& EnumClass, const std::vector<CEnumeratorMetaBase*>& IN_Metas):
			EnumeratorValue_(EnumeratorValue), CEnumeratorBase(name, EnumClass, IN_Metas){ }
		virtual const void* GetEnumeratorPtr() const override
		{
			return &EnumeratorValue_;
		}
	private:
		T EnumeratorValue_;
	};
}