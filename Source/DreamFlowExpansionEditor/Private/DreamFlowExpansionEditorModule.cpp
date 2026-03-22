#include "DreamFlowExpansionEditorModule.h"

#include "AssetTypeActions_Base.h"
#include "DreamDialogueFlowAsset.h"
#include "DreamEncounterFlowAsset.h"
#include "DreamFlowAsset.h"
#include "DreamFlowDialogueNodes.h"
#include "DreamFlowEdGraph.h"
#include "DreamFlowEdGraphNode.h"
#include "DreamFlowEditorToolkit.h"
#include "DreamFlowEditorUtils.h"
#include "DreamFlowStoryNodes.h"
#include "DreamQuestFlowAsset.h"
#include "DreamStoryFlowAsset.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "ContentBrowserDataMenuContexts.h"
#include "ContentBrowserDataSubsystem.h"
#include "DesktopPlatformModule.h"
#include "EdGraph/EdGraphPin.h"
#include "Framework/Commands/UIAction.h"
#include "HAL/FileManager.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "IContentBrowserDataModule.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "Serialization/Csv/CsvParser.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "DreamFlowExpansionEditorModule"

namespace
{
    class FDreamFlowExpansionAssetTypeActions final : public FAssetTypeActions_Base
    {
    public:
        FDreamFlowExpansionAssetTypeActions(uint32 InAssetCategory, UClass* InSupportedClass, FText InDisplayName, FColor InTypeColor)
            : AssetCategory(InAssetCategory)
            , SupportedClass(InSupportedClass)
            , DisplayName(MoveTemp(InDisplayName))
            , TypeColor(InTypeColor)
        {
        }

        virtual FText GetName() const override
        {
            return DisplayName;
        }

        virtual FColor GetTypeColor() const override
        {
            return TypeColor;
        }

        virtual UClass* GetSupportedClass() const override
        {
            return SupportedClass;
        }

        virtual uint32 GetCategories() override
        {
            return AssetCategory;
        }

        virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override
        {
            const EToolkitMode::Type ToolkitMode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

            for (UObject* Object : InObjects)
            {
                if (UDreamFlowAsset* FlowAsset = Cast<UDreamFlowAsset>(Object))
                {
                    TSharedRef<FDreamFlowEditorToolkit> EditorToolkit = MakeShared<FDreamFlowEditorToolkit>();
                    EditorToolkit->InitEditor(ToolkitMode, EditWithinLevelEditor, FlowAsset);
                }
            }
        }

    private:
        uint32 AssetCategory = 0;
        UClass* SupportedClass = nullptr;
        FText DisplayName;
        FColor TypeColor = FColor::White;
    };

    static FString ResolveTargetPath(const UContentBrowserDataMenuContext_AddNewMenu* Context)
    {
        if (Context != nullptr)
        {
            UContentBrowserDataSubsystem* ContentBrowserData = IContentBrowserDataModule::Get().GetSubsystem();
            if (ContentBrowserData != nullptr)
            {
                for (const FName SelectedPath : Context->SelectedPaths)
                {
                    FName InternalPath;
                    if (ContentBrowserData->TryConvertVirtualPath(SelectedPath, InternalPath) == EContentBrowserPathType::Internal)
                    {
                        return InternalPath.ToString();
                    }
                }
            }
        }

        return FDreamFlowEditorUtils::GetCurrentContentBrowserPath();
    }

    static FString SanitizeCsvCell(const FString& RawValue)
    {
        FString Value = RawValue;
        Value.TrimStartAndEndInline();
        Value.ReplaceInline(TEXT("\ufeff"), TEXT(""));

        if (Value.Len() >= 2 && Value.StartsWith(TEXT("\"")) && Value.EndsWith(TEXT("\"")))
        {
            Value = Value.Mid(1, Value.Len() - 2);
        }

        Value.ReplaceInline(TEXT("\"\""), TEXT("\""));
        return Value;
    }

    static FString NormalizeCsvKey(const FString& RawHeader)
    {
        FString Header = SanitizeCsvCell(RawHeader);
        Header.TrimStartAndEndInline();
        Header.ToLowerInline();
        return Header;
    }

    static FString GetCsvValue(const TMap<FString, FString>& Row, const FString& Key)
    {
        const FString* FoundValue = Row.Find(Key.ToLower());
        return FoundValue != nullptr ? *FoundValue : FString();
    }

    static TArray<FString> SplitPipeList(const FString& RawValue)
    {
        TArray<FString> Result;
        TArray<FString> Parts;
        RawValue.ParseIntoArray(Parts, TEXT("|"), true);

        for (FString& Part : Parts)
        {
            Part = SanitizeCsvCell(Part);
            if (!Part.IsEmpty())
            {
                Result.Add(Part);
            }
        }

        return Result;
    }

