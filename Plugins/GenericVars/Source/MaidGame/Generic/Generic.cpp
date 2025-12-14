// Copyright Liquid Fish. All Rights Reserved.

#include "Generic/Generic.h"

#if WITH_EDITOR
#include "EdGraphSchema_K2.h"
#endif

const FGeneric FGeneric::Null = FGeneric();

void FGeneric::Set(const void* SrcPropertyAddress, const FProperty* SrcProperty)
{
	Clear();
	if (!(SrcProperty && SrcPropertyAddress)) return;
	if (IsPlain(SrcProperty))
	{
		SetPlainSize(SrcProperty->GetSize());
		SrcProperty->CopyCompleteValue(PlainData.GetData(), SrcPropertyAddress);
#if WITH_EDITOR
		ClearReferencedObjects();
#endif
#if WITH_EDITORONLY_DATA && WITH_EDITOR
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		K2Schema->ConvertPropertyToPinType(SrcProperty, EditPinType);
#endif
	}
	else
	{
		FString ExportedText;
		SrcProperty->ExportText_Direct(ExportedText, SrcPropertyAddress, 
			/*Src as delta to force full export*/ SrcPropertyAddress, nullptr, PPF_None);
#if WITH_EDITORONLY_DATA && WITH_EDITOR
		if (ExportedText != Data)
#endif
		{
			Data = MoveTemp(ExportedText);
#if WITH_EDITORONLY_DATA && WITH_EDITOR
			const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
			K2Schema->ConvertPropertyToPinType(SrcProperty, EditPinType);
#endif
		}
#if WITH_EDITOR
		CacheReferencedObjects(SrcProperty, SrcPropertyAddress);
#endif

#if GENERIC_USING_CACHE
		DataCache.Set(SrcPropertyAddress, SrcProperty);
#endif
	}
}

void FGeneric::Get(void* DestPropertyAddress, const FProperty* DestProperty) const
{
	if (!(DestPropertyAddress && DestProperty)) return;
	if (IsPlain(DestProperty))
	{
		if (GetPlainSize() >= DestProperty->GetSize())
			DestProperty->CopyCompleteValue(DestPropertyAddress, PlainData.GetData());
		else
			DestProperty->ClearValue(DestPropertyAddress);
	}
	else
	{
#if GENERIC_USING_CACHE
		if (DataCache.ConditionalGet(DestPropertyAddress, DestProperty)) return;
#endif
		DestProperty->ClearValue(DestPropertyAddress);
#if UE_VERSION_NEWER_THAN(5, 1, 0)
		DestProperty->ImportText_Direct(*Data, DestPropertyAddress, nullptr, PPF_None, nullptr);
#else
		DestProperty->ImportText(*Data, DestPropertyAddress, PPF_None, nullptr, nullptr);
#endif
	}
}

void FGeneric::Clear()
{
#if WITH_EDITORONLY_DATA
	EditPinType = FEdGraphPinType();
#endif
	Data.Reset();
	PlainData.Reset();
#if GENERIC_USING_CACHE
	DataCache.Clear();
#endif
#if WITH_EDITOR
	ClearReferencedObjects();
#endif
}

