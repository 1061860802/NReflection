#pragma once
#include "Void.h"
#include "BaseNumeric.h"
#include "Reference.h"
#include "Variable_traits.h"

struct CAny_
{
	CAny_(const TVariable_Type_Container<CType>& IN_Type) :Type_(IN_Type) {};
	virtual ~CAny_() {};
	const TVariable_Type_Container<CType>& GetType() const
	{
		return Type_;
	}
	virtual void* GetValue() const = 0;
	virtual CAny_* CopySelf() const = 0;
	virtual struct CAnyRef_* GetRef() = 0;
	virtual struct CAnyRef_* GetRef() const = 0;
	const TVariable_Type_Container<CType> Type_;
};

struct CAnyRef_ : public CAny_
{
	CAnyRef_(const TVariable_Type_Container<CType>& RefType,const TVariable_Type_Container<CType>& CType) :RefType_(RefType) , CAny_(CType){};
	virtual ~CAnyRef_() {};
	const TVariable_Type_Container<CType>& GetRefType() const
	{
		return RefType_;
	}
	virtual CAnyRef_* CopySelfRef() const = 0;
	virtual CAny_* CopySelf() const override
	{
		return CopySelfRef();
	}
	virtual CAnyRef_* GetRef() override
	{
		return CopySelfRef();
	}
	virtual CAnyRef_* GetRef() const override
	{
		return CopySelfRef();
	}
	const TVariable_Type_Container<CType> RefType_;
};

template<typename T>
struct TAnyRef_ : public CAnyRef_
{
	using Type_T = typename T;
	TAnyRef_(Type_T&& Val, const TVariable_Type_Container<CType>& RefType, const TVariable_Type_Container<CType>& CType) :CAnyRef_(RefType,CType), Value(std::forward<Type_T>(Val)) {};
	virtual ~TAnyRef_()
	{
		CAnyRef_::~CAnyRef_();
	};
	virtual void* GetValue() const override
	{
		return (void*)&Value;
	}
	virtual CAnyRef_* CopySelfRef() const override
	{
		TAnyRef_<Type_T>* NewAny = new TAnyRef_<Type_T>(std::forward<Type_T>(Value), RefType_, Type_);
		return NewAny;
	}
	Type_T&& Value;
};

//-----------------------------------------------------------

template<typename T>
struct TAny_ : public CAny_
{
	using Type_T = typename T;
	TAny_(const T& Val, const TVariable_Type_Container<CType>& CType) :CAny_(CType), Value(Val){};
	virtual ~TAny_() 
	{
		CAny_::~CAny_();
	};
	virtual void* GetValue() const override
	{
		return (void*) & Value;
	}
	virtual CAny_* CopySelf() const override
	{
		TAny_<Type_T>* NewAny = new TAny_<Type_T>(Value,Type_);
		return NewAny;
	}
	virtual CAnyRef_* GetRef() override
	{
		return new TAnyRef_<Type_T&>(Value,GetType_container<Type_T&>(),Type_);
	}
	virtual CAnyRef_* GetRef() const override
	{
		return new TAnyRef_<const Type_T&>(Value, GetType_container<const Type_T&>(),GetType_container<const Type_T>());
	}
	T Value;
};

template<>
struct TAny_<void> : public CAny_
{
	using Type_T = void;
	TAny_(const TVariable_Type_Container<CType>& CType) :CAny_(CType){};
	virtual ~TAny_()
	{
		CAny_::~CAny_();
	};
	virtual void* GetValue() const override
	{
		return nullptr;
	}
	virtual CAny_* CopySelf() const override
	{
		TAny_<Type_T>* NewAny = new TAny_<void>(Type_);
		return NewAny;
	}
	virtual CAnyRef_* GetRef() override
	{
		return nullptr;
	}
	virtual CAnyRef_* GetRef() const override
	{
		return nullptr;
	}
};


struct CAny
{
	template<typename T>
	CAny(T&& Target)
	{
		if constexpr (VReflectionTraits::remove_reference<T>::is_reference)
		{

		}
		else
		{
			if (Val_)
			{
				delete Val_;
				Val_ = nullptr;
			}
			const auto CType = GetType_container<T>();
			Val_ = new TAny_<T>(Target,CType);
		}
	}
	CAny()
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		const auto CType = GetType_container<void>();
		Val_ = new TAny_<void>(CType);
	}
	CAny(CAny&& Other) noexcept
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		if (Other.Val_)
		{
			Val_ = Other.Val_->CopySelf();
		}
	}
	CAny(const CAny& Other)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		if (Other.Val_)
		{
			Val_ = Other.Val_->CopySelf();
		}
	}
	~CAny()
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
	}
	void* GetValue() const
	{
		if (Val_)
		{
			return Val_->GetValue();
		}
		return nullptr;
	}
	CAnyRef_* GetRef_() const
	{
		if (Val_)
		{
			return Val_->GetRef();
		}
		return nullptr;
	}
	const TVariable_Type_Container<CType> GetValType() const
	{
		if (Val_)
		{
			return Val_->GetType();
		}
		return GetType_container<void>();
	}
	template<typename IN_T>
	static CAny GetRefAny(IN_T&& Target)
	{
		CAny Result;
		delete Result.Val_;
		const auto CType = GetType_container<decltype(Target)>();
		using RefType_T = typename VReflectionTraits::remove_reference<IN_T>::type;
		const auto RefType = GetType_container<RefType_T>();
		Result.Val_ = new TAnyRef_<IN_T&&>(std::forward<IN_T>(Target), RefType, CType);
		return Result;
	}
	void operator=(const CAny& Other)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		if (Other.Val_)
		{
			Val_ = Other.Val_->CopySelf();
		}
	}
private:
	CAny_* Val_ = nullptr;
};

struct AnyRef
{
	template<typename T>
	AnyRef(T&& Target)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		const auto CType = GetType_container<decltype(Target)>();
		using RefType_T = typename VReflectionTraits::remove_reference<T>::type;
		const auto RefType = GetType_container<RefType_T>();
		Val_ = new TAnyRef_<T&&>(std::forward<T>(Target),RefType,CType);
	}
	AnyRef(const CAny& Target)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		const auto CType = Target.GetValType();
		Val_ = Target.GetRef_();
	}
	AnyRef(const AnyRef& Other)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		if (Other.Val_)
		{
			Val_ = Other.Val_->CopySelfRef();
		}
	}
	AnyRef(AnyRef& Other)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		if (Other.Val_)
		{
			Val_ = Other.Val_->CopySelfRef();
		}
	}
	~AnyRef()
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
	}
	void* GetValue() const
	{
		if (Val_)
		{
			return Val_->GetValue();
		}
		return nullptr;
	}
	const TVariable_Type_Container<CType> GetValType() const
	{
		if (Val_)
		{
			return Val_->GetRefType();
		}
		return GetType_container<void>();
	}
	void operator=(const AnyRef& Other)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		if (Other.Val_)
		{
			Val_ = Other.Val_->CopySelfRef();
		}
	}
private:
	CAnyRef_* Val_ = nullptr;
};