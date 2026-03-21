#pragma once

#include "CoreMinimal.h"
#include "DreamFlowNode.h"
#include "DreamFlowEncounterNodes.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowEncounterNode : public UDreamFlowNode
{
    GENERATED_BODY()

public:
    UDreamFlowEncounterNode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FName EncounterId;

    virtual FText GetNodeCategory_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowEncounterSetupNode : public UDreamFlowEncounterNode
{
    GENERATED_BODY()

public:
    UDreamFlowEncounterSetupNode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FText SetupLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FText ObjectiveBrief;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowEncounterWaveNode : public UDreamFlowEncounterNode
{
    GENERATED_BODY()

public:
    UDreamFlowEncounterWaveNode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FText WaveLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "30", ClampMin = "1"))
    int32 ExpectedEnemyCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    FName DifficultyTag;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowEncounterResolveNode : public UDreamFlowEncounterNode
{
    GENERATED_BODY()

public:
    UDreamFlowEncounterResolveNode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FText ResolutionLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FText RewardSummary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    bool bVictory = true;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
    virtual bool SupportsMultipleChildren_Implementation() const override;
    virtual bool IsTerminalNode_Implementation() const override;
    virtual bool CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const override;
};
