#include "DreamFlowExpansionEditorModule.h"

#include "AssetTypeActions_Base.h"
#include "DreamDialogueFlowAsset.h"
#include "DreamEncounterFlowAsset.h"
#include "DreamFlowAsset.h"
#include "DreamFlowEditorToolkit.h"
#include "DreamFlowExpansionAssetFactory.h"
#include "DreamQuestFlowAsset.h"
#include "DreamStoryFlowAsset.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "DreamFlowExpansionEditorModule"

namespace
{
    class FDreamFlowExpansionAssetTypeActions final : public FAssetTypeActions_Base
    {
    public:
        FDreamFlowExpansionAssetTypeActions(uint32 InAssetCategory, UClass* InSupportedClass, FText InDisplayName, FColor InTypeColor)
            : AssetCategory(InAssetCategory)
            , SupportedClass(InSupportedClass)
            , DisplayName(MoveTemp(InDisplayName))
            , TypeColor(InTypeColor)
        {
        }

        virtual FText GetName() const override
        {
            return DisplayName;
        }

        virtual FColor GetTypeColor() const override
        {
            return TypeColor;
        }

        virtual UClass* GetSupportedClass() const override
        {
            return SupportedClass;
        }

        virtual uint32 GetCategories() override
        {
            return AssetCategory;
        }

        virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override
        {
            const EToolkitMode::Type ToolkitMode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

            for (UObject* Object : InObjects)
            {
                if (UDreamFlowAsset* FlowAsset = Cast<UDreamFlowAsset>(Object))
                {
                    TSharedRef<FDreamFlowEditorToolkit> EditorToolkit = MakeShared<FDreamFlowEditorToolkit>();
                    EditorToolkit->InitEditor(ToolkitMode, EditWithinLevelEditor, FlowAsset);
                }
            }
        }

    private:
        uint32 AssetCategory = 0;
        UClass* SupportedClass = nullptr;
        FText DisplayName;
        FColor TypeColor = FColor::White;
    };
}

void FDreamFlowExpansionEditorModule::StartupModule()
{
    RegisterAssetTools();
}

void FDreamFlowExpansionEditorModule::ShutdownModule()
{
    UnregisterAssetTools();
}

void FDreamFlowExpansionEditorModule::RegisterAssetTools()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("DreamFlow")), LOCTEXT("DreamFlowAssetCategory", "DreamFlow"));

    const auto RegisterTypeAction = [this, &AssetTools](UClass* SupportedClass, const FText& DisplayName, const FColor& TypeColor)
    {
        TSharedRef<FDreamFlowExpansionAssetTypeActions> AssetTypeAction = MakeShared<FDreamFlowExpansionAssetTypeActions>(
            AssetCategory,
            SupportedClass,
            DisplayName,
            TypeColor);
        AssetTools.RegisterAssetTypeActions(AssetTypeAction);
        RegisteredAssetTypeActions.Add(AssetTypeAction);
    };

    RegisterTypeAction(UDreamQuestFlowAsset::StaticClass(), LOCTEXT("DreamQuestFlowAssetName", "Dream Quest Flow"), FColor(61, 158, 111));
    RegisterTypeAction(UDreamDialogueFlowAsset::StaticClass(), LOCTEXT("DreamDialogueFlowAssetName", "Dream Dialogue Flow"), FColor(67, 113, 217));
    RegisterTypeAction(UDreamStoryFlowAsset::StaticClass(), LOCTEXT("DreamStoryFlowAssetName", "Dream Story Flow"), FColor(188, 74, 112));
    RegisterTypeAction(UDreamEncounterFlowAsset::StaticClass(), LOCTEXT("DreamEncounterFlowAssetName", "Dream Encounter Flow"), FColor(51, 122, 173));
}

void FDreamFlowExpansionEditorModule::UnregisterAssetTools()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        RegisteredAssetTypeActions.Reset();
        return;
    }

    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

    for (const TSharedPtr<IAssetTypeActions>& AssetTypeAction : RegisteredAssetTypeActions)
    {
        if (AssetTypeAction.IsValid())
        {
            AssetTools.UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
        }
    }

    RegisteredAssetTypeActions.Reset();
}

IMPLEMENT_MODULE(FDreamFlowExpansionEditorModule, DreamFlowExpansionEditor)

#undef LOCTEXT_NAMESPACE
