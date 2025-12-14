// Copyright Liquid Fish. All Rights Reserved.

#pragma once

#include "Core/MaidCoreFwd.h"
#include "Generic/Generic.h"

#include "GenericStatics.generated.h"

/**
 * Blueprint Function Library for FGeneric type support in Blueprints
 * Provides conversion utilities and accessors for FGeneric data container
 */
UCLASS()
class MAIDGAME_API UGenericStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ========================
    // Value Accessors & Mutators
    // ========================

    /** Set generic value from integer source */
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (CustomStructureParam = "NewValue", KeyWords = "Assign,SetValue", CompactNodeTitle = "SET"), Category = "Generic")
    static void SetGenericValue(UPARAM(ref) FGeneric& Variable, const int32& NewValue);

    /** Retrieve generic value as integer destination */
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (CustomStructureParam = "ReturnValue", KeyWords = "Obtain,To", CompactNodeTitle = "GET"), Category = "Generic")
    static void GetGenericValue(const FGeneric& Variable, int32& ReturnValue);

    // Array operations
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (ArrayParm = "NewValue", KeyWords = "Assign,SetArray", CompactNodeTitle = "SET"), Category = "Generic")
    static void SetGenericArray(UPARAM(ref) FGeneric& Variable, const TArray<int32>& NewValue);

    UFUNCTION(BlueprintCallable, CustomThunk, meta = (ArrayParm = "ReturnValue", KeyWords = "Obtain,To", CompactNodeTitle = "GET"), Category = "Generic")
    static void GetGenericArray(const FGeneric& Variable, TArray<int32>& ReturnValue);

    // Set operations
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (SetParam = "NewValue", KeyWords = "Assign", CompactNodeTitle = "SET"), Category = "Generic")
    static void SetGenericSet(UPARAM(ref) FGeneric& Variable, const TSet<int32>& NewValue);

    UFUNCTION(BlueprintCallable, CustomThunk, meta = (SetParam = "ReturnValue", KeyWords = "Obtain,To", CompactNodeTitle = "GET"), Category = "Generic")
    static void GetGenericSet(const FGeneric& Variable, TSet<int32>& ReturnValue);

    // Map operations
    UFUNCTION(BlueprintCallable, CustomThunk, meta = (MapParam = "NewValue", KeyWords = "Assign,SetMap", CompactNodeTitle = "SET"), Category = "Generic")
    static void SetGenericMap(UPARAM(ref) FGeneric& Variable, const TMap<int32, int32>& NewValue);

    UFUNCTION(BlueprintCallable, CustomThunk, meta = (MapParam = "ReturnValue", KeyWords = "Obtain,To", CompactNodeTitle = "GET"), Category = "Generic")
    static void GetGenericMap(const FGeneric& Variable, TMap<int32, int32>& ReturnValue);

private:
    // Execution handlers for custom thunk functions
    DECLARE_FUNCTION(execSetGenericValue);
    DECLARE_FUNCTION(execGetGenericValue);
    DECLARE_FUNCTION(execSetGenericArray);
    DECLARE_FUNCTION(execGetGenericArray);
    DECLARE_FUNCTION(execSetGenericSet);
    DECLARE_FUNCTION(execGetGenericSet);
    DECLARE_FUNCTION(execSetGenericMap);
    DECLARE_FUNCTION(execGetGenericMap);

