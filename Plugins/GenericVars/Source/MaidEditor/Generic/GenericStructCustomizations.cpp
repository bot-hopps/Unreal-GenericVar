// Copyright Liquid Fish. All Rights Reserved.

#include "GenericStructCustomizations.h"
#include "SlateOptMacros.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "SPinTypeSelector.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "EdGraph/EdGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/StructureEditorUtils.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "IDocumentation.h"
#include "IDetailChildrenBuilder.h"
#include "Editor/TransBuffer.h"

#define LOCTEXT_NAMESPACE "GenericStructCustomization"

static FORCEINLINE const bool IsPinTypeValid(const FEdGraphPinType& Type)
{
	return FEdGraphPinType() != Type;
}

static FORCEINLINE const bool IsEquivalent(const FStructVariableDescription& VarDesc, const FEdGraphPinType& PinType)
{
	return VarDesc.Category == PinType.PinCategory
		&& VarDesc.SubCategory == PinType.PinSubCategory
		&& VarDesc.SubCategoryObject.Get() == PinType.PinSubCategoryObject
		&& VarDesc.ContainerType == PinType.ContainerType
		&& VarDesc.PinValueType == PinType.PinValueType;
}

static void ModifyEditingObjects(IPropertyHandle* PropertyHandle)
{
	if (!PropertyHandle)
	{
		return;
	}
	TArray<UObject*> ModifiedObjects;
	PropertyHandle->GetOuterObjects(ModifiedObjects);
	for (auto Object : ModifiedObjects)
	{
		if (IsValid(Object))
			Object->Modify();
	}
}

TSharedRef<IPropertyTypeCustomization> FGenericStructCustomization::MakeInstance()
{
	TSharedRef<FGenericStructCustomization> Instance = MakeShareable(new FGenericStructCustomization);
	if (auto* TransBuffer = Cast<UTransBuffer>(GEditor ? GEditor->Trans : nullptr))
	{
		Instance->UndoHandle = TransBuffer->OnUndo().AddLambda([WeakInst = TWeakPtr<FGenericStructCustomization>(Instance)]
		(const FTransactionContext& TransactionContext, bool Succeeded)
			{
				if (Succeeded)
				{
					if (auto* InstPtr = WeakInst.Pin().Get())
					{
						InstPtr->ForceRebuildDetails();
					}
				}
			}
		);
	}
	return Instance;
}

FGenericStructCustomization::~FGenericStructCustomization()
{
	if (UndoHandle.IsValid())
	{
		if (auto* TransBuffer = Cast<UTransBuffer>(GEditor ? GEditor->Trans : nullptr))
		{
			TransBuffer->OnUndo().Remove(UndoHandle);
			UndoHandle.Reset();
		}
	}
}

void FGenericStructCustomization::CustomizeStructHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IStructCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	TSharedPtr<SToolTip> VarTypeTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarTypeTooltip", "The type of the variable."), NULL, TEXT(""), TEXT("VariableType"));
	const FSlateFontInfo DetailFontInfo = IDetailLayoutBuilder::GetDetailFont();
	HeaderRow
		.NameContent()
		[
			SNew(STextBlock)
				//.Text(LOCTEXT("VariableTypeLabel", "Variable Type"))
				.Text(StructPropertyHandle->GetPropertyDisplayName())
				.ToolTip(VarTypeTooltip)
				.Font(DetailFontInfo)
		]
	.ValueContent()
		.MaxDesiredWidth(980.f)
		[
			SNew(SPinTypeSelector, FGetPinTypeTree::CreateUObject(K2Schema, &UEdGraphSchema_K2::GetVariableTypeTree))
				.TargetPinType(this, &FGenericStructCustomization::OnGetVarType)
				.OnPinTypeChanged(this, &FGenericStructCustomization::OnVarTypeChanged)
				.IsEnabled(this, &FGenericStructCustomization::GetVariableTypeChangeEnabled)
				.Schema(K2Schema)
				.TypeTreeFilter(ETypeTreeFilter::None)
				.bAllowArrays(true)
				.Font(DetailFontInfo)
		]
	;
}

void FGenericStructCustomization::CustomizeStructChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& ChildBuilder, IStructCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;
	LastChildBuilder = &ChildBuilder;
	CreateChildrenInternal(ChildBuilder);
}

bool FGenericStructCustomization::GetVariableTypeChangeEnabled() const
{
	return PropertyHandle.IsValid();
}

