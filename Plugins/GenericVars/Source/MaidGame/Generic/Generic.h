// Copyright Liquid Fish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Traits/MaidCoreTraits.h"
#include "Misc/EngineVersionComparison.h"

#if UE_VERSION_NEWER_THAN(5, 5, 0)
#include "StructUtils/UserDefinedStruct.h"
#else
#include "Engine/UserDefinedStruct.h"
#endif

#include "Generic.generated.h"

#pragma warning(disable: 4499)

#ifndef GENERIC_USING_CACHE
#define GENERIC_USING_CACHE 1
#endif

/**
 * Reflection Metadata Host for FGeneric
 *
 * This struct serves as a non-functional container whose sole purpose is to host UPROPERTY()
 * declarations for various engine types. It is never instantiated and holds no actual data.
 *
 * DESIGN PATTERN: Impostor / Reflection Host
 * - Each member variable's UPROPERTY() declaration causes the Unreal Header Tool (UHT) to
 *   generate corresponding FProperty metadata for that type.
 * - The static `Get<Type>()` template methods then retrieve these pre-generated FProperty*
 *   pointers through reflection, which FGeneric uses for serialization and deserialization.
 *
 * This approach avoids the performance cost and complexity of dynamically creating FProperty
 * objects at runtime while providing full type safety and editor integration.
 */
USTRUCT()
struct FGenericPropJunkPrivate
{
	GENERATED_BODY()

	FGenericPropJunkPrivate() { FMemory::Memzero(*this); }

	// Primary data types
	UPROPERTY() bool Bool;
	UPROPERTY() float Float;
	UPROPERTY() double Double;
	UPROPERTY() int8 Int8;
	UPROPERTY() int16 Int16;
	UPROPERTY() int32 Int32;
	UPROPERTY() int64 Int64;
	UPROPERTY() uint8 UInt8;
	UPROPERTY() uint16 UInt16;
	UPROPERTY() uint32 UInt32;
	UPROPERTY() uint64 UInt64;
	UPROPERTY() FName Name;
	UPROPERTY() FString String;
	UPROPERTY() UObject* Object;
	UPROPERTY() TSubclassOf<UObject> Class;
	UPROPERTY() TSoftObjectPtr<UObject> SoftObject;
	UPROPERTY() TSoftClassPtr<UObject> SoftClass;

	// Common struct types
	UPROPERTY() FVector2D Vector2D;
	UPROPERTY() FVector Vector;
	UPROPERTY() FVector4 Vector4;
	UPROPERTY() FIntVector IntVector;
	UPROPERTY() FIntPoint IntPoint;
	UPROPERTY() FRotator Rotator;
	UPROPERTY() FQuat Quat;
	UPROPERTY() FTransform Transform;
	UPROPERTY() FMatrix Matrix;
	UPROPERTY() FLinearColor LinearColor;
	UPROPERTY() FColor Color;
	UPROPERTY() FPlane Plane;
	UPROPERTY() FGuid Guid;
	UPROPERTY() FBox Box;
	UPROPERTY() FBox2D Box2D;
	UPROPERTY() FDateTime DateTime;
	UPROPERTY() FTimespan Timespan;

	template<typename T> struct FUncommentCheck { static uint8 Func(); };
	/**
	 * Uncomment the following code if UE_VERSION_NEWER_THAN(5, 0, 0)
	 */
	//UPROPERTY() FVector2f Vector2f;
	//UPROPERTY() FVector3f Vector3f;
	//UPROPERTY() FVector4f Vector4f;
	//UPROPERTY() FMatrix44f Matrix44f;
	//UPROPERTY() FBox3f Box3f;
	//UPROPERTY() FBox2f Box2f;
	//template<> struct FUncommentCheck<FGenericPropJunkPrivate> { static uint16 Func(); };
#if UE_VERSION_NEWER_THAN(5, 0, 0)
	static_assert(sizeof(FUncommentCheck<FGenericPropJunkPrivate>::Func()) - 1,
		"Uncomment the above lines for UE5.0+ compatibility");
#endif

	// Array types
	UPROPERTY() TArray<int32> Int32Array;
	UPROPERTY() TArray<float> FloatArray;
	UPROPERTY() TArray<FString> StringArray;
	UPROPERTY() TArray<FName> NameArray;
	UPROPERTY() TArray<UObject*> ObjectArray;
	UPROPERTY() TArray<FVector> VectorArray;

public:
#if CPP
#pragma push_macro("GENERIC_PROPERTY")
#define GENERIC_PROPERTY(CppType, Name) \
	static inline FProperty* Get(CppType&&) \
	{ \
		static FProperty* Prop = StaticStruct()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FGenericPropJunkPrivate, Name)); \
		return Prop; \
	}
	// END DEFINE GENERIC_PROPERTY
