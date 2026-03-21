#include "DreamFlowStoryNodes.h"

#include "DreamStoryFlowAsset.h"

namespace
{
    static FName MakeStoryPinName(const int32 Index)
    {
        return FName(*FString::Printf(TEXT("Choice_%d"), Index));
    }
}

UDreamFlowStoryNode::UDreamFlowStoryNode()
{
    SupportedFlowAssetType = UDreamStoryFlowAsset::StaticClass();
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
}

FText UDreamFlowStoryBeatNode::GetNodeDisplayName_Implementation() const
{
    return !BeatLabel.IsEmpty() ? BeatLabel : Title;
}

FLinearColor UDreamFlowStoryBeatNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.78f, 0.26f, 0.44f, 1.0f);
}

FText UDreamFlowStoryBeatNode::GetNodeAccentLabel_Implementation() const
{
    return bCriticalBeat ? FText::FromString(TEXT("Critical")) : FText::FromString(TEXT("Beat"));
}

UDreamFlowStoryBranchNode::UDreamFlowStoryBranchNode()
{
    Title = FText::FromString(TEXT("Story Branch"));
    Description = FText::FromString(TEXT("A narrative decision point with authored branches."));
}

FText UDreamFlowStoryBranchNode::GetNodeDisplayName_Implementation() const
{
    return !DecisionPrompt.IsEmpty() ? DecisionPrompt : Title;
}

FLinearColor UDreamFlowStoryBranchNode::GetNodeTint_Implementation() const
{
    return FLinearColor(0.91f, 0.56f, 0.18f, 1.0f);
}

FText UDreamFlowStoryBranchNode::GetNodeAccentLabel_Implementation() const
{
    return bMajorDecision ? FText::FromString(TEXT("Major")) : FText::FromString(TEXT("Minor"));
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
}

FText UDreamFlowStoryEndingNode::GetNodeDisplayName_Implementation() const
{
    return !EndingLabel.IsEmpty() ? EndingLabel : Title;
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
