// Copyright Liquid Fish. All Rights Reserved.

using UnrealBuildTool;

public class MaidGame : ModuleRules
{
	public MaidGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		DynamicallyLoadedModuleNames.AddRange(new string[] { });

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "BlueprintGraph" });
		}

		PublicIncludePaths.AddRange(new string[] { ModuleDirectory });
		PatchGenericTypes();
	}

    private void PatchGenericTypes()
    {
        string GenericFilePath = System.IO.Path.Combine(ModuleDirectory, "Generic", "Generic.h");

        if (System.IO.File.Exists(GenericFilePath))
        {
            string FileContent = System.IO.File.ReadAllText(GenericFilePath);
            string ModifiedContent = FileContent;

            if (Target.Version.MajorVersion >= 5)
            {
                ModifiedContent = ModifiedContent.Replace("\t//UPROPERTY() FVector2f", "\tUPROPERTY() FVector2f");
                ModifiedContent = ModifiedContent.Replace("\t//UPROPERTY() FVector3f", "\tUPROPERTY() FVector3f");
                ModifiedContent = ModifiedContent.Replace("\t//UPROPERTY() FVector4f", "\tUPROPERTY() FVector4f");
                ModifiedContent = ModifiedContent.Replace("\t//UPROPERTY() FMatrix44f", "\tUPROPERTY() FMatrix44f");
            }
            else
            {
                ModifiedContent = ModifiedContent.Replace("\tUPROPERTY() FVector2f", "\t//UPROPERTY() FVector2f");
                ModifiedContent = ModifiedContent.Replace("\tUPROPERTY() FVector3f", "\t//UPROPERTY() FVector3f");
                ModifiedContent = ModifiedContent.Replace("\tUPROPERTY() FVector4f", "\t//UPROPERTY() FVector4f");
                ModifiedContent = ModifiedContent.Replace("\tUPROPERTY() FMatrix44f", "\t//UPROPERTY() FMatrix44f");
            }

            if (FileContent != ModifiedContent)
            {
                System.IO.FileAttributes Attributes = System.IO.File.GetAttributes(GenericFilePath);
                if ((Attributes & System.IO.FileAttributes.ReadOnly) == System.IO.FileAttributes.ReadOnly)
                {
                    System.IO.File.SetAttributes(GenericFilePath, Attributes & ~System.IO.FileAttributes.ReadOnly);
                }

                System.IO.File.WriteAllText(GenericFilePath, ModifiedContent);
                if (Target.Version.MajorVersion >= 5)
                {
                    System.Console.WriteLine("[MaidGame] Generic.h has been upgraded to UE5");
                }
                else
                {
                    System.Console.WriteLine("[MaidGame] Generic.h has been downgraded to UE4");
                }
            }
        }
    }
}
