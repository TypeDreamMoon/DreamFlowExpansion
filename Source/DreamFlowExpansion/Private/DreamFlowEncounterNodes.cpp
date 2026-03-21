#include "DreamFlowEncounterNodes.h"

#include "DreamEncounterFlowAsset.h"

UDreamFlowEncounterNode::UDreamFlowEncounterNode()
{
    SupportedFlowAssetType = UDreamEncounterFlowAsset::StaticClass();
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
}

FText UDreamFlowEncounterSetupNode::GetNodeDisplayName_Implementation() const
{
    return !SetupLabel.IsEmpty() ? SetupLabel : Title;
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
}

FText UDreamFlowEncounterWaveNode::GetNodeDisplayName_Implementation() const
{
    return !WaveLabel.IsEmpty() ? WaveLabel : Title;
}

FLinearColor UDreamFlowEncounterWaveNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.71f, 0.24f, 0.15f, 1.0f);
}

FText UDreamFlowEncounterWaveNode::GetNodeAccentLabel_Implementation() const
{
    return FText::Format(FText::FromString(TEXT("{0} Enemies")), ExpectedEnemyCount);
}

UDreamFlowEncounterResolveNode::UDreamFlowEncounterResolveNode()
{
    Title = FText::FromString(TEXT("Encounter Resolve"));
    Description = FText::FromString(TEXT("A terminal resolution node for victory, retreat, or aftermath handling."));
}

FText UDreamFlowEncounterResolveNode::GetNodeDisplayName_Implementation() const
{
    return !ResolutionLabel.IsEmpty() ? ResolutionLabel : Title;
}

FLinearColor UDreamFlowEncounterResolveNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.17f, 0.63f, 0.58f, 1.0f);
}

FText UDreamFlowEncounterResolveNode::GetNodeAccentLabel_Implementation() const
{
    return bVictory ? FText::FromString(TEXT("Victory")) : FText::FromString(TEXT("Fallback"));
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
