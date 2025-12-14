// Copyright Liquid Fish. All Rights Reserved.

#include "MaidEditor.h"
#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Modules/ModuleManager.h"
#include "Features/IModularFeatures.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyEditorModule.h"
#include "Generic/GenericStructCustomizations.h"

#define LOCTEXT_NAMESPACE "FMaidEditor"

void FMaidEditor::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("Generic", FOnGetPropertyTypeCustomizationInstance::CreateLambda(
		[]() -> auto{ return FGenericStructCustomization::MakeInstance(); }));
}

void FMaidEditor::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout("Generic");
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMaidEditor, MaidEditor)