#include "GenericProperties.inl"
#pragma pop_macro("GENERIC_PROPERTY")
#endif // CPP
};

#pragma push_macro("GET_GENERIC_PROP_PRIVATE")
#define GET_GENERIC_PROP_PRIVATE(CppType) FGenericPropJunkPrivate::Get(CppType())

/**
 * Universal container type supporting both Blueprint and C++ systems
 *
 * Stores values of diverse types with full Blueprint-C++ interoperability.
 * Uses different storage strategies based on data type characteristics:
 *
 * Plain Data Types (direct memory storage):
 * - POD (Plain Old Data) structure
 * - No pointers or references
 * - No strings or dynamic allocations
 * - No virtual functions or vtables
 * - Memory alignment safe (endianness-aware)
 *
 * Complex Types (serialized text storage):
 * - All non-plain types using UE property export/import system
 *
 * Example usage:
 *   FGeneric foo = FLinearColor::White;
 *   float myFloat = foo;
 *   FGeneric foo = (UObject*)MyActor;
 *   bool result = foo; // Converts to bool based on stored value
 */
USTRUCT(BlueprintType, Category = "Generic")
struct MAIDGAME_API FGeneric
{
	GENERATED_BODY()

private:
	/** Serialized data for complex types (non-plain types) */
	UPROPERTY(VisibleAnywhere)
	FString Data;

	/** Raw memory storage for plain data types */
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> PlainData;

	/**
	 * Soft references to UObjects contained in the data for asset dependency tracking
	 * Used by editor tools to ensure referenced assets are included during packaging
	 */
	UPROPERTY(VisibleAnywhere)
	TArray<TSoftObjectPtr<UObject>> ReferencedObjects;

#if WITH_EDITORONLY_DATA
	/** Pin type information for editor visualization */
	UPROPERTY(VisibleAnywhere)
	FEdGraphPinType EditPinType;
#endif

	friend class UGenericStatics;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	UUserDefinedStruct* EditorStruct = nullptr;

	friend class FGenericStructCustomization;
#endif

#if GENERIC_USING_CACHE
private:
	/**
	 * Data cache for performance optimization
	 * Stores deserialized values to avoid repeated text parsing
	 */
	struct FDataCache
	{
		using FDataCacheStorageType = decltype(FGeneric::PlainData)::ElementType;
		FProperty* Prop = nullptr;
		TArray<FDataCacheStorageType> CacheData;

