#include "DreamFlowQuestNodes.h"

#include "DreamQuestFlowAsset.h"

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

UDreamFlowQuestNode::UDreamFlowQuestNode()
{
    SupportedFlowAssetType = UDreamQuestFlowAsset::StaticClass();
    QuestIdBinding.LiteralValue.Type = EDreamFlowValueType::Name;
}

void UDreamFlowQuestNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultNameValue;
    DefaultNameValue.Type = EDreamFlowValueType::Name;
    DefaultNameValue.NameValue = NAME_None;

    if (IsDefaultLiteralBinding(QuestIdBinding, DefaultNameValue) && !QuestId.IsNone())
    {
        QuestIdBinding.LiteralValue = DefaultNameValue;
        QuestIdBinding.LiteralValue.NameValue = QuestId;
    }
}

FText UDreamFlowQuestNode::GetNodeCategory_Implementation() const
{
    return FText::FromString(TEXT("Quest"));
}

FText UDreamFlowQuestNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("Quest"));
}

UDreamFlowQuestTaskNode::UDreamFlowQuestTaskNode()
{
    Title = FText::FromString(TEXT("Quest Task"));
    Description = FText::FromString(TEXT("A concrete quest step or objective."));
    TaskLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    OptionalBinding.LiteralValue.Type = EDreamFlowValueType::Bool;
    OptionalBinding.LiteralValue.BoolValue = false;
}

void UDreamFlowQuestTaskNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    FDreamFlowValue DefaultBoolValue;
    DefaultBoolValue.Type = EDreamFlowValueType::Bool;
    DefaultBoolValue.BoolValue = false;

    if (IsDefaultLiteralBinding(TaskLabelBinding, DefaultTextValue) && !TaskLabel.IsEmpty())
    {
        TaskLabelBinding.LiteralValue = DefaultTextValue;
        TaskLabelBinding.LiteralValue.TextValue = TaskLabel;
    }

    if (IsDefaultLiteralBinding(OptionalBinding, DefaultBoolValue))
    {
        OptionalBinding.LiteralValue = DefaultBoolValue;
        OptionalBinding.LiteralValue.BoolValue = bOptional;
    }
}

FText UDreamFlowQuestTaskNode::GetNodeDisplayName_Implementation() const
{
    if (IsBindingMeaningfullySet(TaskLabelBinding))
    {
        return FText::FromString(DescribeBindingText(TaskLabelBinding));
    }

    if (IsBindingMeaningfullySet(QuestIdBinding))
    {
        return FText::FromString(DescribeBindingText(QuestIdBinding));
    }

    return Title;
}

FLinearColor UDreamFlowQuestTaskNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.13f, 0.58f, 0.42f, 1.0f);
}

FText UDreamFlowQuestTaskNode::GetNodeAccentLabel_Implementation() const
{
    if (OptionalBinding.SourceType == EDreamFlowValueSourceType::Literal)
    {
        return OptionalBinding.LiteralValue.BoolValue ? FText::FromString(TEXT("Optional")) : FText::FromString(TEXT("Required"));
    }

    return FText::FromString(DescribeBindingText(OptionalBinding));
}

UDreamFlowQuestConditionNode::UDreamFlowQuestConditionNode()
{
    Title = FText::FromString(TEXT("Quest Condition"));
    Description = FText::FromString(TEXT("A branch point driven by quest state or gameplay checks."));
    ConditionLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    ExpectedResultBinding.LiteralValue.Type = EDreamFlowValueType::Bool;
    ExpectedResultBinding.LiteralValue.BoolValue = true;
    FailureHintBinding.LiteralValue.Type = EDreamFlowValueType::Text;
}

void UDreamFlowQuestConditionNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    FDreamFlowValue DefaultBoolValue;
    DefaultBoolValue.Type = EDreamFlowValueType::Bool;
    DefaultBoolValue.BoolValue = true;

    if (IsDefaultLiteralBinding(ConditionLabelBinding, DefaultTextValue) && !ConditionLabel.IsEmpty())
    {
        ConditionLabelBinding.LiteralValue = DefaultTextValue;
        ConditionLabelBinding.LiteralValue.TextValue = ConditionLabel;
    }

    if (IsDefaultLiteralBinding(ExpectedResultBinding, DefaultBoolValue))
    {
        ExpectedResultBinding.LiteralValue = DefaultBoolValue;
        ExpectedResultBinding.LiteralValue.BoolValue = bExpectedResult;
    }

    if (IsDefaultLiteralBinding(FailureHintBinding, DefaultTextValue) && !FailureHint.IsEmpty())
    {
        FailureHintBinding.LiteralValue = DefaultTextValue;
        FailureHintBinding.LiteralValue.TextValue = FailureHint;
    }
}

FText UDreamFlowQuestConditionNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(ConditionLabelBinding) ? FText::FromString(DescribeBindingText(ConditionLabelBinding)) : Title;
}

FLinearColor UDreamFlowQuestConditionNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.84f, 0.53f, 0.16f, 1.0f);
}

FText UDreamFlowQuestConditionNode::GetNodeAccentLabel_Implementation() const
{
    if (ExpectedResultBinding.SourceType == EDreamFlowValueSourceType::Literal)
    {
        return ExpectedResultBinding.LiteralValue.BoolValue ? FText::FromString(TEXT("True")) : FText::FromString(TEXT("False"));
    }

    return FText::FromString(DescribeBindingText(ExpectedResultBinding));
}

UDreamFlowQuestCompleteNode::UDreamFlowQuestCompleteNode()
{
    Title = FText::FromString(TEXT("Quest Complete"));
    Description = FText::FromString(TEXT("A terminal node representing quest completion or reward payout."));
    CompletionLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    RewardPreviewBinding.LiteralValue.Type = EDreamFlowValueType::Text;
}

void UDreamFlowQuestCompleteNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    if (IsDefaultLiteralBinding(CompletionLabelBinding, DefaultTextValue) && !CompletionLabel.IsEmpty())
    {
        CompletionLabelBinding.LiteralValue = DefaultTextValue;
        CompletionLabelBinding.LiteralValue.TextValue = CompletionLabel;
    }

    if (IsDefaultLiteralBinding(RewardPreviewBinding, DefaultTextValue) && !RewardPreview.IsEmpty())
    {
        RewardPreviewBinding.LiteralValue = DefaultTextValue;
        RewardPreviewBinding.LiteralValue.TextValue = RewardPreview;
    }
}

FText UDreamFlowQuestCompleteNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(CompletionLabelBinding) ? FText::FromString(DescribeBindingText(CompletionLabelBinding)) : Title;
}

FLinearColor UDreamFlowQuestCompleteNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.08f, 0.69f, 0.68f, 1.0f);
}

FText UDreamFlowQuestCompleteNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("Complete"));
}

bool UDreamFlowQuestCompleteNode::SupportsMultipleChildren_Implementation() const
{
    return false;
}

bool UDreamFlowQuestCompleteNode::IsTerminalNode_Implementation() const
{
    return true;
}

bool UDreamFlowQuestCompleteNode::CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const
{
    (void)OtherNode;
    return false;
}
