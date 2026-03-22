#include "DreamFlowEncounterNodes.h"

#include "DreamEncounterFlowAsset.h"

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

UDreamFlowEncounterNode::UDreamFlowEncounterNode()
{
    SupportedFlowAssetType = UDreamEncounterFlowAsset::StaticClass();
    EncounterIdBinding.LiteralValue.Type = EDreamFlowValueType::Name;
}

void UDreamFlowEncounterNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultNameValue;
    DefaultNameValue.Type = EDreamFlowValueType::Name;
    DefaultNameValue.NameValue = NAME_None;

    if (IsDefaultLiteralBinding(EncounterIdBinding, DefaultNameValue) && !EncounterId.IsNone())
    {
        EncounterIdBinding.LiteralValue = DefaultNameValue;
        EncounterIdBinding.LiteralValue.NameValue = EncounterId;
    }
}

FText UDreamFlowEncounterNode::GetNodeCategory_Implementation() const
{
    return FText::FromString(TEXT("Encounter"));
}

FText UDreamFlowEncounterNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("Encounter"));
}

UDreamFlowEncounterSetupNode::UDreamFlowEncounterSetupNode()
{
    Title = FText::FromString(TEXT("Encounter Setup"));
    Description = FText::FromString(TEXT("Defines the framing, stakes, and entry conditions for a gameplay encounter."));
    SetupLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    ObjectiveBriefBinding.LiteralValue.Type = EDreamFlowValueType::Text;
}

void UDreamFlowEncounterSetupNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    if (IsDefaultLiteralBinding(SetupLabelBinding, DefaultTextValue) && !SetupLabel.IsEmpty())
    {
        SetupLabelBinding.LiteralValue = DefaultTextValue;
        SetupLabelBinding.LiteralValue.TextValue = SetupLabel;
    }

    if (IsDefaultLiteralBinding(ObjectiveBriefBinding, DefaultTextValue) && !ObjectiveBrief.IsEmpty())
    {
        ObjectiveBriefBinding.LiteralValue = DefaultTextValue;
        ObjectiveBriefBinding.LiteralValue.TextValue = ObjectiveBrief;
    }
}

FText UDreamFlowEncounterSetupNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(SetupLabelBinding) ? FText::FromString(DescribeBindingText(SetupLabelBinding)) : Title;
}

FLinearColor UDreamFlowEncounterSetupNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.12f, 0.41f, 0.77f, 1.0f);
}

FText UDreamFlowEncounterSetupNode::GetNodeAccentLabel_Implementation() const
{
    return FText::FromString(TEXT("Setup"));
}

UDreamFlowEncounterWaveNode::UDreamFlowEncounterWaveNode()
{
    Title = FText::FromString(TEXT("Encounter Wave"));
    Description = FText::FromString(TEXT("A playable combat or challenge phase inside the current encounter."));
    WaveLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    ExpectedEnemyCountBinding.LiteralValue.Type = EDreamFlowValueType::Int;
    ExpectedEnemyCountBinding.LiteralValue.IntValue = 3;
    DifficultyTagBinding.LiteralValue.Type = EDreamFlowValueType::Name;
}

void UDreamFlowEncounterWaveNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    FDreamFlowValue DefaultIntValue;
    DefaultIntValue.Type = EDreamFlowValueType::Int;
    DefaultIntValue.IntValue = 3;

    FDreamFlowValue DefaultNameValue;
    DefaultNameValue.Type = EDreamFlowValueType::Name;
    DefaultNameValue.NameValue = NAME_None;

    if (IsDefaultLiteralBinding(WaveLabelBinding, DefaultTextValue) && !WaveLabel.IsEmpty())
    {
        WaveLabelBinding.LiteralValue = DefaultTextValue;
        WaveLabelBinding.LiteralValue.TextValue = WaveLabel;
    }

    if (IsDefaultLiteralBinding(ExpectedEnemyCountBinding, DefaultIntValue))
    {
        ExpectedEnemyCountBinding.LiteralValue = DefaultIntValue;
        ExpectedEnemyCountBinding.LiteralValue.IntValue = ExpectedEnemyCount;
    }

    if (IsDefaultLiteralBinding(DifficultyTagBinding, DefaultNameValue) && !DifficultyTag.IsNone())
    {
        DifficultyTagBinding.LiteralValue = DefaultNameValue;
        DifficultyTagBinding.LiteralValue.NameValue = DifficultyTag;
    }
}

FText UDreamFlowEncounterWaveNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(WaveLabelBinding) ? FText::FromString(DescribeBindingText(WaveLabelBinding)) : Title;
}

FLinearColor UDreamFlowEncounterWaveNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.71f, 0.24f, 0.15f, 1.0f);
}

FText UDreamFlowEncounterWaveNode::GetNodeAccentLabel_Implementation() const
{
    if (ExpectedEnemyCountBinding.SourceType == EDreamFlowValueSourceType::Literal)
    {
        return FText::Format(FText::FromString(TEXT("{0} Enemies")), ExpectedEnemyCountBinding.LiteralValue.IntValue);
    }

    return FText::Format(FText::FromString(TEXT("{0} Enemies")), FText::FromString(DescribeBindingText(ExpectedEnemyCountBinding)));
}

UDreamFlowEncounterResolveNode::UDreamFlowEncounterResolveNode()
{
    Title = FText::FromString(TEXT("Encounter Resolve"));
    Description = FText::FromString(TEXT("A terminal resolution node for victory, retreat, or aftermath handling."));
    ResolutionLabelBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    RewardSummaryBinding.LiteralValue.Type = EDreamFlowValueType::Text;
    VictoryBinding.LiteralValue.Type = EDreamFlowValueType::Bool;
    VictoryBinding.LiteralValue.BoolValue = true;
}

void UDreamFlowEncounterResolveNode::PostLoad()
{
    Super::PostLoad();

    FDreamFlowValue DefaultTextValue;
    DefaultTextValue.Type = EDreamFlowValueType::Text;
    DefaultTextValue.TextValue = FText::GetEmpty();

    FDreamFlowValue DefaultBoolValue;
    DefaultBoolValue.Type = EDreamFlowValueType::Bool;
    DefaultBoolValue.BoolValue = true;

    if (IsDefaultLiteralBinding(ResolutionLabelBinding, DefaultTextValue) && !ResolutionLabel.IsEmpty())
    {
        ResolutionLabelBinding.LiteralValue = DefaultTextValue;
        ResolutionLabelBinding.LiteralValue.TextValue = ResolutionLabel;
    }

    if (IsDefaultLiteralBinding(RewardSummaryBinding, DefaultTextValue) && !RewardSummary.IsEmpty())
    {
        RewardSummaryBinding.LiteralValue = DefaultTextValue;
        RewardSummaryBinding.LiteralValue.TextValue = RewardSummary;
    }

    if (IsDefaultLiteralBinding(VictoryBinding, DefaultBoolValue))
    {
        VictoryBinding.LiteralValue = DefaultBoolValue;
        VictoryBinding.LiteralValue.BoolValue = bVictory;
    }
}

FText UDreamFlowEncounterResolveNode::GetNodeDisplayName_Implementation() const
{
    return IsBindingMeaningfullySet(ResolutionLabelBinding) ? FText::FromString(DescribeBindingText(ResolutionLabelBinding)) : Title;
}

FLinearColor UDreamFlowEncounterResolveNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.17f, 0.63f, 0.58f, 1.0f);
}

FText UDreamFlowEncounterResolveNode::GetNodeAccentLabel_Implementation() const
{
    if (VictoryBinding.SourceType == EDreamFlowValueSourceType::Literal)
    {
        return VictoryBinding.LiteralValue.BoolValue ? FText::FromString(TEXT("Victory")) : FText::FromString(TEXT("Fallback"));
    }

    return FText::FromString(DescribeBindingText(VictoryBinding));
}

bool UDreamFlowEncounterResolveNode::SupportsMultipleChildren_Implementation() const
{
    return false;
}

bool UDreamFlowEncounterResolveNode::IsTerminalNode_Implementation() const
{
    return true;
}

bool UDreamFlowEncounterResolveNode::CanConnectTo_Implementation(const UDreamFlowNode* OtherNode) const
{
    (void)OtherNode;
    return false;
}