		~FDataCache() { Clear(); }
		FORCEINLINE void SetSize(int32 NewSize)
		{
			NewSize = FMath::Max((NewSize / CacheData.GetTypeSize()), 1u);
			NewSize = FMath::Max(NewSize, CacheData.Num());
			CacheData.SetNumZeroed(NewSize);
		}
		FORCEINLINE int32 GetSize() const
		{
			return CacheData.Num() * CacheData.GetTypeSize();
		}
		FORCEINLINE FDataCacheStorageType* GetData() 
		{ 
			return CacheData.GetData();
		}
		FORCEINLINE const FDataCacheStorageType* GetData() const
		{ 
			return CacheData.GetData();
		}
		FORCEINLINE void Clear()
		{
			if (Prop && Prop->GetSize() <= GetSize())
			{
				Prop->ClearValue(GetData());
			}
			CacheData.Reset();
			Prop = nullptr;
		}
		FORCEINLINE void Set(const void* SrcPropertyAddress, const FProperty* SrcProperty)
		{
			Clear();
			if (SrcProperty)
			{
				SetSize(SrcProperty->GetSize());
				if (SrcPropertyAddress)
				{
					SrcProperty->CopyCompleteValue(GetData(), SrcPropertyAddress);
				}
				else SrcProperty->InitializeValue(GetData());
			}
		}
		FORCEINLINE bool ConditionalGet(void* DestPropertyAddress, const FProperty* DestProperty) const
		{
			if (Prop && DestPropertyAddress && DestProperty && DestProperty->SameType(Prop) && GetSize() >= DestProperty->GetSize())
			{
				DestProperty->ClearValue(DestPropertyAddress);
				DestProperty->CopyCompleteValue(DestPropertyAddress, GetData());
				return true;
			}
			return false;
		}
	} DataCache;
#endif

public:
	/** Null instance representing an empty FGeneric value */
	static const FGeneric Null;

public:
#if CPP
#pragma push_macro("GENERIC_COPY_DATA")
#pragma push_macro("GENERIC_COPY_DATA_ED")
#pragma push_macro("GENERIC_COPY_DATA_CACHE")
#pragma push_macro("GENERIC_CTOR")
#define GENERIC_COPY_DATA(DECORATE) do{ Data = DECORATE(Other.Data); PlainData = DECORATE(Other.PlainData); ReferencedObjects = DECORATE(Other.ReferencedObjects); } while(0);
#if WITH_EDITORONLY_DATA
#define GENERIC_COPY_DATA_ED(DECORATE) do{ EditPinType = DECORATE(Other.EditPinType); } while(0);
#else
#define GENERIC_COPY_DATA_ED(...)
#endif
#if GENERIC_USING_CACHE && 0 // Ignore cache for faster copy
#define GENERIC_COPY_DATA_CACHE(DECORATE) do{ DataCache = DECORATE(Other.DataCache); } while(0);
#else
#define GENERIC_COPY_DATA_CACHE(...)
#endif
#define GENERIC_CTOR(DECORATE, ...) { if(this != &Other) { GENERIC_COPY_DATA(DECORATE); GENERIC_COPY_DATA_ED(DECORATE); GENERIC_COPY_DATA_CACHE(DECORATE); } __VA_ARGS__; }
	FGeneric() = default;
	FGeneric(const FGeneric& Other) GENERIC_CTOR(*&, );
	FGeneric(FGeneric&& Other) GENERIC_CTOR(MoveTempIfPossible, );
	FGeneric& operator=(const FGeneric& Other) GENERIC_CTOR(*&, return *this;);
	FGeneric& operator=(FGeneric&& Other) GENERIC_CTOR(MoveTempIfPossible, return *this;);
	FGeneric(EForceInit) {}
#pragma pop_macro("GENERIC_COPY_DATA")
#pragma pop_macro("GENERIC_COPY_DATA_ED")
#pragma pop_macro("GENERIC_COPY_DATA_CACHE")
#pragma pop_macro("GENERIC_CTOR")
#else
	FGeneric() {}
	FGeneric(const FGeneric& Other);
	FGeneric(FGeneric&& Other);
	FGeneric& operator=(const FGeneric& Other);
	FGeneric& operator=(FGeneric&& Other);
	FGeneric(EForceInit) {}
#endif
public:
	/** Get the address of PlainData for direct memory access */
	void* GetPlainData() { return PlainData.GetData(); }

	/** Get the address of PlainData for direct memory access (const version) */
	const void* GetPlainData() const { return PlainData.GetData(); }

	/** Get the serialized string data for non-plain types */
	const FString& GetStringData() const { return Data; }

	/**
	 * Set the value from a source address and property description
	 * @param SrcPropertyAddress - Address of the source data
	 * @param SrcProperty - Property describing the data type
	 */
	void Set(const void* SrcPropertyAddress, const FProperty* SrcProperty);

	/**
	* Retrieve the value to a destination address
	* @param DestPropertyAddress - Address where to store the retrieved data
	* @param DestProperty - Property describing the expected data type
	*/
	void Get(void* DestPropertyAddress, const FProperty* DestProperty) const;

	/** Clear the stored value and release resources */
	void Clear();

	/** Check if this instance contains no data */
	bool IsEmpty() const { return Data.IsEmpty() && PlainData.Num() == 0; }

	/** Equality comparison operator */
	FORCEINLINE bool operator== (const FGeneric& Other) const
	{
		return Data.Equals(Other.Data, ESearchCase::CaseSensitive) && PlainData == Other.PlainData;
	}

	/** Inequality comparison operator */
	FORCEINLINE bool operator!= (const FGeneric& Other) const { return !(*this == Other); }

	/** Compute hash value for this instance */
	friend uint32 GetTypeHash(const FGeneric& Generic) { return GetTypeHash(Generic.GetStringData()) ^ FCrc::MemCrc32(Generic.GetPlainData(), Generic.GetPlainSize()); }

#if WITH_EDITORONLY_DATA
	/** Check if the pin type is valid */
	FORCEINLINE bool IsValidPinType() const { return EditPinType != FEdGraphPinType(); };
	FORCEINLINE const FEdGraphPinType& GetEditPinType() const { return EditPinType; }
	FORCEINLINE void SetEditPinType(const FEdGraphPinType& NewType) { EditPinType = NewType; }
	FORCEINLINE void SetEditPinType(const FProperty* Property) { GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Property, EditPinType); }
