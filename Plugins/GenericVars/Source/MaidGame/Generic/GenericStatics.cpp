// Copyright Liquid Fish. All Rights Reserved.

#include "Generic/GenericStatics.h"

#pragma push_macro("GENERIC_DEF_HELPER")
#define GENERIC_DEF_HELPER(ValueType, PropertyType)						\
DEFINE_FUNCTION(UGenericStatics::execSetGeneric##ValueType)				\
{																		\
	P_GET_STRUCT_REF(FGeneric, Variable);								\
	Stack.StepCompiledIn<PropertyType>(nullptr);						\
	void* SrcPropertyAddress = Stack.MostRecentPropertyAddress;			\
	FProperty* SrcProperty = Stack.MostRecentProperty;					\
	P_FINISH;															\
	P_NATIVE_BEGIN;														\
	Variable.Set(SrcPropertyAddress, SrcProperty);						\
	P_NATIVE_END;														\
}																		\
DEFINE_FUNCTION(UGenericStatics::execGetGeneric##ValueType)				\
{																		\
	P_GET_STRUCT_REF(FGeneric, Variable);								\
	Stack.StepCompiledIn<PropertyType>(nullptr);						\
	void* DestPropertyAddress = Stack.MostRecentPropertyAddress;		\
	FProperty* DestProperty = Stack.MostRecentProperty;					\
	P_FINISH;															\
	P_NATIVE_BEGIN;														\
	Variable.Get(DestPropertyAddress, DestProperty);					\
	P_NATIVE_END;														\
}

GENERIC_DEF_HELPER(Value, FStructProperty)
GENERIC_DEF_HELPER(Array, FArrayProperty)
GENERIC_DEF_HELPER(Set, FSetProperty)
GENERIC_DEF_HELPER(Map, FMapProperty)
#pragma pop_macro("GENERIC_DEF_HELPER")
#pragma push_macro("GENERIC_DEF_HELPER")
#define GENERIC_DEF_HELPER(ValueType, PropertyType)						\
DEFINE_FUNCTION(UGenericStatics::exec##ValueType##ToGeneric)			\
{																		\
	Stack.StepCompiledIn<PropertyType>(nullptr);						\
	void* SrcPropertyAddress = Stack.MostRecentPropertyAddress;			\
	FProperty* SrcProperty = Stack.MostRecentProperty;					\
	P_GET_STRUCT_REF(FGeneric, Variable);								\
	P_FINISH;															\
	P_NATIVE_BEGIN;														\
	Variable.Set(SrcPropertyAddress, SrcProperty);						\
	P_NATIVE_END;														\
}																		\
DEFINE_FUNCTION(UGenericStatics::execGenericTo##ValueType)				\
{																		\
	P_GET_STRUCT_REF(FGeneric, Variable);								\
	Stack.StepCompiledIn<PropertyType>(nullptr);						\
	void* DestPropertyAddress = Stack.MostRecentPropertyAddress;		\
	FProperty* DestProperty = Stack.MostRecentProperty;					\
	P_FINISH;															\
	P_NATIVE_BEGIN;														\
	Variable.Get(DestPropertyAddress, DestProperty);					\
	P_NATIVE_END;														\
}

GENERIC_DEF_HELPER(Value, FStructProperty)
GENERIC_DEF_HELPER(Array, FArrayProperty)
GENERIC_DEF_HELPER(Set, FSetProperty)
GENERIC_DEF_HELPER(Map, FMapProperty)
#pragma pop_macro("GENERIC_DEF_HELPER")
#pragma push_macro("GENERIC_DEF_HELPER")
#define GENERIC_DEF_HELPER(CppType, TypeName, ...)						\
FGeneric UGenericStatics::TypeName##ToGeneric(CppType Value)			\
{																		\
	return (FGeneric)Value;												\
}																		\
CppType UGenericStatics::GenericTo##TypeName(FGeneric Variable)			\
{																		\
	return (CppType)Variable;											\
}
GENERIC_DEF_HELPER(float, Float, FFloatProperty)
GENERIC_DEF_HELPER(int32, Int, FIntProperty)
GENERIC_DEF_HELPER(int64, Int64, FInt64Property)
GENERIC_DEF_HELPER(bool, Bool, FBoolProperty)
GENERIC_DEF_HELPER(uint8, Byte, FByteProperty)
GENERIC_DEF_HELPER(FString, String, FStrProperty)
GENERIC_DEF_HELPER(FName, Name, FNameProperty)
GENERIC_DEF_HELPER(UObject*, ObjectRef, FObjectProperty)
GENERIC_DEF_HELPER(TSubclassOf<UObject>, ClassRef, FObjectProperty)
#pragma pop_macro("GENERIC_DEF_HELPER")

bool UGenericStatics::Equal(const FGeneric& A, const FGeneric& B)
{
	return A == B;
}

bool UGenericStatics::NotEqual(const FGeneric& A, const FGeneric& B)
{
	return A != B;
}

bool UGenericStatics::IsEmpty(const FGeneric& Variable)
{
	return Variable.IsEmpty();
}

void UGenericStatics::Clear(FGeneric& Variable)
{
	Variable.Clear();
}