public:
    // ========================
    // Type Conversion Utilities
    // ========================

    /** Bidirectional conversion between primitive types and FGeneric */
    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void ValueToGeneric(const int32& Value, FGeneric& Variable);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void GenericToValue(const FGeneric& Variable, int32& Value);

    // Array conversions
    UFUNCTION(BlueprintPure, CustomThunk, meta = (ArrayParm = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void ArrayToGeneric(const TArray<int32>& Value, FGeneric& Variable);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (ArrayParm = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void GenericToArray(const FGeneric& Variable, TArray<int32>& Value);

    // Set conversions
    UFUNCTION(BlueprintPure, CustomThunk, meta = (SetParam = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void SetToGeneric(const TSet<int32>& Value, FGeneric& Variable);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (SetParam = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void GenericToSet(const FGeneric& Variable, TSet<int32>& Value);

    // Map conversions
    UFUNCTION(BlueprintPure, CustomThunk, meta = (MapParam = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void MapToGeneric(const TMap<int32, int32>& Value, FGeneric& Variable);

    UFUNCTION(BlueprintPure, CustomThunk, meta = (MapParam = "Value", KeyWords = "cast convert", CompactNodeTitle = "->"), Category = "Generic")
    static void GenericToMap(const FGeneric& Variable, TMap<int32, int32>& Value);

private:
    // Execution handlers for conversion functions
    DECLARE_FUNCTION(execValueToGeneric);
    DECLARE_FUNCTION(execGenericToValue);
    DECLARE_FUNCTION(execArrayToGeneric);
    DECLARE_FUNCTION(execGenericToArray);
    DECLARE_FUNCTION(execSetToGeneric);
    DECLARE_FUNCTION(execGenericToSet);
    DECLARE_FUNCTION(execMapToGeneric);
    DECLARE_FUNCTION(execGenericToMap);

public:
    // ========================
    // Auto-cast Conversions
    // ========================

    /** Float type conversions with Blueprint auto-cast support */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (Float)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric FloatToGeneric(float Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToFloat (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static float GenericToFloat(FGeneric Variable);

    // Integer type conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (Int)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric IntToGeneric(int32 Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToInt (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static int32 GenericToInt(FGeneric Variable);

    // 64-bit integer conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (Int64)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric Int64ToGeneric(int64 Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToInt64 (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static int64 GenericToInt64(FGeneric Variable);

    // Boolean conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (Bool)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric BoolToGeneric(bool Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToBool (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static bool GenericToBool(FGeneric Variable);

    // Byte conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (Byte)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric ByteToGeneric(uint8 Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToByte (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static uint8 GenericToByte(FGeneric Variable);

    // String conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (String)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric StringToGeneric(FString Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToString (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FString GenericToString(FGeneric Variable);

    // Name conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (Name)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric NameToGeneric(FName Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToName (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FName GenericToName(FGeneric Variable);

    // Object reference conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (ObjectReference)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric ObjectRefToGeneric(UObject* Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToObjectReference (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static UObject* GenericToObjectRef(FGeneric Variable);

    // Class reference conversions
    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToGeneric (ClassReference)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static FGeneric ClassRefToGeneric(TSubclassOf<UObject> Value);

    UFUNCTION(BlueprintPure, meta = (DisplayName = "ToClassReference (Generic)", KeyWords = "cast convert", CompactNodeTitle = "->", BlueprintAutocast), Category = "Generic")
    static TSubclassOf<UObject> GenericToClassRef(FGeneric Variable);

public:
    // ========================
    // Utility Functions
    // ========================

    /** Equality comparison between two FGeneric instances */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (Generic)", CompactNodeTitle = "=="), Category = "Generic")
    static bool Equal(const FGeneric& A, const FGeneric& B);

    /** Inequality comparison between two FGeneric instances */
    UFUNCTION(BlueprintPure, meta = (DisplayName = "Not Equal (Generic)", CompactNodeTitle = "!="), Category = "Generic")
    static bool NotEqual(const FGeneric& A, const FGeneric& B);

    /** Clear contents of FGeneric variable */
    UFUNCTION(BlueprintCallable, Category = "Generic", meta = (CompactNodeTitle = "CLEAR"))
    static void Clear(UPARAM(ref) FGeneric& Variable);

    /** Check if FGeneric instance contains no data */
    UFUNCTION(BlueprintPure, Category = "Generic")
    static bool IsEmpty(const FGeneric& Variable);
};