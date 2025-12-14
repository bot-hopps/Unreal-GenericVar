// Copyright Liquid Fish. All Rights Reserved.

using UnrealBuildTool;

public class MaidEditor : ModuleRules
{
    public MaidEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UMG" });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "MaidGame",
            "UnrealEd", "PropertyEditor", "KismetWidgets", "BlueprintGraph",
            "EditorStyle", "ClassViewer", "GraphEditor", "PropertyEditor",
            "UnrealEd", "DesktopPlatform", "SlateCore", "Slate", "AssetRegistry", 
		});

        DynamicallyLoadedModuleNames.AddRange(new string[] { });

        PublicIncludePaths.AddRange(new string[] { ModuleDirectory });
    }
}