#else
	template <typename... Args> FORCEINLINE void SetEditPinType(Args&&...) {}
#endif

public:
	/**
	 * Check if a property represents a plain data type (POD, no pointers, no strings, no vtables)
	 * @param Prop - The property to check
	 * @return True if the property represents a plain data type
	 */
	static const bool IsPlain(const FProperty* Prop);

	/** Get the size of the plain data in bytes */
	FORCEINLINE int32 GetPlainSize() const { return PlainData.Num() * PlainData.GetTypeSize(); }

private:
	/** Resize the plain data storage to the specified size */
	FORCEINLINE void SetPlainSize(int32 NewSize) { PlainData.SetNumZeroed(FMath::Max((NewSize / PlainData.GetTypeSize()), 1u)); }

#if WITH_EDITOR
	void CacheReferencedObjects(const FProperty* InProperty, const void* InData);
	void CacheReferencedObjects(const UScriptStruct* InProperty, const void* InData);
	void ClearReferencedObjects();
#endif

#if CPP
public:
	/** Construct from UObject pointer (converts to soft object pointer internally) */
	FGeneric(const UObject* Other)
	{
		TSoftObjectPtr<UObject> SoftPtr(const_cast<UObject*>(Other));
		Set(&SoftPtr, GET_GENERIC_PROP_PRIVATE(TSoftObjectPtr<UObject>));
	}

	/** Assign from UObject pointer (converts to soft object pointer internally) */
	FGeneric& operator=(const UObject* Other)
	{
		TSoftObjectPtr<UObject> SoftPtr(const_cast<UObject*>(Other));
		Set(&SoftPtr, GET_GENERIC_PROP_PRIVATE(TSoftObjectPtr<UObject>));
		return *this;
	}

	/** Construct from TSubclassOf (converts to soft class pointer internally) */
	template<class TClass, typename std::enable_if_t<std::is_base_of_v<TClass, TClass>>* = nullptr>
	explicit FGeneric(const TSubclassOf<TClass>& Other)
	{
		(*this) = Other;
	}

	/** Assign from TSubclassOf (converts to soft class pointer internally) */
	template<class TClass, typename std::enable_if_t<std::is_base_of_v<TClass, TClass>>* = nullptr>
	FGeneric& operator=(const TSubclassOf<TClass>& Other)
	{
		TSoftClassPtr<UObject> SoftPtr(Other);
		Set(&SoftPtr, GET_GENERIC_PROP_PRIVATE(TSoftClassPtr<UObject>));
		return *this;
	}

	/** Construct from string literal */
	explicit FGeneric(const TCHAR* Other) : FGeneric(FString(Other)) {}

	/** Assign from string literal */
	FGeneric& operator=(const TCHAR* Other)
	{
		return operator=(FString(Other));
	}

	/** Construct from any UScriptStruct-based type */
	template<class CppType, typename std::enable_if_t<TIsUStruct<CppType>>* = nullptr>
	explicit FGeneric(const CppType& Other)
	{
		(*this) = Other;
	}

	/** Assign from any UScriptStruct-based type */
	template<class CppType, typename std::enable_if_t<TIsUStruct<CppType>>* = nullptr>
	FORCEINLINE FGeneric& operator=(const CppType& Other)
	{
		Clear();
		UScriptStruct* Struct = CppType::StaticStruct();
		Struct->ExportText(Data, &Other, nullptr, nullptr, 0, nullptr);
#if WITH_EDITOR
		CacheReferencedObjects(Struct, &Other);
#endif
#if WITH_EDITORONLY_DATA
		EditPinType.PinCategory = "struct";
		EditPinType.PinSubCategoryObject = Struct;
#endif
		return *this;
	}

	/** Construct from UEnum type */
	template<class CppType, typename std::enable_if_t<TIsUEnum<CppType>>* = nullptr>
	explicit FGeneric(const CppType& Other)
	{
		(*this) = Other;
	}

	/** Assign from UEnum type */
	template<class CppType, typename std::enable_if_t<TIsUEnum<CppType>>* = nullptr>
	FORCEINLINE FGeneric& operator=(const CppType& Other)
	{
		(*this) = (__underlying_type(CppType))Other;
		return *this;
	}

	/** Construct from TEnumAsByte of UEnum type */
	template<class CppType, typename std::enable_if_t<TIsUEnum<CppType>>* = nullptr>
	explicit FGeneric(const TEnumAsByte<CppType>& Other)
	{
		(*this) = Other;
	}

	/** Assign from TEnumAsByte of UEnum type */
	template<class CppType, typename std::enable_if_t<TIsUEnum<CppType>>* = nullptr>
	FORCEINLINE FGeneric& operator=(const TEnumAsByte<CppType>& Other)
	{
		(*this) = (__underlying_type(CppType))Other;
		return *this;
	}

