using UnrealBuildTool;

public class DreamFlowExpansion : ModuleRules
{
    public DreamFlowExpansion(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new[]
            {
                "Core",
                "CoreUObject",
                "DreamFlow",
                "Engine",
            });
    }
}