FEdGraphPinType FGenericStructCustomization::OnGetVarType() const
{
	if (auto PropertyHandleShared = PropertyHandle.Pin())
	{
		void* PropertyPtr = nullptr;
		PropertyHandleShared->GetValueData(PropertyPtr);
		if (PropertyPtr)
		{
			FGeneric& Generic = *reinterpret_cast<FGeneric*>(PropertyPtr);
			return Generic.GetEditPinType();
		}
	}
	return FEdGraphPinType();
}

void FGenericStructCustomization::OnVarTypeChanged(const FEdGraphPinType& NewPinType)
{
	if (auto PropertyHandleShared = PropertyHandle.Pin())
	{
		ModifyEditingObjects(PropertyHandleShared.Get());

		TArray<void*> RawData;
		PropertyHandleShared->AccessRawData(RawData);
		for (int32 i = 0; i < RawData.Num(); ++i)
		{
			FGeneric& Generic = *reinterpret_cast<FGeneric*>(RawData[i]);
			if (Generic.GetEditPinType() != NewPinType)
			{
				Generic.Clear();
				Generic.SetEditPinType(NewPinType);
			}
		}

		PropertyHandleShared->NotifyPostChange(EPropertyChangeType::ValueSet);
		PropertyHandleShared->NotifyFinishedChangingProperties();
	}

	ForceRebuildDetails();
}

uint32 GetTypeHash(const FEdGraphPinType& Struct)
{
	return HashCombine(
		HashCombine(
			HashCombine(
				HashCombine(GetTypeHash(Struct.PinCategory), GetTypeHash(Struct.PinSubCategory)),
				HashCombine(GetTypeHash(Struct.PinSubCategoryObject.Get()), GetTypeHash(Struct.PinSubCategoryMemberReference.MemberGuid))
			),
			HashCombine(
				GetTypeHash((int32)Struct.ContainerType),
				GetTypeHash(((int32)Struct.bIsReference) | ((int32)Struct.bIsConst << 1) | ((int32)Struct.bIsWeakPointer << 2) | ((int32)Struct.bIsUObjectWrapper << 3))
			)
		),
		HashCombine(
			HashCombine(GetTypeHash(Struct.PinValueType.TerminalCategory), GetTypeHash(Struct.PinValueType.TerminalSubCategory)),
			HashCombine(GetTypeHash(Struct.PinValueType.TerminalSubCategoryObject.Get()),
				GetTypeHash(((int32)Struct.PinValueType.bTerminalIsConst) | ((int32)Struct.PinValueType.bTerminalIsWeakPointer << 1) | ((int32)Struct.PinValueType.bTerminalIsUObjectWrapper << 2)))
		)
	);
}

void FGenericStructCustomization::ForceRebuildDetails()
{
	if (LastChildBuilder && LastChildBuilder->GetParentCategory().IsParentLayoutValid())
	{
		LastChildBuilder->GetParentCategory().GetParentLayout().ForceRefreshDetails();
	}
}

