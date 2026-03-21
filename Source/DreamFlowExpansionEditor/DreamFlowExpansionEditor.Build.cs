using UnrealBuildTool;

public class DreamFlowExpansionEditor : ModuleRules
{
    public DreamFlowExpansionEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "AssetTools",
                "Core",
                "CoreUObject",
                "DreamFlow",
                "DreamFlowEditor",
                "DreamFlowExpansion",
                "Engine",
                "UnrealEd",
            });
    }
}
