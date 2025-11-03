#include "TypeManager.h"
#include "Class.h"
#include "ClassFactory.h"

template<>
struct TTypeName<CTypeManager>
{
	inline static const auto Name = "CTypeManager";
};

CTypeManager::CTypeManager()
{

}

CTypeManager::~CTypeManager()
{
	for (auto i = TypesMap.begin(); i != TypesMap.end(); ++i)
	{
		delete i->second;
	}
}


CType* CTypeManager::FromNameFindTypeImp(const std::string& Name) const
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

CType* CTypeManager::FromRealNameFindTypeImp(const std::string& Name) const
{
	auto i = TypesMap.find(Name);
	if (i != TypesMap.end())
	{
		return i->second;
	}
	return nullptr;
}

size_t CTypeManager::TypesNum() const
{
	return TypesMap.size();
}

void CTypeManager::RegisterTypeManager()
{
	//auto& TypeTarget = Register<CTypeManager>();
	//TypeTarget.Info();
	// .ApplyFunction(&TryApplyType, "TryApplyType");
}

bool CTypeManager::TryApplyType(CType* NewType)
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

static CTypeManager StaticTypeManager;

CTypeManager& GetTypeManager()
{
	return StaticTypeManager;
}
