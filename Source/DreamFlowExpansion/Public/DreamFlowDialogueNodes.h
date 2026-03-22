#pragma once

#include "CoreMinimal.h"
#include "DreamFlowNode.h"
#include "DreamFlowVariableTypes.h"
#include "DreamFlowDialogueNodes.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowDialogueNode : public UDreamFlowNode
{
    GENERATED_BODY()

public:
    UDreamFlowDialogueNode();

    virtual FText GetNodeCategory_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowDialogueLineNode : public UDreamFlowDialogueNode
{
    GENERATED_BODY()

public:
    UDreamFlowDialogueLineNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding SpeakerNameBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (DreamFlowExpectedValueType = "Text", MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    FDreamFlowValueBinding LineTextBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (DreamFlowExpectedValueType = "Name", DreamFlowInlineEditable, DreamFlowInlinePriority = "40"))
    FDreamFlowValueBinding VoiceTagBinding;

    UPROPERTY()
    FText SpeakerName;

    UPROPERTY()
    FText LineText;

    UPROPERTY()
    FName VoiceTag;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual bool SupportsMultipleChildren_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowDialogueChoiceNode : public UDreamFlowDialogueNode
{
    GENERATED_BODY()

public:
    UDreamFlowDialogueChoiceNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (DreamFlowExpectedValueType = "Text", MultiLine = true, DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding PromptBinding;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (DreamFlowInlineEditable, DreamFlowInlinePriority = "30"))
    TArray<FText> ChoiceLabels;

    UPROPERTY()
    FText Prompt;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
};

UCLASS(BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class DREAMFLOWEXPANSION_API UDreamFlowDialogueEndNode : public UDreamFlowDialogueNode
{
    GENERATED_BODY()

public:
    UDreamFlowDialogueEndNode();
    virtual void PostLoad() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (DreamFlowExpectedValueType = "Text", DreamFlowInlineEditable, DreamFlowInlinePriority = "20"))
    FDreamFlowValueBinding EndLabelBinding;

    UPROPERTY()
    FText EndLabel;

    virtual FText GetNodeDisplayName_Implementation() const override;
    virtual FLinearColor GetNodeTint_Implementation() const override;
    virtual FText GetNodeAccentLabel_Implementation() const override;
    virtual bool SupportsMultipleChildren_Implementation() const override;
    virtual bool IsTerminalNode_Implementation() const override;
    virtual bool CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const override;
};