void FGenericStructCustomization::CreateChildrenInternal(IDetailChildrenBuilder& ChildBuilder)
{
	// Generate or get auxiliary structs for reflection.

	auto StructPropertyHandle = PropertyHandle.Pin();
	if (!StructPropertyHandle.IsValid() || !StructPropertyHandle->GetProperty())
	{
		return;
	}

	TArray<void*> RawData;
	StructPropertyHandle->AccessRawData(RawData);
	if (RawData.Num() == 0)
	{
		return;
	}

	FGeneric& LeadGenericVar = *reinterpret_cast<FGeneric*>(RawData[0]);
	FEdGraphPinType TargetPinType = LeadGenericVar.GetEditPinType();
	if (!IsPinTypeValid(TargetPinType))
	{
		return;
	}

	if (RawData.ContainsByPredicate([TargetPinType](const void* GenericPtr)
		{ return reinterpret_cast<const FGeneric*>(GenericPtr)->GetEditPinType() != TargetPinType; }))
	{
		return;
	}

	FProperty* EditProperty = nullptr;
	const FName EditPropertyName = FName("Value");
	UUserDefinedStruct*& EditorStruct = LeadGenericVar.EditorStruct;
	TArray<FStructVariableDescription> StructLayout;

	auto TryRetrieveEditProperty = [&]()
		{
			if (EditorStruct)
			{
				StructLayout = FStructureEditorUtils::GetVarDesc(EditorStruct);
				EditProperty = EditorStruct->PropertyLink;
			}
		};

	TryRetrieveEditProperty();

	// Check if we need to regenerate the struct property
	if (EditProperty == nullptr
		|| StructLayout.Num() != 1
		|| !IsEquivalent(StructLayout[0], TargetPinType))
	{
		EditorStruct = nullptr;
		EditProperty = nullptr;
		StructLayout.Reset();

		static TMap<FEdGraphPinType, TWeakObjectPtr<UUserDefinedStruct>> StructCache;
		if (auto* CachedStruct = StructCache.Find(TargetPinType))
		{
			EditorStruct = CachedStruct->Get();
			TryRetrieveEditProperty();
		}

		if (EditProperty == nullptr)
		{
			EditorStruct = FStructureEditorUtils::CreateUserDefinedStruct(
				GetTransientPackage(),
				(FName)*FString::Printf(TEXT("GENERIC_DUMMY_%s"), *FGuid::NewGuid().ToString(EGuidFormats::Base36Encoded)),
				EObjectFlags::RF_Transient
			);
			EditorStruct->SetMetaData(FBlueprintMetadata::MD_NotAllowableBlueprintVariableType, TEXT("true"));
			// We don't need to call FStructureEditorUtils::AddVariable since there's already a default var in the field.
			StructLayout = FStructureEditorUtils::GetVarDesc(EditorStruct);
			FStructureEditorUtils::ChangeVariableType(EditorStruct, StructLayout[0].VarGuid, TargetPinType);
			FStructureEditorUtils::RenameVariable(EditorStruct, StructLayout[0].VarGuid, EditPropertyName.ToString());
			EditProperty = EditorStruct->PropertyLink;

			StructCache.Add(TargetPinType, EditorStruct);
		}
	}

	// Generate an instance struct for reflection
	if (EditorStruct && ensure(EditProperty))
	{
		ensureAlways(StructLayout.Num() == 1);

		// If empty, the type was likely just set - initialize content now.
		if (RawData.ContainsByPredicate([](void* GenericPtr) { return reinterpret_cast<FGeneric*>(GenericPtr)->IsEmpty(); }))
		{
			FStructOnScope EmptyValue(EditorStruct);

			for (auto* GenericPtr : RawData)
			{
				FGeneric& Generic = *reinterpret_cast<FGeneric*>(GenericPtr);
				if (Generic.IsEmpty())
				{
					Generic.Set(EmptyValue.GetStructMemory(), EditProperty);
				}
			}
			
			ModifyEditingObjects(StructPropertyHandle.Get());
		}

		TSharedRef<FStructOnScope> StructOnScope = MakeShareable<FStructOnScope>(
			new FStructOnScope(EditorStruct)
		);
		StructOnScope->SetPackage(GetTransientPackage());
		LeadGenericVar.Get(StructOnScope->GetStructMemory(), EditProperty);

		FAddPropertyParams Params;
		Params.ForceShowProperty();
		auto* ValueRow = ChildBuilder.AddExternalStructureProperty(StructOnScope, *(EditProperty->GetName()), Params);
		auto OnValuePreChangeLambda = FSimpleDelegate::CreateLambda([=]()
			{
				ModifyEditingObjects(StructPropertyHandle.Get());
			}
		);
		auto OnValueChangedLambda = FSimpleDelegate::CreateLambda([=, &LeadGenericVar]()
			{
				check(EditProperty && StructOnScope->GetStructMemory());
				LeadGenericVar.Set(StructOnScope->GetStructMemory(), EditProperty);
				for (void* Data : RawData)
				{
					if (Data == &LeadGenericVar) continue;
					FGeneric& GenericVar = *reinterpret_cast<FGeneric*>(Data);
					GenericVar = LeadGenericVar;
				}
				StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
				StructPropertyHandle->NotifyFinishedChangingProperties();
			});
		ValueRow->GetPropertyHandle()->SetOnPropertyValuePreChange(OnValuePreChangeLambda);
		ValueRow->GetPropertyHandle()->SetOnPropertyValueChanged(OnValueChangedLambda);
		ValueRow->GetPropertyHandle()->SetOnChildPropertyValueChanged(OnValueChangedLambda);
		ValueRow->ShouldAutoExpand(true);
	}
	// Unhandled
	else
	{
		FText ErrorMessage = LOCTEXT("Generic_UnknownPropertyType", "Unknown property type");
		ChildBuilder.AddCustomRow
		(ErrorMessage)
			[
				SNew(STextBlock)
					.ToolTipText(ErrorMessage)
					.Text(ErrorMessage)
					.Font(IDetailLayoutBuilder::GetDetailFont())
			];
	}
}

#undef LOCTEXT_NAMESPACE