    static FDreamFlowValueBinding MakeTextLiteralBinding(const FString& InValue)
    {
        FDreamFlowValueBinding Binding;
        Binding.SourceType = EDreamFlowValueSourceType::Literal;
        Binding.LiteralValue.Type = EDreamFlowValueType::Text;
        Binding.LiteralValue.TextValue = FText::FromString(InValue);
        return Binding;
    }

    static FDreamFlowValueBinding MakeNameLiteralBinding(const FString& InValue)
    {
        FDreamFlowValueBinding Binding;
        Binding.SourceType = EDreamFlowValueSourceType::Literal;
        Binding.LiteralValue.Type = EDreamFlowValueType::Name;
        Binding.LiteralValue.NameValue = FName(*InValue);
        return Binding;
    }

    static FDreamFlowValueBinding MakeBoolLiteralBinding(const bool bInValue)
    {
        FDreamFlowValueBinding Binding;
        Binding.SourceType = EDreamFlowValueSourceType::Literal;
        Binding.LiteralValue.Type = EDreamFlowValueType::Bool;
        Binding.LiteralValue.BoolValue = bInValue;
        return Binding;
    }

    static bool ParseBool(const FString& RawValue, const bool bDefaultValue = false)
    {
        if (RawValue.IsEmpty())
        {
            return bDefaultValue;
        }

        const FString LowerValue = RawValue.ToLower();
        return LowerValue == TEXT("1")
            || LowerValue == TEXT("true")
            || LowerValue == TEXT("yes")
            || LowerValue == TEXT("y");
    }

    static bool LoadCsvRows(const FString& FilePath, TArray<TMap<FString, FString>>& OutRows, FString& OutError)
    {
        OutRows.Reset();

        FString CsvText;
        if (!FFileHelper::LoadFileToString(CsvText, *FilePath))
        {
            OutError = FString::Printf(TEXT("Failed to load CSV file '%s'."), *FilePath);
            return false;
        }

        FCsvParser Parser(CsvText);
        const FCsvParser::FRows& ParsedRows = Parser.GetRows();
        if (ParsedRows.Num() < 2)
        {
            OutError = TEXT("CSV must contain a header row and at least one data row.");
            return false;
        }

        TArray<FString> Headers;
        Headers.Reserve(ParsedRows[0].Num());
        for (const FString& RawHeader : ParsedRows[0])
        {
            Headers.Add(NormalizeCsvKey(RawHeader));
        }

        for (int32 RowIndex = 1; RowIndex < ParsedRows.Num(); ++RowIndex)
        {
            const TArray<const TCHAR*>& ParsedRow = ParsedRows[RowIndex];
            TMap<FString, FString> RowValues;
            bool bHasAnyValue = false;

            for (int32 CellIndex = 0; CellIndex < Headers.Num(); ++CellIndex)
            {
                const FString CellValue = ParsedRow.IsValidIndex(CellIndex)
                    ? SanitizeCsvCell(FString(ParsedRow[CellIndex]))
                    : FString();

                RowValues.Add(Headers[CellIndex], CellValue);
                bHasAnyValue |= !CellValue.IsEmpty();
            }

            if (bHasAnyValue)
            {
                OutRows.Add(MoveTemp(RowValues));
            }
        }

        if (OutRows.Num() == 0)
        {
            OutError = TEXT("CSV did not contain any non-empty data rows.");
            return false;
        }

        return true;
    }

    template <typename AssetType>
    static AssetType* CreateFlowAsset(const FString& TargetPath, const FString& BaseName, const bool bOpenInEditor)
    {
        FString PackagePath = TargetPath.IsEmpty() ? FDreamFlowEditorUtils::GetCurrentContentBrowserPath() : TargetPath;
        FText ValidationError;
        if (!FPackageName::IsValidLongPackageName(PackagePath, false, &ValidationError))
        {
            PackagePath = TEXT("/Game");
        }

        FString UniquePackageName;
        FString UniqueAssetName;
        FAssetToolsModule::GetModule().Get().CreateUniqueAssetName(PackagePath / BaseName, TEXT(""), UniquePackageName, UniqueAssetName);

        UPackage* Package = CreatePackage(*UniquePackageName);
        if (Package == nullptr)
        {
            return nullptr;
        }

        AssetType* NewAsset = NewObject<AssetType>(Package, *UniqueAssetName, RF_Public | RF_Standalone | RF_Transactional);
        if (NewAsset == nullptr)
        {
            return nullptr;
        }

        NewAsset->MarkPackageDirty();
        FAssetRegistryModule::AssetCreated(NewAsset);
        FDreamFlowEditorUtils::GetOrCreateGraph(NewAsset);

        TArray<UObject*> AssetsToSync;
        AssetsToSync.Add(NewAsset);
        IContentBrowserSingleton::Get().SyncBrowserToAssets(AssetsToSync, false, true);

        if (bOpenInEditor && GEditor != nullptr)
        {
            if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
            {
                AssetEditorSubsystem->OpenEditorForAsset(NewAsset);
            }
        }

        return NewAsset;
    }

