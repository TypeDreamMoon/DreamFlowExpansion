#pragma once

#include "CoreMinimal.h"
#include "DreamFlowNode.h"
#include "DreamFlowVariableTypes.h"
#include "DreamFlowQuestNodes.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowQuestNode : public UDreamFlowNode
{
    GENERATED_BODY()

public:
    UDreamFlowQuestNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Name", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding QuestIdBinding;

    UPROPERTY()
    FName QuestId;

    virtual FText GetNodeCategory_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowQuestTaskNode : public UDreamFlowQuestNode
{
    GENERATED_BODY()

public:
    UDreamFlowQuestTaskNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding TaskLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Bool", DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding OptionalBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    TArray<FText> Objectives;

    UPROPERTY()
    FText TaskLabel;

    UPROPERTY()
    bool bOptional = false;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowQuestConditionNode : public UDreamFlowQuestNode
{
    GENERATED_BODY()

public:
    UDreamFlowQuestConditionNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding ConditionLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Bool", DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding ExpectedResultBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    FDreamFlowValueBinding FailureHintBinding;

    UPROPERTY()
    FText ConditionLabel;

    UPROPERTY()
    bool bExpectedResult = true;

    UPROPERTY()
    FText FailureHint;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowQuestCompleteNode : public UDreamFlowQuestNode
{
    GENERATED_BODY()

public:
    UDreamFlowQuestCompleteNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding CompletionLabelBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding RewardPreviewBinding;

    UPROPERTY()
    FText CompletionLabel;

    UPROPERTY()
    FText RewardPreview;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
    virtual bool SupportsMultipleChildren_Implementation() const override;
    virtual bool IsTerminalNode_Implementation() const override;
    virtual bool CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const override;
};