const bool FGeneric::IsPlain(const FProperty* Prop)
{
	static constexpr auto NonPlainCastFlags =
		EClassCastFlags::CASTCLASS_FObjectProperty |
		EClassCastFlags::CASTCLASS_FObjectPropertyBase |
		EClassCastFlags::CASTCLASS_FWeakObjectProperty |
		EClassCastFlags::CASTCLASS_FLazyObjectProperty |
		EClassCastFlags::CASTCLASS_FSoftObjectProperty |
		EClassCastFlags::CASTCLASS_FClassProperty |
		EClassCastFlags::CASTCLASS_FSoftClassProperty |
		EClassCastFlags::CASTCLASS_FStrProperty |
		EClassCastFlags::CASTCLASS_FNameProperty |
		EClassCastFlags::CASTCLASS_FTextProperty |
		EClassCastFlags::CASTCLASS_FArrayProperty |
		EClassCastFlags::CASTCLASS_FMapProperty |
		EClassCastFlags::CASTCLASS_FSetProperty;

	static constexpr auto PlainCastFlags =
		EClassCastFlags::CASTCLASS_FFloatProperty |
		EClassCastFlags::CASTCLASS_FDoubleProperty |
		EClassCastFlags::CASTCLASS_FBoolProperty |
		EClassCastFlags::CASTCLASS_FInt8Property |
		EClassCastFlags::CASTCLASS_FInt16Property |
		EClassCastFlags::CASTCLASS_FIntProperty |
		EClassCastFlags::CASTCLASS_FInt64Property |
		EClassCastFlags::CASTCLASS_FByteProperty |
		EClassCastFlags::CASTCLASS_FUInt16Property |
		EClassCastFlags::CASTCLASS_FUInt32Property |
		EClassCastFlags::CASTCLASS_FUInt64Property;

	static constexpr auto StructCastFlags =
		EClassCastFlags::CASTCLASS_FStructProperty;

	static auto StaticGetBaseStructureInternal = [](const TCHAR* Package, const TCHAR* Name)
		{
			return FindObjectChecked<UScriptStruct>(nullptr, *FString::Printf(TEXT("/Script/%s.%s"), Package, Name));
		};

	static const TSet<const UScriptStruct*> PlainStructs = 
	{
#if UE_VERSION_NEWER_THAN(5, 0, 0)
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Vector2f")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Vector3f")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Vector4f")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Matrix44f")),
#endif

		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Vector")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Vector2D")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Vector4")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("IntVector")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("IntPoint")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Rotator")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Quat")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Transform")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Matrix")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("LinearColor")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Color")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Plane")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Guid")),

		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Box")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Box2D")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("BoxSphereBounds")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("OrientedBox")),

		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("FloatRange")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("FloatRangeBound")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Int32Range")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Int32RangeBound")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("FloatInterval")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Int32Interval")),

		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("DateTime")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("Timespan")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("RandomStream")),

		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("FrameNumber")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("FrameTime")),

		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("TwoVectors")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("InterpCurvePointFloat")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("InterpCurvePointVector2D")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("InterpCurvePointVector")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("InterpCurvePointQuat")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("InterpCurvePointTwoVectors")),
		StaticGetBaseStructureInternal(TEXT("CoreUObject"), TEXT("InterpCurvePointLinearColor")),
	};

	if (Prop)
	{
		const auto PropCastFlags = Prop->GetCastFlags();
		if (PropCastFlags & NonPlainCastFlags) return false;
		if (PropCastFlags & PlainCastFlags) return true;
		if (PropCastFlags & StructCastFlags)
		{
			auto* StructProp = ((FStructProperty*)Prop);
			if (PlainStructs.Contains(StructProp->Struct)) return true;
		}
	}
	return false;
}

#if WITH_EDITOR
static struct
{
	TSoftObjectPtr<UObject> GetSoftObjectValue(const FProperty* Prop, const void* Data)
	{
		const auto PropCastFlags = Prop->GetCastFlags();
		if (0) {}
		else if (PropCastFlags & CASTCLASS_FSoftClassProperty) return TSoftObjectPtr<UObject>(FSoftClassProperty::GetPropertyValue(Data).ToSoftObjectPath());
		else if (PropCastFlags & CASTCLASS_FSoftObjectProperty) return TSoftObjectPtr<UObject>(FSoftObjectProperty::GetPropertyValue(Data).ToSoftObjectPath());
		else if (PropCastFlags & CASTCLASS_FLazyObjectProperty) return ((FWeakObjectPtr*)&FLazyObjectProperty::GetPropertyValue(Data))->Get();
		else if (PropCastFlags & CASTCLASS_FWeakObjectProperty) return FWeakObjectProperty::GetPropertyValue(Data).Get();
		else if (PropCastFlags & CASTCLASS_FClassProperty) return FClassProperty::GetPropertyValue(Data);
		else if (PropCastFlags & CASTCLASS_FObjectProperty) return FObjectProperty::GetPropertyValue(Data);
		else if (PropCastFlags & CASTCLASS_FObjectPropertyBase) ensure(false);
		return nullptr;
	}
	void operator()(TArray<TSoftObjectPtr<UObject>>& RefList, const UScriptStruct* Struct, const void* Data)
	{
		for (auto* SubProp = Struct->PropertyLink; SubProp; SubProp = SubProp->PropertyLinkNext)
		{
			(*this)(RefList, SubProp, SubProp->ContainerPtrToValuePtr<void>(Data));
		}
	}
	void operator()(TArray<TSoftObjectPtr<UObject>>& RefList, const FProperty* Prop, const void* Data)
	{
		if (Prop->IsA<FStructProperty>())
		{
			if (FGeneric::IsPlain(Prop)) return;
			(*this)(RefList, ((FStructProperty*)Prop)->Struct, Data);
		}
		else
		{
			const auto& SoftRef = GetSoftObjectValue(Prop, Data);
			if (!SoftRef.IsNull())
			{
				RefList.AddUnique(SoftRef);
			}
		}
	}
} CacheReferencedObjectsImpl;

void FGeneric::CacheReferencedObjects(const FProperty* InProperty, const void* InData)
{
	ClearReferencedObjects();

	if (InProperty && InData)
	{
		CacheReferencedObjectsImpl(ReferencedObjects, InProperty, InData);
	}
}

void FGeneric::CacheReferencedObjects(const UScriptStruct* InProperty, const void* InData)
{
	ClearReferencedObjects();

	if (InProperty && InData)
	{
		CacheReferencedObjectsImpl(ReferencedObjects, InProperty, InData);
	}
}

void FGeneric::ClearReferencedObjects()
{
	ReferencedObjects.Reset();
}
#endif
