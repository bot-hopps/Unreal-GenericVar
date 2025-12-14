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

#define LOCTEXT_NAMESPACE "BlueprintDetailsCustomization"

TSharedRef<IPropertyTypeCustomization> FGenericStructCustomization::MakeInstance()
{
	return MakeShareable(new FGenericStructCustomization);
}

void FGenericStructCustomization::CustomizeStructHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IStructCustomizationUtils& StructCustomizationUtils)
{
	PropertyHandle = StructPropertyHandle;
	const FString DocLink = TEXT("");
	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
	TSharedPtr<SToolTip> VarTypeTooltip = IDocumentation::Get()->CreateToolTip(LOCTEXT("VarTypeTooltip", "The type of the variable."), NULL, DocLink, TEXT("VariableType"));
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
		void* PropertyAddress = nullptr;
		PropertyHandleShared->GetValueData(PropertyAddress);
		if (PropertyAddress)
		{
			FGeneric& Generic = *(FGeneric*)PropertyAddress;
			return Generic.EditPinType;
		}
	}
	return FEdGraphPinType();
}

void FGenericStructCustomization::OnVarTypeChanged(const FEdGraphPinType& NewPinType)
{
	if (auto PropertyHandleShared = PropertyHandle.Pin())
	{
		if (FGenericStructCustomization::IsPinTypeValid(NewPinType))
		{
			TArray<void*> RawData;
			PropertyHandleShared->AccessRawData(RawData);
			for (int32 i = 0; i < RawData.Num(); ++i)
			{
				FGeneric& Generic = *(FGeneric*)RawData[i];
				Generic.EditPinType = NewPinType;
				Generic.ClearReferencedObjects();
			}
		}
	}
	if (LastChildBuilder)
	{
		LastChildBuilder->GetParentCategory().GetParentLayout().ForceRefreshDetails();
	}
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

void FGenericStructCustomization::CreateChildrenInternal(IDetailChildrenBuilder& ChildBuilder)
{
	// Generate or get auxiliary structs for reflection.

	auto StructPropertyHandle = PropertyHandle.Pin();
	if (!StructPropertyHandle.IsValid() || !StructPropertyHandle->GetProperty())
	{
		return;
	}

	TArray<UPackage*> EditingPackages;
	StructPropertyHandle->GetOuterPackages(EditingPackages);

	TArray<void*> RawData;
	StructPropertyHandle->AccessRawData(RawData);
	if (RawData.Num() == 0)
	{
		return;
	}

	FGeneric& Generic = *(FGeneric*)RawData[0];
	if (!FGenericStructCustomization::IsPinTypeValid(Generic.EditPinType))
	{
		return;
	}

	for (int32 i = 1; i < RawData.Num(); ++i)
	{
		if (((FGeneric*)RawData[i])->EditPinType != Generic.EditPinType)
		{
			return;
		}
	}

	FProperty* EditProperty = nullptr;
	const FName EditPropertyName = FName("Value");
	UUserDefinedStruct*& EditorStruct = Generic.EditorStruct;
	TArray<FStructVariableDescription> EditorStructMem;
	
	if (EditorStruct)
	{
		EditorStructMem = FStructureEditorUtils::GetVarDesc(EditorStruct);
		EditProperty = EditorStruct->PropertyLink;
	}

	// Check if we need to regenerate the struct property
	if (EditProperty == nullptr || (EditorStruct && EditorStructMem.Num() == 1 || (
		EditorStructMem[0].Category != Generic.EditPinType.PinCategory ||
		EditorStructMem[0].SubCategory != Generic.EditPinType.PinSubCategory ||
		EditorStructMem[0].SubCategoryObject.Get() != Generic.EditPinType.PinSubCategoryObject ||
		EditorStructMem[0].ContainerType != Generic.EditPinType.ContainerType ||
		EditorStructMem[0].PinValueType != Generic.EditPinType.PinValueType
		)))
	{
		EditorStruct = nullptr;

		static TMap<FEdGraphPinType, TWeakObjectPtr<UUserDefinedStruct>> StructCache;
		if (auto* CachedStruct = StructCache.Find(Generic.EditPinType))
		{
			EditorStruct = CachedStruct->Get();
			if (EditorStruct)
			{
				EditorStructMem = FStructureEditorUtils::GetVarDesc(EditorStruct);
				EditProperty = EditorStruct->PropertyLink;
			}
		}
		
		if (EditorStruct == nullptr)
		{
			EditorStruct = FStructureEditorUtils::CreateUserDefinedStruct(
				GetTransientPackage(),
				(FName)*FString::Printf(TEXT("Generic_GEN_%s"), *FGuid::NewGuid().ToString(EGuidFormats::Base36Encoded)),
				EObjectFlags::RF_Transient
			);
			EditorStruct->SetMetaData(FBlueprintMetadata::MD_NotAllowableBlueprintVariableType, TEXT("true"));
			// We don't need to call FStructureEditorUtils::AddVariable since there's already a default var in the field.
			EditorStructMem = FStructureEditorUtils::GetVarDesc(EditorStruct);
			FStructureEditorUtils::ChangeVariableType(EditorStruct, EditorStructMem[0].VarGuid, Generic.EditPinType);
			FStructureEditorUtils::RenameVariable(EditorStruct, EditorStructMem[0].VarGuid, EditPropertyName.ToString());
			EditProperty = EditorStruct->PropertyLink;

			StructCache.Add(Generic.EditPinType, EditorStruct);
		}
	}

	// Generate an instance struct for reflection
	if (Generic.EditorStruct && EditorStructMem.Num() == 1)
	{
		TSharedRef<FStructOnScope> StructOnScope = MakeShareable<FStructOnScope>(
			new FStructOnScope(Generic.EditorStruct)
			);
		StructOnScope->SetPackage(GetTransientPackage());
		if (!Generic.IsEmpty())
		{
			Generic.Get(StructOnScope->GetStructMemory(), EditProperty);
		}
		else
		{
			Generic.Set(StructOnScope->GetStructMemory(), EditProperty);
			for (auto* Package : EditingPackages) Package->MarkPackageDirty();
		}
		FAddPropertyParams Params;
		Params.ForceShowProperty();
		auto* ValueRow = ChildBuilder.AddExternalStructureProperty(StructOnScope, *(EditProperty->GetName()), Params);
		auto OnValueChangedLambda = FSimpleDelegate::CreateLambda([RawData, StructOnScope, EditProperty]()
			{
				check(EditProperty && StructOnScope->GetStructMemory());
				for (void* RawDataPtr : RawData)
				{
					FGeneric& Generic = *(FGeneric*)RawDataPtr;
					Generic.Set(StructOnScope->GetStructMemory(), EditProperty);
				}
			});
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

bool FGenericStructCustomization::IsPinTypeValid(const FEdGraphPinType& Type)
{
	return FEdGraphPinType() != Type;
}