    static UDreamFlowEdGraphNode* FindEntryGraphNode(UDreamFlowEdGraph* FlowGraph)
    {
        if (FlowGraph == nullptr)
        {
            return nullptr;
        }

        TArray<UDreamFlowEdGraphNode*> GraphNodes;
        FlowGraph->GetNodesOfClass(GraphNodes);
        for (UDreamFlowEdGraphNode* GraphNode : GraphNodes)
        {
            if (GraphNode != nullptr)
            {
                const UDreamFlowNode* RuntimeNode = GraphNode->GetRuntimeNode();
                if (RuntimeNode != nullptr && RuntimeNode->IsEntryNode())
                {
                    return GraphNode;
                }
            }
        }

        return nullptr;
    }

    static void LinkGraphNodes(UDreamFlowEdGraphNode* SourceNode, const FName OutputPinName, UDreamFlowEdGraphNode* TargetNode)
    {
        if (SourceNode == nullptr || TargetNode == nullptr)
        {
            return;
        }

        UEdGraphPin* OutputPin = SourceNode->FindOutputPinByName(OutputPinName);
        if (OutputPin == nullptr)
        {
            OutputPin = SourceNode->GetPrimaryOutputPin();
        }

        UEdGraphPin* InputPin = TargetNode->FindPin(UDreamFlowEdGraphNode::InputPinName, EGPD_Input);
        if (OutputPin != nullptr && InputPin != nullptr)
        {
            OutputPin->MakeLinkTo(InputPin);
        }
    }

    static FString PickCsvFile(const void* ParentWindowHandle, const FString& DialogTitle)
    {
        IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
        if (DesktopPlatform == nullptr)
        {
            return FString();
        }

        TArray<FString> SelectedFiles;
        const bool bPickedFile = DesktopPlatform->OpenFileDialog(
            ParentWindowHandle,
            DialogTitle,
            FPaths::ProjectContentDir(),
            FString(),
            TEXT("CSV Files (*.csv)|*.csv"),
            EFileDialogFlags::None,
            SelectedFiles);

        return bPickedFile && SelectedFiles.Num() > 0 ? SelectedFiles[0] : FString();
    }

