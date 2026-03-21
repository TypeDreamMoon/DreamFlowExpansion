#include "DreamFlowExpansionAssetFactory.h"

#include "DreamDialogueFlowAsset.h"
#include "DreamEncounterFlowAsset.h"
#include "DreamQuestFlowAsset.h"
#include "DreamStoryFlowAsset.h"

#define LOCTEXT_NAMESPACE "DreamFlowExpansionAssetFactory"

UDreamQuestFlowAssetFactory::UDreamQuestFlowAssetFactory()
{
    SupportedClass = UDreamQuestFlowAsset::StaticClass();
}

FText UDreamQuestFlowAssetFactory::GetDisplayName() const
{
    return LOCTEXT("DreamQuestFlowAssetDisplayName", "Dream Quest Flow");
}

UDreamDialogueFlowAssetFactory::UDreamDialogueFlowAssetFactory()
{
    SupportedClass = UDreamDialogueFlowAsset::StaticClass();
}

FText UDreamDialogueFlowAssetFactory::GetDisplayName() const
{
    return LOCTEXT("DreamDialogueFlowAssetDisplayName", "Dream Dialogue Flow");
}

UDreamStoryFlowAssetFactory::UDreamStoryFlowAssetFactory()
{
    SupportedClass = UDreamStoryFlowAsset::StaticClass();
}

FText UDreamStoryFlowAssetFactory::GetDisplayName() const
{
    return LOCTEXT("DreamStoryFlowAssetDisplayName", "Dream Story Flow");
}

UDreamEncounterFlowAssetFactory::UDreamEncounterFlowAssetFactory()
{
    SupportedClass = UDreamEncounterFlowAsset::StaticClass();
}

FText UDreamEncounterFlowAssetFactory::GetDisplayName() const
{
    return LOCTEXT("DreamEncounterFlowAssetDisplayName", "Dream Encounter Flow");
}

#undef LOCTEXT_NAMESPACE
