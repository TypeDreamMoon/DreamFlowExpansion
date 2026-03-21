#pragma once

#include "CoreMinimal.h"
#include "DreamFlowAssetFactory.h"
#include "DreamFlowExpansionAssetFactory.generated.h"

UCLASS()
class DREAMFLOWEXPANSIONEDITOR_API UDreamQuestFlowAssetFactory : public UDreamFlowAssetFactory
{
    GENERATED_BODY()

public:
    UDreamQuestFlowAssetFactory();

    virtual FText GetDisplayName() const override;
};

UCLASS()
class DREAMFLOWEXPANSIONEDITOR_API UDreamDialogueFlowAssetFactory : public UDreamFlowAssetFactory
{
    GENERATED_BODY()

public:
    UDreamDialogueFlowAssetFactory();

    virtual FText GetDisplayName() const override;
};

UCLASS()
class DREAMFLOWEXPANSIONEDITOR_API UDreamStoryFlowAssetFactory : public UDreamFlowAssetFactory
{
    GENERATED_BODY()

public:
    UDreamStoryFlowAssetFactory();

    virtual FText GetDisplayName() const override;
};

UCLASS()
class DREAMFLOWEXPANSIONEDITOR_API UDreamEncounterFlowAssetFactory : public UDreamFlowAssetFactory
{
    GENERATED_BODY()

public:
    UDreamEncounterFlowAssetFactory();

    virtual FText GetDisplayName() const override;
};
