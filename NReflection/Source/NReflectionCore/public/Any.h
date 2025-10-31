#pragma once
#include "Void.h"
#include "BaseNumeric.h"
#include "Reference.h"
#include "Variable_traits.h"

struct Any_
{
	Any_(const Variable_Type_Container<Type>& Type) :Type_(Type) {};
	virtual ~Any_() {};
	const Variable_Type_Container<Type>& GetType() const
	{
		return Type_;
	}
	virtual void* GetValue() const = 0;
	virtual Any_* CopySelf() const = 0;
	virtual struct AnyRef_* GetRef() = 0;
	virtual struct AnyRef_* GetRef() const = 0;
	const Variable_Type_Container<Type> Type_;
};

struct AnyRef_ : public Any_
{
	AnyRef_(const Variable_Type_Container<Type>& RefType,const Variable_Type_Container<Type>& Type) :RefType_(RefType) , Any_(Type){};
	virtual ~AnyRef_() {};
	const Variable_Type_Container<Type>& GetRefType() const
	{
		return RefType_;
	}
	virtual AnyRef_* CopySelfRef() const = 0;
	virtual Any_* CopySelf() const override
	{
		return CopySelfRef();
	}
	virtual AnyRef_* GetRef() override
	{
		return CopySelfRef();
	}
	virtual AnyRef_* GetRef() const override
	{
		return CopySelfRef();
	}
	const Variable_Type_Container<Type> RefType_;
};

template<typename T>
struct AnyRef__ : public AnyRef_
{
	using Type_T = typename T;
	AnyRef__(Type_T&& Val, const Variable_Type_Container<Type>& RefType, const Variable_Type_Container<Type>& Type) :AnyRef_(RefType,Type), Value(std::forward<Type_T>(Val)) {};
	virtual ~AnyRef__()
	{
		AnyRef_::~AnyRef_();
	};
	virtual void* GetValue() const override
	{
		return (void*)&Value;
	}
	virtual AnyRef_* CopySelfRef() const override
	{
		AnyRef__<Type_T>* NewAny = new AnyRef__<Type_T>(std::forward<Type_T>(Value), RefType_, Type_);
		return NewAny;
	}
	Type_T&& Value;
};

//-----------------------------------------------------------

template<typename T>
struct Any__ : public Any_
{
	using Type_T = typename T;
	Any__(const T& Val, const Variable_Type_Container<Type>& Type) :Any_(Type), Value(Val){};
	virtual ~Any__() 
	{
		Any_::~Any_();
	};
	virtual void* GetValue() const override
	{
		return (void*) & Value;
	}
	virtual Any_* CopySelf() const override
	{
		Any__<Type_T>* NewAny = new Any__<Type_T>(Value,Type_);
		return NewAny;
	}
	virtual AnyRef_* GetRef() override
	{
		return new AnyRef__<Type_T&>(Value,GetType_container<Type_T&>(),Type_);
	}
	virtual AnyRef_* GetRef() const override
	{
		return new AnyRef__<const Type_T&>(Value, GetType_container<const Type_T&>(),GetType_container<const Type_T>());
	}
	T Value;
};

template<>
struct Any__<void> : public Any_
{
	using Type_T = void;
	Any__(const Variable_Type_Container<Type>& Type) :Any_(Type){};
	virtual ~Any__()
	{
		Any_::~Any_();
	};
	virtual void* GetValue() const override
	{
		return nullptr;
	}
	virtual Any_* CopySelf() const override
	{
		Any__<Type_T>* NewAny = new Any__<void>(Type_);
		return NewAny;
	}
	virtual AnyRef_* GetRef() override
	{
		return nullptr;
	}
	virtual AnyRef_* GetRef() const override
	{
		return nullptr;
	}
};


struct Any
{
	template<typename T>
	Any(T&& Target)
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
			const auto Type = GetType_container<T>();
			Val_ = new Any__<T>(Target,Type);
		}
	}
	Any()
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		const auto Type = GetType_container<void>();
		Val_ = new Any__<void>(Type);
	}
	Any(Any&& Other) noexcept
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
	Any(const Any& Other)
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
	~Any()
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
	AnyRef_* GetRef_() const
	{
		if (Val_)
		{
			return Val_->GetRef();
		}
		return nullptr;
	}
	const Variable_Type_Container<Type> GetValType() const
	{
		if (Val_)
		{
			return Val_->GetType();
		}
		return GetType_container<void>();
	}
	template<typename IN_T>
	static Any GetRefAny(IN_T&& Target)
	{
		Any Result;
		delete Result.Val_;
		const auto Type = GetType_container<decltype(Target)>();
		using RefType_T = typename VReflectionTraits::remove_reference<IN_T>::type;
		const auto RefType = GetType_container<RefType_T>();
		Result.Val_ = new AnyRef__<IN_T&&>(std::forward<IN_T>(Target), RefType, Type);
		return Result;
	}
	void operator=(const Any& Other)
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
	Any_* Val_ = nullptr;
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
		const auto Type = GetType_container<decltype(Target)>();
		using RefType_T = typename VReflectionTraits::remove_reference<T>::type;
		const auto RefType = GetType_container<RefType_T>();
		Val_ = new AnyRef__<T&&>(std::forward<T>(Target),RefType,Type);
	}
	AnyRef(const Any& Target)
	{
		if (Val_)
		{
			delete Val_;
			Val_ = nullptr;
		}
		const auto Type = Target.GetValType();
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
	const Variable_Type_Container<Type> GetValType() const
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
	AnyRef_* Val_ = nullptr;
};