    static bool ImportDialogueFlowFromCsv(const FString& FilePath, const FString& TargetPath)
    {
        TArray<TMap<FString, FString>> Rows;
        FString ErrorMessage;
        if (!LoadCsvRows(FilePath, Rows, ErrorMessage))
        {
            return false;
        }

        const FString BaseName = FString::Printf(TEXT("%s_DialogueFlow"), *FPaths::GetBaseFilename(FilePath));
        UDreamDialogueFlowAsset* FlowAsset = CreateFlowAsset<UDreamDialogueFlowAsset>(TargetPath, BaseName, true);
        UDreamFlowEdGraph* FlowGraph = FlowAsset != nullptr ? FDreamFlowEditorUtils::GetOrCreateGraph(FlowAsset) : nullptr;
        if (FlowAsset == nullptr || FlowGraph == nullptr)
        {
            return false;
        }

        TMap<FString, UDreamFlowEdGraphNode*> GraphNodeById;
        TArray<FString> OrderedIds;

        for (int32 RowIndex = 0; RowIndex < Rows.Num(); ++RowIndex)
        {
            const TMap<FString, FString>& Row = Rows[RowIndex];
            FString NodeId = GetCsvValue(Row, TEXT("id"));
            if (NodeId.IsEmpty())
            {
                NodeId = FString::Printf(TEXT("Row_%d"), RowIndex + 1);
            }

            const FString RowType = GetCsvValue(Row, TEXT("type")).ToLower();
            TSubclassOf<UDreamFlowNode> NodeClass = UDreamFlowDialogueLineNode::StaticClass();
            if (RowType == TEXT("choice"))
            {
                NodeClass = UDreamFlowDialogueChoiceNode::StaticClass();
            }
            else if (RowType == TEXT("end"))
            {
                NodeClass = UDreamFlowDialogueEndNode::StaticClass();
            }

            UDreamFlowEdGraphNode* GraphNode = FDreamFlowEditorUtils::CreateNodeInGraph(
                FlowGraph,
                NodeClass,
                FVector2f(340.0f + RowIndex * 420.0f, (RowIndex % 2 == 0) ? 0.0f : 180.0f),
                nullptr,
                false);

            if (GraphNode == nullptr)
            {
                continue;
            }

            OrderedIds.Add(NodeId);
            GraphNodeById.Add(NodeId, GraphNode);

            if (UDreamFlowDialogueLineNode* LineNode = Cast<UDreamFlowDialogueLineNode>(GraphNode->GetRuntimeNode()))
            {
                LineNode->SpeakerNameBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("speaker")));
                LineNode->LineTextBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("text")));
                LineNode->VoiceTagBinding = MakeNameLiteralBinding(GetCsvValue(Row, TEXT("voicetag")));
            }
            else if (UDreamFlowDialogueChoiceNode* ChoiceNode = Cast<UDreamFlowDialogueChoiceNode>(GraphNode->GetRuntimeNode()))
            {
                ChoiceNode->PromptBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("prompt")));
                ChoiceNode->ChoiceLabels.Reset();
                for (const FString& ChoiceString : SplitPipeList(GetCsvValue(Row, TEXT("choices"))))
                {
                    ChoiceNode->ChoiceLabels.Add(FText::FromString(ChoiceString));
                }
            }
            else if (UDreamFlowDialogueEndNode* EndNode = Cast<UDreamFlowDialogueEndNode>(GraphNode->GetRuntimeNode()))
            {
                EndNode->EndLabelBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("label")));
            }
        }

        if (UDreamFlowEdGraphNode* EntryNode = FindEntryGraphNode(FlowGraph))
        {
            if (OrderedIds.Num() > 0)
            {
                LinkGraphNodes(EntryNode, NAME_None, GraphNodeById.FindRef(OrderedIds[0]));
            }
        }

        for (const TMap<FString, FString>& Row : Rows)
        {
            UDreamFlowEdGraphNode* SourceNode = GraphNodeById.FindRef(GetCsvValue(Row, TEXT("id")));
            if (SourceNode == nullptr)
            {
                continue;
            }

            const FString RowType = GetCsvValue(Row, TEXT("type")).ToLower();
            if (RowType == TEXT("choice"))
            {
                for (const FString& TargetId : SplitPipeList(GetCsvValue(Row, TEXT("targets"))))
                {
                    LinkGraphNodes(SourceNode, NAME_None, GraphNodeById.FindRef(TargetId));
                }
            }
            else
            {
                LinkGraphNodes(SourceNode, NAME_None, GraphNodeById.FindRef(GetCsvValue(Row, TEXT("next"))));
            }
        }

        FDreamFlowEditorUtils::SynchronizeAssetFromGraph(FlowAsset);
        FlowGraph->NotifyGraphChanged();
        return true;
    }

    static bool ImportStoryFlowFromCsv(const FString& FilePath, const FString& TargetPath)
    {
        TArray<TMap<FString, FString>> Rows;
        FString ErrorMessage;
        if (!LoadCsvRows(FilePath, Rows, ErrorMessage))
        {
            return false;
        }

        const FString BaseName = FString::Printf(TEXT("%s_StoryFlow"), *FPaths::GetBaseFilename(FilePath));
        UDreamStoryFlowAsset* FlowAsset = CreateFlowAsset<UDreamStoryFlowAsset>(TargetPath, BaseName, true);
        UDreamFlowEdGraph* FlowGraph = FlowAsset != nullptr ? FDreamFlowEditorUtils::GetOrCreateGraph(FlowAsset) : nullptr;
        if (FlowAsset == nullptr || FlowGraph == nullptr)
        {
            return false;
        }

        TMap<FString, UDreamFlowEdGraphNode*> GraphNodeById;
        TArray<FString> OrderedIds;

        for (int32 RowIndex = 0; RowIndex < Rows.Num(); ++RowIndex)
        {
            const TMap<FString, FString>& Row = Rows[RowIndex];
            FString NodeId = GetCsvValue(Row, TEXT("id"));
            if (NodeId.IsEmpty())
            {
                NodeId = FString::Printf(TEXT("Row_%d"), RowIndex + 1);
            }

            const FString RowType = GetCsvValue(Row, TEXT("type")).ToLower();
            TSubclassOf<UDreamFlowNode> NodeClass = UDreamFlowStoryBeatNode::StaticClass();
            if (RowType == TEXT("branch"))
            {
                NodeClass = UDreamFlowStoryBranchNode::StaticClass();
            }
            else if (RowType == TEXT("ending"))
            {
                NodeClass = UDreamFlowStoryEndingNode::StaticClass();
            }

            UDreamFlowEdGraphNode* GraphNode = FDreamFlowEditorUtils::CreateNodeInGraph(
                FlowGraph,
                NodeClass,
                FVector2f(340.0f + RowIndex * 420.0f, (RowIndex % 2 == 0) ? 0.0f : 200.0f),
                nullptr,
                false);

            if (GraphNode == nullptr)
            {
                continue;
            }

            OrderedIds.Add(NodeId);
            GraphNodeById.Add(NodeId, GraphNode);

            const FString StoryArc = GetCsvValue(Row, TEXT("storyarc"));

            if (UDreamFlowStoryBeatNode* BeatNode = Cast<UDreamFlowStoryBeatNode>(GraphNode->GetRuntimeNode()))
            {
                BeatNode->StoryArcBinding = MakeNameLiteralBinding(StoryArc);
                BeatNode->BeatLabelBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("label")));
                BeatNode->BeatSummaryBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("summary")));
                BeatNode->CriticalBeatBinding = MakeBoolLiteralBinding(ParseBool(GetCsvValue(Row, TEXT("critical"))));
            }
            else if (UDreamFlowStoryBranchNode* BranchNode = Cast<UDreamFlowStoryBranchNode>(GraphNode->GetRuntimeNode()))
            {
                BranchNode->StoryArcBinding = MakeNameLiteralBinding(StoryArc);
                BranchNode->DecisionPromptBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("prompt")));
                BranchNode->MajorDecisionBinding = MakeBoolLiteralBinding(ParseBool(GetCsvValue(Row, TEXT("major")), true));
                BranchNode->BranchLabels.Reset();

                TArray<FString> Labels = SplitPipeList(GetCsvValue(Row, TEXT("labels")));
                const TArray<FString> Targets = SplitPipeList(GetCsvValue(Row, TEXT("targets")));
                if (Labels.Num() == 0 && Targets.Num() > 0)
                {
                    for (int32 TargetIndex = 0; TargetIndex < Targets.Num(); ++TargetIndex)
                    {
                        Labels.Add(FString::Printf(TEXT("Choice %d"), TargetIndex + 1));
                    }
                }

                for (const FString& Label : Labels)
                {
                    BranchNode->BranchLabels.Add(FText::FromString(Label));
                }

                GraphNode->ReconstructNode();
            }
            else if (UDreamFlowStoryEndingNode* EndingNode = Cast<UDreamFlowStoryEndingNode>(GraphNode->GetRuntimeNode()))
            {
                EndingNode->StoryArcBinding = MakeNameLiteralBinding(StoryArc);
                EndingNode->EndingLabelBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("label")));
                EndingNode->OutcomeSummaryBinding = MakeTextLiteralBinding(GetCsvValue(Row, TEXT("outcome")));
            }
        }

        if (UDreamFlowEdGraphNode* EntryNode = FindEntryGraphNode(FlowGraph))
        {
            if (OrderedIds.Num() > 0)
            {
                LinkGraphNodes(EntryNode, NAME_None, GraphNodeById.FindRef(OrderedIds[0]));
            }
        }

        for (const TMap<FString, FString>& Row : Rows)
        {
            UDreamFlowEdGraphNode* SourceNode = GraphNodeById.FindRef(GetCsvValue(Row, TEXT("id")));
            if (SourceNode == nullptr)
            {
                continue;
            }

            const FString RowType = GetCsvValue(Row, TEXT("type")).ToLower();
            if (RowType == TEXT("branch"))
            {
                const TArray<FString> Targets = SplitPipeList(GetCsvValue(Row, TEXT("targets")));
                for (int32 TargetIndex = 0; TargetIndex < Targets.Num(); ++TargetIndex)
                {
                    LinkGraphNodes(SourceNode, FName(*FString::Printf(TEXT("Choice_%d"), TargetIndex)), GraphNodeById.FindRef(Targets[TargetIndex]));
                }
            }
            else
            {
                LinkGraphNodes(SourceNode, NAME_None, GraphNodeById.FindRef(GetCsvValue(Row, TEXT("next"))));
            }
        }

        FDreamFlowEditorUtils::SynchronizeAssetFromGraph(FlowAsset);
        FlowGraph->NotifyGraphChanged();
        return true;
    }

    static bool CreateDialogueExampleFlow(const FString& TargetPath)
    {
        UDreamDialogueFlowAsset* FlowAsset = CreateFlowAsset<UDreamDialogueFlowAsset>(TargetPath, TEXT("DreamDialogueExample"), true);
        UDreamFlowEdGraph* FlowGraph = FlowAsset != nullptr ? FDreamFlowEditorUtils::GetOrCreateGraph(FlowAsset) : nullptr;
        if (FlowAsset == nullptr || FlowGraph == nullptr)
        {
            return false;
        }

        UDreamFlowEdGraphNode* LineNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowDialogueLineNode::StaticClass(), FVector2f(360.0f, 0.0f), nullptr, false);
        UDreamFlowEdGraphNode* ChoiceNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowDialogueChoiceNode::StaticClass(), FVector2f(780.0f, 0.0f), nullptr, false);
        UDreamFlowEdGraphNode* ReplyNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowDialogueLineNode::StaticClass(), FVector2f(1220.0f, -120.0f), nullptr, false);
        UDreamFlowEdGraphNode* EndNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowDialogueEndNode::StaticClass(), FVector2f(1220.0f, 120.0f), nullptr, false);

        if (UDreamFlowDialogueLineNode* IntroLine = LineNode != nullptr ? Cast<UDreamFlowDialogueLineNode>(LineNode->GetRuntimeNode()) : nullptr)
        {
            IntroLine->SpeakerNameBinding = MakeTextLiteralBinding(TEXT("Guide"));
            IntroLine->LineTextBinding = MakeTextLiteralBinding(TEXT("The road splits here. Choose whether to learn more or move on."));
            IntroLine->VoiceTagBinding = MakeNameLiteralBinding(TEXT("Guide_Intro"));
        }

        if (UDreamFlowDialogueChoiceNode* Choice = ChoiceNode != nullptr ? Cast<UDreamFlowDialogueChoiceNode>(ChoiceNode->GetRuntimeNode()) : nullptr)
        {
            Choice->PromptBinding = MakeTextLiteralBinding(TEXT("How do you respond?"));
            Choice->ChoiceLabels = { FText::FromString(TEXT("Tell me more")), FText::FromString(TEXT("I'm ready")) };
        }

        if (UDreamFlowDialogueLineNode* Reply = ReplyNode != nullptr ? Cast<UDreamFlowDialogueLineNode>(ReplyNode->GetRuntimeNode()) : nullptr)
        {
            Reply->SpeakerNameBinding = MakeTextLiteralBinding(TEXT("Guide"));
            Reply->LineTextBinding = MakeTextLiteralBinding(TEXT("Every branch is a promise. Keep your variables clean and your sub flows small."));
            Reply->VoiceTagBinding = MakeNameLiteralBinding(TEXT("Guide_Advice"));
        }

        if (UDreamFlowDialogueEndNode* End = EndNode != nullptr ? Cast<UDreamFlowDialogueEndNode>(EndNode->GetRuntimeNode()) : nullptr)
        {
            End->EndLabelBinding = MakeTextLiteralBinding(TEXT("Conversation Complete"));
        }

        if (UDreamFlowEdGraphNode* EntryNode = FindEntryGraphNode(FlowGraph))
        {
            LinkGraphNodes(EntryNode, NAME_None, LineNode);
        }

        LinkGraphNodes(LineNode, NAME_None, ChoiceNode);
        LinkGraphNodes(ChoiceNode, NAME_None, ReplyNode);
        LinkGraphNodes(ChoiceNode, NAME_None, EndNode);
        LinkGraphNodes(ReplyNode, NAME_None, EndNode);

        FDreamFlowEditorUtils::SynchronizeAssetFromGraph(FlowAsset);
        FlowGraph->NotifyGraphChanged();
        return true;
    }

    static bool CreateStoryExampleFlow(const FString& TargetPath)
    {
        UDreamStoryFlowAsset* FlowAsset = CreateFlowAsset<UDreamStoryFlowAsset>(TargetPath, TEXT("DreamStoryExample"), true);
        UDreamFlowEdGraph* FlowGraph = FlowAsset != nullptr ? FDreamFlowEditorUtils::GetOrCreateGraph(FlowAsset) : nullptr;
        if (FlowAsset == nullptr || FlowGraph == nullptr)
        {
            return false;
        }

        UDreamFlowEdGraphNode* BeatNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowStoryBeatNode::StaticClass(), FVector2f(360.0f, 0.0f), nullptr, false);
        UDreamFlowEdGraphNode* BranchNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowStoryBranchNode::StaticClass(), FVector2f(820.0f, 0.0f), nullptr, false);
        UDreamFlowEdGraphNode* SuccessNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowStoryBeatNode::StaticClass(), FVector2f(1280.0f, -140.0f), nullptr, false);
        UDreamFlowEdGraphNode* EndingNode = FDreamFlowEditorUtils::CreateNodeInGraph(FlowGraph, UDreamFlowStoryEndingNode::StaticClass(), FVector2f(1280.0f, 160.0f), nullptr, false);

        if (UDreamFlowStoryBeatNode* IntroBeat = BeatNode != nullptr ? Cast<UDreamFlowStoryBeatNode>(BeatNode->GetRuntimeNode()) : nullptr)
        {
            IntroBeat->StoryArcBinding = MakeNameLiteralBinding(TEXT("MainArc"));
            IntroBeat->BeatLabelBinding = MakeTextLiteralBinding(TEXT("An Offer at Dusk"));
            IntroBeat->BeatSummaryBinding = MakeTextLiteralBinding(TEXT("A messenger arrives with a sealed map and an impossible deadline."));
            IntroBeat->CriticalBeatBinding = MakeBoolLiteralBinding(true);
        }

        if (UDreamFlowStoryBranchNode* Branch = BranchNode != nullptr ? Cast<UDreamFlowStoryBranchNode>(BranchNode->GetRuntimeNode()) : nullptr)
        {
            Branch->StoryArcBinding = MakeNameLiteralBinding(TEXT("MainArc"));
            Branch->DecisionPromptBinding = MakeTextLiteralBinding(TEXT("Do you follow the hidden road or fortify the town?"));
            Branch->BranchLabels = { FText::FromString(TEXT("Hidden Road")), FText::FromString(TEXT("Fortify")) };
            Branch->MajorDecisionBinding = MakeBoolLiteralBinding(true);
            BranchNode->ReconstructNode();
        }

        if (UDreamFlowStoryBeatNode* SuccessBeat = SuccessNode != nullptr ? Cast<UDreamFlowStoryBeatNode>(SuccessNode->GetRuntimeNode()) : nullptr)
        {
            SuccessBeat->StoryArcBinding = MakeNameLiteralBinding(TEXT("MainArc"));
            SuccessBeat->BeatLabelBinding = MakeTextLiteralBinding(TEXT("The Road Answers"));
            SuccessBeat->BeatSummaryBinding = MakeTextLiteralBinding(TEXT("The shortcut works, but it reveals an older threat waiting beyond the pass."));
            SuccessBeat->CriticalBeatBinding = MakeBoolLiteralBinding(false);
        }

        if (UDreamFlowStoryEndingNode* Ending = EndingNode != nullptr ? Cast<UDreamFlowStoryEndingNode>(EndingNode->GetRuntimeNode()) : nullptr)
        {
            Ending->StoryArcBinding = MakeNameLiteralBinding(TEXT("MainArc"));
            Ending->EndingLabelBinding = MakeTextLiteralBinding(TEXT("Chapter End"));
            Ending->OutcomeSummaryBinding = MakeTextLiteralBinding(TEXT("The town survives the night, but the map remains unread and the larger journey is delayed."));
        }

        if (UDreamFlowEdGraphNode* EntryNode = FindEntryGraphNode(FlowGraph))
        {
            LinkGraphNodes(EntryNode, NAME_None, BeatNode);
        }

        LinkGraphNodes(BeatNode, NAME_None, BranchNode);
        LinkGraphNodes(BranchNode, FName(TEXT("Choice_0")), SuccessNode);
        LinkGraphNodes(BranchNode, FName(TEXT("Choice_1")), EndingNode);
        LinkGraphNodes(SuccessNode, NAME_None, EndingNode);

        FDreamFlowEditorUtils::SynchronizeAssetFromGraph(FlowAsset);
        FlowGraph->NotifyGraphChanged();
        return true;
    }
}

