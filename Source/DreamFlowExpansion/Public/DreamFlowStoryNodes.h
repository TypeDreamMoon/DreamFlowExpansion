#pragma once

#include "CoreMinimal.h"
#include "DreamFlowNode.h"
#include "DreamFlowVariableTypes.h"
#include "DreamFlowStoryNodes.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowStoryNode : public UDreamFlowNode
{
    GENERATED_BODY()

public:
    UDreamFlowStoryNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Name", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding StoryArcBinding;

    UPROPERTY()
    FName StoryArcId;

    virtual FText GetNodeCategory_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowStoryBeatNode : public UDreamFlowStoryNode
{
    GENERATED_BODY()

public:
    UDreamFlowStoryBeatNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding BeatLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Text", MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding BeatSummaryBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Bool", DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    FDreamFlowValueBinding CriticalBeatBinding;

    UPROPERTY()
    FText BeatLabel;

    UPROPERTY()
    FText BeatSummary;

    UPROPERTY()
    bool bCriticalBeat = false;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowStoryBranchNode : public UDreamFlowStoryNode
{
    GENERATED_BODY()

public:
    UDreamFlowStoryBranchNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Text", MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding DecisionPromptBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    TArray<FText> BranchLabels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Bool", DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    FDreamFlowValueBinding MajorDecisionBinding;

    UPROPERTY()
    FText DecisionPrompt;

    UPROPERTY()
    bool bMajorDecision = true;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
    virtual TArray<FDreamFlowNodeOutputPin> GetOutputPins_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowStoryEndingNode : public UDreamFlowStoryNode
{
    GENERATED_BODY()

public:
    UDreamFlowStoryEndingNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding EndingLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowExpectedValueType = "Text", MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding OutcomeSummaryBinding;

    UPROPERTY()
    FText EndingLabel;

    UPROPERTY()
    FText OutcomeSummary;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
    virtual bool SupportsMultipleChildren_Implementation() const override;
    virtual bool IsTerminalNode_Implementation() const override;
    virtual bool CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const override;
};
