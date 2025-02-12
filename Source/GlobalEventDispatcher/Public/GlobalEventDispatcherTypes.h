// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "Templates/Casts.h"
#include "Templates/TypeHash.h"
#include <iostream>


// The base class for storing data.
class FBaseVariant
{
public:
    virtual ~FBaseVariant() = default;
};

// A template class for storing data of a specific type.
template<typename T>
class FVariantImpl : public FBaseVariant
{
public:
    FVariantImpl() {}
    FVariantImpl(T& InValue) : Value(InValue) {}

    T& GetValue() const
    {
        return Value;
    }

private:
    T Value;
};

// A universal container for various types of data.
class FUniversalVariant
{
public:
    template<typename T>
    FUniversalVariant(T& InValue)
    {
        Data = MakeShareable(new FVariantImpl<T&>(InValue));
    }

    template<typename T>
    T Get() const
    {
        const FVariantImpl<T&>* Variant = static_cast<const FVariantImpl<T&>*>(Data.Get());
        check(Variant); 
        return Variant->GetValue();
    }

private:
    TSharedPtr<FBaseVariant> Data;
};

// The key for the delegate card.
struct FDelegateKey
{
    FName Namespace;
    FName EventName;

    bool operator==(const FDelegateKey& Other) const
    {
        return Namespace == Other.Namespace && EventName == Other.EventName;
    }

    friend uint32 GetTypeHash(const FDelegateKey& Key)
    {
        return HashCombine(GetTypeHash(Key.Namespace), GetTypeHash(Key.EventName));
    }
};

// The interface for delegates.
class IGlobalDelegate
{
public:
    virtual ~IGlobalDelegate() = default;
    virtual void* GetOwner() const = 0;

    virtual void Execute(const TArray<FUniversalVariant>& Params) = 0;
};

// A delegate with a specific type.
template<typename TOwner, typename... ParamTypes>
class TGlobalDelegate : public IGlobalDelegate
{
public:
    TGlobalDelegate(TOwner* Object, void(TOwner::* InFunction)(ParamTypes...))
        : OwnerRef(Object), Function(InFunction) {}

    virtual void* GetOwner() const override
    {
        return OwnerRef;
    }

    virtual void Execute(const TArray<FUniversalVariant>& Params) override
    {
        if (OwnerRef && Function)
            CallFunction(Params, TMakeIntegerSequence<int, sizeof...(ParamTypes)>());
    }

private:
    TOwner* OwnerRef;
    void (TOwner::* Function)(ParamTypes...);

    template<int... Indices>
    void CallFunction(const TArray<FUniversalVariant>& Params, TIntegerSequence<int, Indices...>)
    {
            (OwnerRef->*Function)(GetParam<ParamTypes&>(Params[Indices])...);
    }

    template<typename T>
    T GetParam(const FUniversalVariant& Variant) const
    {
        return Variant.Get<T>();
    }
};