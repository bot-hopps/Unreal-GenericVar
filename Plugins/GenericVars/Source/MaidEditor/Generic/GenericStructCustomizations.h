// Copyright Liquid Fish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "SGraphPin.h"
#include "IDetailPropertyRow.h"
#include "Generic/Generic.h"

/**
 * FGeneric editor reflection
 */
class MAIDEDITOR_API FGenericStructCustomization : public IStructCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
public:
	virtual void CustomizeStructHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IStructCustomizationUtils& StructCustomizationUtils) override;
	virtual void CustomizeStructChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& ChildBuilder, IStructCustomizationUtils& StructCustomizationUtils) override;
public:
	TWeakPtr<IPropertyHandle> PropertyHandle;
	IDetailChildrenBuilder* LastChildBuilder = nullptr;
	bool GetVariableTypeChangeEnabled() const;
	FEdGraphPinType OnGetVarType() const;
	void OnVarTypeChanged(const FEdGraphPinType& NewPinType);
private:
	void CreateChildrenInternal(IDetailChildrenBuilder& ChildBuilder);
public:
	static bool IsPinTypeValid(const FEdGraphPinType& Type);
};
