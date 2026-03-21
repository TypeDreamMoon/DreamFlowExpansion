#pragma once

#include "CoreMinimal.h"
#include "DreamFlowNode.h"
#include "DreamFlowStoryNodes.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowStoryNode : public UDreamFlowNode
{
    GENERATED_BODY()

public:
    UDreamFlowStoryNode();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FText BeatLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FText BeatSummary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FText DecisionPrompt;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    TArray<FText> BranchLabels;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FText EndingLabel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Story", meta = (MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FText OutcomeSummary;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
    virtual bool SupportsMultipleChildren_Implementation() const override;
    virtual bool IsTerminalNode_Implementation() const override;
    virtual bool CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const override;
};