#pragma push_macro("GENERIC_PROPERTY")
#pragma push_macro("GENERIC_PROPERTY_OBJECT")
#pragma push_macro("GENERIC_PROPERTY_CLASS")
#define GENERIC_PROPERTY(CppType, Name) \
	FGeneric(const CppType& Other) \
	{ \
		(*this) = Other; \
	} \
	FGeneric& operator=(const CppType& Other) \
	{ \
		/* Fast path for integral types */ \
		if constexpr (TIsIntegral<CppType>::Value || TIsFloatingPoint<CppType>::Value) \
		{ \
			Clear(); \
			PlainData.SetNumUninitialized(sizeof(Other)); \
			FMemory::Memcpy(GetPlainData(), &Other, GetPlainSize()); \
			SetEditPinType(GET_GENERIC_PROP_PRIVATE(CppType)); \
		} \
		else \
		{ \
			Set(&Other, GET_GENERIC_PROP_PRIVATE(CppType)); \
		} \
		return *this; \
	}
	// END DEFINE GENERIC_PROPERTY
#define GENERIC_PROPERTY_OBJECT(CppType, Name)
#define GENERIC_PROPERTY_CLASS(CppType, Name)
#include "GenericProperties.inl"
#pragma pop_macro("GENERIC_PROPERTY")
#pragma pop_macro("GENERIC_PROPERTY_OBJECT")
#pragma pop_macro("GENERIC_PROPERTY_CLASS")

	/**
	 * Convert the stored value to the specified type
	 * @tparam CppType - Target type for conversion
	 * @return Converted value of type CppType
	 */
	template<typename CppType> const CppType As() const
	{
		using CppTypeNoCV = typename std::remove_cv<CppType>::type;
		static_assert(!std::is_same_v<CppTypeNoCV, void>, "Template parameter cannot be void.");

		if constexpr (false) {}
		else if constexpr (std::is_same_v<CppTypeNoCV, bool>)
		{
			if (GetPlainSize() == 1)
				return (bool)PlainData[0];
			else if (GetPlainSize() == 0)
				return !Data.IsEmpty();
			else {
				for (const auto Elem : PlainData)
					if (Elem) return true;
			}
			return false;
		}
		else if constexpr (TIsFloatingPoint<CppTypeNoCV>::Value)
		{
			if (GetPlainSize() == sizeof(float))
				return static_cast<CppTypeNoCV>(*reinterpret_cast<const float*>(GetPlainData()));
			else if (GetPlainSize() == sizeof(double))
				return static_cast<CppTypeNoCV>(*reinterpret_cast<const double*>(GetPlainData()));
			else if (GetPlainSize() == sizeof(long double))
				return static_cast<CppTypeNoCV>(*reinterpret_cast<const long double*>(GetPlainData()));
			else if (!Data.IsEmpty())
				if constexpr (std::is_same_v<CppTypeNoCV, float>)
					return FCString::Atof(*Data);
				else
					return FCString::Atod(*Data);
			return static_cast<CppTypeNoCV>(0);
		}
		else if constexpr (TIsIntegral<CppTypeNoCV>::Value || TIsUEnum<CppTypeNoCV>)
		{
			using TDestType = typename std::conditional_t<TIsUEnum<CppTypeNoCV>, TUnderlyingType<CppTypeNoCV>, CppTypeNoCV>;
			if (GetPlainSize() == 0)
				if (Data.IsEmpty())
					return static_cast<CppTypeNoCV>(TDestType(0));
				else
					return static_cast<CppTypeNoCV>(TDestType(FCString::Atoi64(*Data)));
			else if (GetPlainSize() == sizeof(int8))
				return static_cast<CppTypeNoCV>(TDestType(*reinterpret_cast<const int8*>(GetPlainData())));
			else if (GetPlainSize() == sizeof(int16))
				return static_cast<CppTypeNoCV>(TDestType(*reinterpret_cast<const int16*>(GetPlainData())));
			else if (GetPlainSize() == sizeof(int32))
				return static_cast<CppTypeNoCV>(TDestType(*reinterpret_cast<const int32*>(GetPlainData())));
			else if (GetPlainSize() == sizeof(int64))
				return static_cast<CppTypeNoCV>(TDestType(*reinterpret_cast<const int64*>(GetPlainData())));
			return static_cast<CppTypeNoCV>(TDestType(*reinterpret_cast<const TDestType*>(GetPlainData())));
		}
		else if constexpr (std::is_enum_v<CppTypeNoCV>)
		{
			static_assert(!std::is_same_v<CppTypeNoCV, CppTypeNoCV>,
				"Only UENUM types with blueprint reflection are supported by FGeneric");
			return CppTypeNoCV();
		}
		else if constexpr (std::is_pointer_v<CppTypeNoCV> && std::is_convertible_v<CppTypeNoCV, UObject*>)
		{
			return Cast<std::remove_pointer_t<CppTypeNoCV>>(As<TSoftObjectPtr<>>().LoadSynchronous());
		}
		else if constexpr (std::is_pointer_v<CppTypeNoCV>)
		{
			static_assert(!std::is_same_v<CppTypeNoCV, CppTypeNoCV>,
				"Only UObject pointer types with blueprint reflection are supported by FGeneric");
			return nullptr;
		}
		else if constexpr (TIsSubclassOf<CppTypeNoCV>)
		{
			return Cast<UClass>(As<TSoftObjectPtr<>>().LoadSynchronous());
		}
		else if constexpr (std::is_same_v<CppTypeNoCV, FSoftObjectPath> || std::is_same_v<CppTypeNoCV, FSoftClassPath>)
		{
			return As<TSoftObjectPtr<>>();
		}
		else if constexpr (TIsUStruct<CppTypeNoCV>)
		{
			CppTypeNoCV Ans;
			UScriptStruct* Struct = CppTypeNoCV::StaticStruct();
			Struct->ImportText(*Data, &Ans, nullptr, 0, nullptr, Struct->GetName());
			return Ans;
		}
#pragma push_macro("GENERIC_PROPERTY")
#pragma push_macro("GENERIC_PROPERTY_OBJECT")
#define GENERIC_PROPERTY(InType, Name) \
		else if constexpr (std::is_same_v<CppTypeNoCV, InType>) \
		{ \
			InType Ans; \
			Get(&Ans, GET_GENERIC_PROP_PRIVATE(InType)); \
			return Ans; \
		}
		// END DEFINE GENERIC_PROPERTY
#define GENERIC_PROPERTY_OBJECT(CppType, Name)
#include "GenericProperties.inl"
#pragma pop_macro("GENERIC_PROPERTY")
#pragma pop_macro("GENERIC_PROPERTY_OBJECT")
		else
		{
			static_assert(!std::is_same_v<CppTypeNoCV, CppTypeNoCV>,
				"CppType must have blueprint reflection support to be used with FGeneric");
			return CppTypeNoCV();
		}
	}

	template<typename Type> FORCEINLINE operator const Type() const
	{
		return As<Type>();
	}

#endif // CPP
};

#pragma pop_macro("GET_GENERIC_PROP_PRIVATE")

/** Macro to set FGeneric from a class member */
#define SET_GENERIC_FROM_MEMBER(Generic, ClassName, MemberName, Object) \
{ Generic.Set(&Object->MemberName, ClassName::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(ClassName, MemberName))); }

/** Macro to set a class member from FGeneric */
#define SET_MEMBER_FROM_GENERIC(Generic, ClassName, MemberName, Object) \
{ Generic.Get(&Object->MemberName, ClassName::StaticClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(ClassName, MemberName))); }

/** Type traits for FGeneric */
template<>
struct TStructOpsTypeTraits<FGeneric> : public TStructOpsTypeTraitsBase2<FGeneric>
{
	enum
	{
		WithZeroConstructor = true,
		WithCopy = true,
		WithIdenticalViaEquality = true,
	};
};

/** Big-endian platform warning */
#if !PLATFORM_LITTLE_ENDIAN
#pragma message("CAUTION: This code assumes Little-Endian. Audit serialization logic for Big-Endian compatibility.")
#endif
