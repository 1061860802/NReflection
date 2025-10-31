#include "TypeManager.h"
#include "Class.h"
#include "ClassFactory.h"

template<>
struct TypeName<TypeManager>
{
	inline static const auto Name = "TypeManager";
};

TypeManager::TypeManager()
{

}

TypeManager::~TypeManager()
{
}


Type* TypeManager::FromNameFindTypeImp(const std::string& Name) const
{
	for (auto i = TypesMap.begin(); i != TypesMap.end(); ++i)
	{
		if (i->second->GetTypeName() == Name)
		{
			return i->second;
		}
	}
	return nullptr;
}

Type* TypeManager::FromRealNameFindTypeImp(const std::string& Name) const
{
	auto i = TypesMap.find(Name);
	if (i != TypesMap.end())
	{
		return i->second;
	}
	return nullptr;
}

size_t TypeManager::TypesNum() const
{
	return TypesMap.size();
}

void TypeManager::RegisterTypeManager()
{
	//auto& TypeTarget = Register<TypeManager>();
	//TypeTarget.Info();
	// .ApplyFunction(&TryApplyType, "TryApplyType");
}

bool TypeManager::TryApplyType(Type* NewType)
{
	Lock.lock();
	if (FromRealNameFindTypeImp(NewType->GetRealName()))
	{
		Lock.unlock();
		return false;
	}
	TypesMap.insert(std::pair(NewType->GetRealName(),NewType));
	Lock.unlock();
	return true;
}

static TypeManager StaticTypeManager;

TypeManager& GetTypeManager()
{
	return StaticTypeManager;
}
