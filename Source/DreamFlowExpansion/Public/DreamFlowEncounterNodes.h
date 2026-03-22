#pragma once

#include "CoreMinimal.h"
#include "DreamFlowNode.h"
#include "DreamFlowVariableTypes.h"
#include "DreamFlowEncounterNodes.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowEncounterNode : public UDreamFlowNode
{
    GENERATED_BODY()

public:
    UDreamFlowEncounterNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Name", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding EncounterIdBinding;

    UPROPERTY()
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
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding SetupLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Text", MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding ObjectiveBriefBinding;

    UPROPERTY()
    FText SetupLabel;

    UPROPERTY()
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
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding WaveLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Int", DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding ExpectedEnemyCountBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Name", DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    FDreamFlowValueBinding DifficultyTagBinding;

    UPROPERTY()
    FText WaveLabel;

    UPROPERTY()
    int32 ExpectedEnemyCount = 3;

    UPROPERTY()
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
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding ResolutionLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Text", MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding RewardSummaryBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Encounter", meta = (DreamFlowExpectedValueType = "Bool", DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    FDreamFlowValueBinding VictoryBinding;

    UPROPERTY()
    FText ResolutionLabel;

    UPROPERTY()
    FText RewardSummary;

    UPROPERTY()
    bool bVictory = true;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
    virtual bool SupportsMultipleChildren_Implementation() const override;
    virtual bool IsTerminalNode_Implementation() const override;
    virtual bool CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const override;
};
