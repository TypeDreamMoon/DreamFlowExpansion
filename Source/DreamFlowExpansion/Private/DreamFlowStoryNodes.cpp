#include "DreamFlowStoryNodes.h"

#include "DreamStoryFlowAsset.h"

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

    static FName MakeStoryPinName(const int32 Index)
    {
        return FName(*FString::Printf(TEXT("Choice_%d"), Index));
    }
}

UDreamFlowStoryNode::UDreamFlowStoryNode()
{
    SupportedFlowAssetType = UDreamStoryFlowAsset::StaticClass();
    StoryArcBinding.LiteralValue.Type = EDreamFlowValueType::Name;
}

void UDreamFlowStoryNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultNameValue;
    DefaultNameValue.Type = EDreamFlowValueType::Name;
    DefaultNameValue.NameValue = NAME_None;

    if (IsDefaultLiteralBinding(StoryArcBinding, DefaultNameValue) && !StoryArcId.IsNone())
    {
        StoryArcBinding.LiteralValue = DefaultNameValue;
        StoryArcBinding.LiteralValue.NameValue = StoryArcId;
    }
}

FText UDreamFlowStoryNode::GetNodeCategory_Implementation() const
{
    return FText::FromString(TEXT("Story"));
}

FText UDreamFlowStoryNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("Story"));
}

UDreamFlowStoryBeatNode::UDreamFlowStoryBeatNode()
{
    Title = FText::FromString(TEXT("Story Beat"));
    Description = FText::FromString(TEXT("A key narrative beat that advances pacing, reveals, or character state."));
    BeatLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    BeatSummaryBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    CriticalBeatBinding.LiteralValue.Type = EDreamFlowValueType::Bool;
    CriticalBeatBinding.LiteralValue.BoolValue = false;
}

void UDreamFlowStoryBeatNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    FDreamFlowValue DefaultBoolValue;
    DefaultBoolValue.Type = EDreamFlowValueType::Bool;
    DefaultBoolValue.BoolValue = false;

    if (IsDefaultLiteralBinding(BeatLabelBinding, DefaultTextValue) && !BeatLabel.IsEmpty())
    {
        BeatLabelBinding.LiteralValue = DefaultTextValue;
        BeatLabelBinding.LiteralValue.TextValue = BeatLabel;
    }

    if (IsDefaultLiteralBinding(BeatSummaryBinding, DefaultTextValue) && !BeatSummary.IsEmpty())
    {
        BeatSummaryBinding.LiteralValue = DefaultTextValue;
        BeatSummaryBinding.LiteralValue.TextValue = BeatSummary;
    }

    if (IsDefaultLiteralBinding(CriticalBeatBinding, DefaultBoolValue))
    {
        CriticalBeatBinding.LiteralValue = DefaultBoolValue;
        CriticalBeatBinding.LiteralValue.BoolValue = bCriticalBeat;
    }
}

FText UDreamFlowStoryBeatNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(BeatLabelBinding) ? FText::FromString(DescribeBindingText(BeatLabelBinding)) : Title;
}

FLinearColor UDreamFlowStoryBeatNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.78f, 0.26f, 0.44f, 1.0f);
}

FText UDreamFlowStoryBeatNode::GetNodeAccentLabel_Implementation() const
{
    if (CriticalBeatBinding.SourceType == EDreamFlowValueSourceType::Literal)
    {
        return CriticalBeatBinding.LiteralValue.BoolValue ? FText::FromString(TEXT("Critical")) : FText::FromString(TEXT("Beat"));
    }

    return FText::FromString(DescribeBindingText(CriticalBeatBinding));
}

UDreamFlowStoryBranchNode::UDreamFlowStoryBranchNode()
{
    Title = FText::FromString(TEXT("Story Branch"));
    Description = FText::FromString(TEXT("A narrative decision point with authored branches."));
    DecisionPromptBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    MajorDecisionBinding.LiteralValue.Type = EDreamFlowValueType::Bool;
    MajorDecisionBinding.LiteralValue.BoolValue = true;
}

void UDreamFlowStoryBranchNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    FDreamFlowValue DefaultBoolValue;
    DefaultBoolValue.Type = EDreamFlowValueType::Bool;
    DefaultBoolValue.BoolValue = true;

    if (IsDefaultLiteralBinding(DecisionPromptBinding, DefaultTextValue) && !DecisionPrompt.IsEmpty())
    {
        DecisionPromptBinding.LiteralValue = DefaultTextValue;
        DecisionPromptBinding.LiteralValue.TextValue = DecisionPrompt;
    }

    if (IsDefaultLiteralBinding(MajorDecisionBinding, DefaultBoolValue))
    {
        MajorDecisionBinding.LiteralValue = DefaultBoolValue;
        MajorDecisionBinding.LiteralValue.BoolValue = bMajorDecision;
    }
}

FText UDreamFlowStoryBranchNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(DecisionPromptBinding) ? FText::FromString(DescribeBindingText(DecisionPromptBinding)) : Title;
}

FLinearColor UDreamFlowStoryBranchNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.91f, 0.56f, 0.18f, 1.0f);
}

FText UDreamFlowStoryBranchNode::GetNodeAccentLabel_Implementation() const
{
    if (MajorDecisionBinding.SourceType == EDreamFlowValueSourceType::Literal)
    {
        return MajorDecisionBinding.LiteralValue.BoolValue ? FText::FromString(TEXT("Major")) : FText::FromString(TEXT("Minor"));
    }

    return FText::FromString(DescribeBindingText(MajorDecisionBinding));
}

TArray<FDreamFlowNodeOutputPin> UDreamFlowStoryBranchNode::GetOutputPins_Implementation() const
{
    TArray<FDreamFlowNodeOutputPin> OutputPins;

    if (BranchLabels.Num() == 0)
    {
        FDreamFlowNodeOutputPin& ContinuePin = OutputPins.AddDefaulted_GetRef();
        ContinuePin.PinName = MakeStoryPinName(0);
        ContinuePin.DisplayName = FText::FromString(TEXT("Continue"));

        FDreamFlowNodeOutputPin& DetourPin = OutputPins.AddDefaulted_GetRef();
        DetourPin.PinName = MakeStoryPinName(1);
        DetourPin.DisplayName = FText::FromString(TEXT("Detour"));
        return OutputPins;
    }

    for (int32 Index = 0; Index < BranchLabels.Num(); ++Index)
    {
        FDreamFlowNodeOutputPin& OutputPin = OutputPins.AddDefaulted_GetRef();
        OutputPin.PinName = MakeStoryPinName(Index);
        OutputPin.DisplayName = BranchLabels[Index].IsEmpty()
            ? FText::Format(FText::FromString(TEXT("Choice {0}")), Index + 1)
            : BranchLabels[Index];
    }

    return OutputPins;
}

UDreamFlowStoryEndingNode::UDreamFlowStoryEndingNode()
{
    Title = FText::FromString(TEXT("Story Ending"));
    Description = FText::FromString(TEXT("A terminal beat that closes the current story thread or chapter."));
    EndingLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    OutcomeSummaryBinding.LiteralValue.Type = EDreamFlowValueType::Text;
}

void UDreamFlowStoryEndingNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    if (IsDefaultLiteralBinding(EndingLabelBinding, DefaultTextValue) && !EndingLabel.IsEmpty())
    {
        EndingLabelBinding.LiteralValue = DefaultTextValue;
        EndingLabelBinding.LiteralValue.TextValue = EndingLabel;
    }

    if (IsDefaultLiteralBinding(OutcomeSummaryBinding, DefaultTextValue) && !OutcomeSummary.IsEmpty())
    {
        OutcomeSummaryBinding.LiteralValue = DefaultTextValue;
        OutcomeSummaryBinding.LiteralValue.TextValue = OutcomeSummary;
    }
}

FText UDreamFlowStoryEndingNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(EndingLabelBinding) ? FText::FromString(DescribeBindingText(EndingLabelBinding)) : Title;
}

FLinearColor UDreamFlowStoryEndingNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.29f, 0.18f, 0.58f, 1.0f);
}

FText UDreamFlowStoryEndingNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("Ending"));
}

bool UDreamFlowStoryEndingNode::SupportsMultipleChildren_Implementation() const
{
    return false;
}

bool UDreamFlowStoryEndingNode::IsTerminalNode_Implementation() const
{
    return true;
}

bool UDreamFlowStoryEndingNode::CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const
{
    (void)OtherNode;
    return false;
}