void FDreamFlowExpansionEditorModule::StartupModule()
{
    RegisterAssetTools();
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FDreamFlowExpansionEditorModule::RegisterMenus));
}

void FDreamFlowExpansionEditorModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    UnregisterAssetTools();
}

void FDreamFlowExpansionEditorModule::RegisterAssetTools()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("DreamFlow")), LOCTEXT("DreamFlowAssetCategory", "DreamFlow"));

    const auto RegisterTypeAction = [this, &AssetTools](UClass* SupportedClass, const FText& DisplayName, const FColor& TypeColor)
    {
        TSharedRef<FDreamFlowExpansionAssetTypeActions> AssetTypeAction = MakeShared<FDreamFlowExpansionAssetTypeActions>(
            AssetCategory,
            SupportedClass,
            DisplayName,
            TypeColor);
        AssetTools.RegisterAssetTypeActions(AssetTypeAction);
        RegisteredAssetTypeActions.Add(AssetTypeAction);
    };

    RegisterTypeAction(UDreamQuestFlowAsset::StaticClass(), LOCTEXT("DreamQuestFlowAssetName", "Dream Quest Flow"), FColor(61, 158, 111));
    RegisterTypeAction(UDreamDialogueFlowAsset::StaticClass(), LOCTEXT("DreamDialogueFlowAssetName", "Dream Dialogue Flow"), FColor(67, 113, 217));
    RegisterTypeAction(UDreamStoryFlowAsset::StaticClass(), LOCTEXT("DreamStoryFlowAssetName", "Dream Story Flow"), FColor(188, 74, 112));
    RegisterTypeAction(UDreamEncounterFlowAsset::StaticClass(), LOCTEXT("DreamEncounterFlowAssetName", "Dream Encounter Flow"), FColor(51, 122, 173));
}

