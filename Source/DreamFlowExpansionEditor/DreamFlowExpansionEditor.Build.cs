using UnrealBuildTool;

public class DreamFlowExpansionEditor : ModuleRules
{
    public DreamFlowExpansionEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new[]
            {
                "AssetRegistry",
                "AssetTools",
                "ContentBrowser",
                "ContentBrowserData",
                "Core",
                "CoreUObject",
                "DesktopPlatform",
                "DreamFlow",
                "DreamFlowEditor",
                "DreamFlowExpansion",
                "Engine",
                "Slate",
                "SlateCore",
                "ToolMenus",
                "UnrealEd",
            });
    }
}
