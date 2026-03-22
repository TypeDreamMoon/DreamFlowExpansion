#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class IAssetTypeActions;

class DREAMFLOWEXPANSIONEDITOR_API FDreamFlowExpansionEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterAssetTools();
    void UnregisterAssetTools();
    void RegisterMenus();
    void AddDreamFlowExpansionAddNewMenu(struct FToolMenuSection& Section);

private:
    uint32 AssetCategory = 0;
    TArray<TSharedPtr<IAssetTypeActions>> RegisteredAssetTypeActions;
};