void FDreamFlowExpansionEditorModule::UnregisterAssetTools()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        RegisteredAssetTypeActions.Reset();
        return;
    }

    IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

    for (const TSharedPtr<IAssetTypeActions>& AssetTypeAction : RegisteredAssetTypeActions)
    {
        if (AssetTypeAction.IsValid())
        {
            AssetTools.UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
        }
    }

    RegisteredAssetTypeActions.Reset();
}

void FDreamFlowExpansionEditorModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AddNewContextMenu"))
    {
        FToolMenuSection& Section = Menu->FindOrAddSection("ContentBrowserNewAsset");
        Section.AddDynamicEntry(
            "DreamFlowExpansionAddNewActions",
            FNewToolMenuSectionDelegate::CreateRaw(this, &FDreamFlowExpansionEditorModule::AddDreamFlowExpansionAddNewMenu));
    }
}

void FDreamFlowExpansionEditorModule::AddDreamFlowExpansionAddNewMenu(FToolMenuSection& Section)
{
    const UContentBrowserDataMenuContext_AddNewMenu* Context = Section.FindContext<UContentBrowserDataMenuContext_AddNewMenu>();
    if (Context == nullptr || !Context->bCanBeModified || !Context->bContainsValidPackagePath)
    {
        return;
    }

    const FString TargetPath = ResolveTargetPath(Context);

    Section.AddSubMenu(
        "DreamFlowExpansionAddNewMenu",
        LOCTEXT("DreamFlowExpansionAddNewMenu", "DreamFlow Expansion"),
        LOCTEXT("DreamFlowExpansionAddNewMenuTooltip", "Import CSV-backed expansion flows or create ready-to-edit example templates."),
        FNewToolMenuDelegate::CreateLambda([TargetPath](UToolMenu* Menu)
        {
            const void* ParentWindowHandle = nullptr;

            FToolMenuSection& ImportSection = Menu->AddSection("DreamFlowExpansionImportSection", LOCTEXT("DreamFlowExpansionImportSection", "Import"));
            ImportSection.AddMenuEntry(
                "DreamFlowImportDialogueCsv",
                LOCTEXT("DreamFlowImportDialogueCsv", "Import Dialogue Flow CSV..."),
                LOCTEXT("DreamFlowImportDialogueCsvTooltip", "Create a Dream Dialogue Flow asset from a CSV file in the current content browser path."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([TargetPath, ParentWindowHandle]()
                {
                    const FString FilePath = PickCsvFile(ParentWindowHandle, TEXT("Import Dialogue Flow CSV"));
                    if (!FilePath.IsEmpty())
                    {
                        ImportDialogueFlowFromCsv(FilePath, TargetPath);
                    }
                })));

            ImportSection.AddMenuEntry(
                "DreamFlowImportStoryCsv",
                LOCTEXT("DreamFlowImportStoryCsv", "Import Story Flow CSV..."),
                LOCTEXT("DreamFlowImportStoryCsvTooltip", "Create a Dream Story Flow asset from a CSV file in the current content browser path."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([TargetPath, ParentWindowHandle]()
                {
                    const FString FilePath = PickCsvFile(ParentWindowHandle, TEXT("Import Story Flow CSV"));
                    if (!FilePath.IsEmpty())
                    {
                        ImportStoryFlowFromCsv(FilePath, TargetPath);
                    }
                })));

            FToolMenuSection& ExampleSection = Menu->AddSection("DreamFlowExpansionExampleSection", LOCTEXT("DreamFlowExpansionExampleSection", "Examples"));
            ExampleSection.AddMenuEntry(
                "DreamFlowCreateDialogueExample",
                LOCTEXT("DreamFlowCreateDialogueExample", "Create Dialogue Example"),
                LOCTEXT("DreamFlowCreateDialogueExampleTooltip", "Create a sample Dream Dialogue Flow asset in the current content browser path."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([TargetPath]()
                {
                    CreateDialogueExampleFlow(TargetPath);
                })));

            ExampleSection.AddMenuEntry(
                "DreamFlowCreateStoryExample",
                LOCTEXT("DreamFlowCreateStoryExample", "Create Story Example"),
                LOCTEXT("DreamFlowCreateStoryExampleTooltip", "Create a sample Dream Story Flow asset in the current content browser path."),
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateLambda([TargetPath]()
                {
                    CreateStoryExampleFlow(TargetPath);
                })));
        }));
}

IMPLEMENT_MODULE(FDreamFlowExpansionEditorModule, DreamFlowExpansionEditor)

#undef LOCTEXT_NAMESPACE
