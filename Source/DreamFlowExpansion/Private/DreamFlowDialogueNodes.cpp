#include "DreamFlowDialogueNodes.h"

#include "DreamDialogueFlowAsset.h"

namespace
{
    static FString DescribeBindingText(const FDreamFlowValueBinding& Binding)
    {
        return Binding.Describe();
    }

    static bool IsBindingMeaningfullySet(const FDreamFlowValueBinding& Binding)
    {
        return Binding.SourceType == EDreamFlowValueSourceType::FlowVariable
            ? !Binding.VariableName.IsNone()
            : !Binding.Describe().IsEmpty();
    }

    static bool IsDefaultLiteralBinding(const FDreamFlowValueBinding& Binding, const FDreamFlowValue& DefaultValue)
    {
        if (Binding.SourceType != EDreamFlowValueSourceType::Literal || !Binding.VariableName.IsNone())
        {
            return false;
        }

        bool bEqual = false;
        return DreamFlowVariable::TryCompareValues(Binding.LiteralValue, DefaultValue, EDreamFlowComparisonOperation::Equal, bEqual) && bEqual;
    }
}

UDreamFlowDialogueNode::UDreamFlowDialogueNode()
{
    SupportedFlowAssetType = UDreamDialogueFlowAsset::StaticClass();
}

FText UDreamFlowDialogueNode::GetNodeCategory_Implementation() const
{
    return FText::FromString(TEXT("Dialogue"));
}

FText UDreamFlowDialogueNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("Dialogue"));
}

UDreamFlowDialogueLineNode::UDreamFlowDialogueLineNode()
{
    Title = FText::FromString(TEXT("Dialogue Line"));
    Description = FText::FromString(TEXT("A single spoken or narrated line."));
    SpeakerNameBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    LineTextBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    VoiceTagBinding.LiteralValue.Type = EDreamFlowValueType::Name;
}

void UDreamFlowDialogueLineNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    FDreamFlowValue DefaultNameValue;
    DefaultNameValue.Type = EDreamFlowValueType::Name;
    DefaultNameValue.NameValue = NAME_None;

    if (IsDefaultLiteralBinding(SpeakerNameBinding, DefaultTextValue) && !SpeakerName.IsEmpty())
    {
        SpeakerNameBinding.LiteralValue = DefaultTextValue;
        SpeakerNameBinding.LiteralValue.TextValue = SpeakerName;
    }

    if (IsDefaultLiteralBinding(LineTextBinding, DefaultTextValue) && !LineText.IsEmpty())
    {
        LineTextBinding.LiteralValue = DefaultTextValue;
        LineTextBinding.LiteralValue.TextValue = LineText;
    }

    if (IsDefaultLiteralBinding(VoiceTagBinding, DefaultNameValue) && !VoiceTag.IsNone())
    {
        VoiceTagBinding.LiteralValue = DefaultNameValue;
        VoiceTagBinding.LiteralValue.NameValue = VoiceTag;
    }
}

FText UDreamFlowDialogueLineNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(SpeakerNameBinding)
        ? FText::Format(FText::FromString(TEXT("{0} Line")), FText::FromString(DescribeBindingText(SpeakerNameBinding)))
        : Title;
}

FLinearColor UDreamFlowDialogueLineNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.26f, 0.44f, 0.85f, 1.0f);
}

bool UDreamFlowDialogueLineNode::SupportsMultipleChildren_Implementation() const
{
    return false;
}

UDreamFlowDialogueChoiceNode::UDreamFlowDialogueChoiceNode()
{
    Title = FText::FromString(TEXT("Dialogue Choice"));
    Description = FText::FromString(TEXT("A branching point that offers the player multiple dialogue responses."));
    PromptBinding.LiteralValue.Type = EDreamFlowValueType::Text;
}

void UDreamFlowDialogueChoiceNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    if (IsDefaultLiteralBinding(PromptBinding, DefaultTextValue) && !Prompt.IsEmpty())
    {
        PromptBinding.LiteralValue = DefaultTextValue;
        PromptBinding.LiteralValue.TextValue = Prompt;
    }
}

FText UDreamFlowDialogueChoiceNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(PromptBinding) ? FText::FromString(DescribeBindingText(PromptBinding)) : Title;
}

FLinearColor UDreamFlowDialogueChoiceNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.58f, 0.30f, 0.82f, 1.0f);
}

FText UDreamFlowDialogueChoiceNode::GetNodeAccentLabel_Implementation() const
{
    return ChoiceLabels.Num() > 0
        ? FText::Format(FText::FromString(TEXT("{0} Choices")), ChoiceLabels.Num())
        : FText::FromString(TEXT("Branch"));
}

UDreamFlowDialogueEndNode::UDreamFlowDialogueEndNode()
{
    Title = FText::FromString(TEXT("Dialogue End"));
    Description = FText::FromString(TEXT("A terminal node that closes the current conversation."));
    EndLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
}

void UDreamFlowDialogueEndNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    if (IsDefaultLiteralBinding(EndLabelBinding, DefaultTextValue) && !EndLabel.IsEmpty())
    {
        EndLabelBinding.LiteralValue = DefaultTextValue;
        EndLabelBinding.LiteralValue.TextValue = EndLabel;
    }
}

FText UDreamFlowDialogueEndNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(EndLabelBinding) ? FText::FromString(DescribeBindingText(EndLabelBinding)) : Title;
}

FLinearColor UDreamFlowDialogueEndNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.91f, 0.24f, 0.33f, 1.0f);
}

FText UDreamFlowDialogueEndNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("End"));
}

bool UDreamFlowDialogueEndNode::SupportsMultipleChildren_Implementation() const
{
    return false;
}

bool UDreamFlowDialogueEndNode::IsTerminalNode_Implementation() const
{
    return true;
}

bool UDreamFlowDialogueEndNode::CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const
{
    (void)OtherNode;
    return false;
}
