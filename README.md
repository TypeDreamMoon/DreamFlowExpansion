# DreamFlowExpansion

DreamFlowExpansion is the domain-extension plugin for `DreamFlow`.

It moves the previously bundled quest and dialogue flow families out of the core `DreamFlow` plugin, and adds extra reusable flow families for story-driven and encounter-driven workflows.

## What it includes

- `DreamFlowExpansion`: runtime module for expansion flow assets and node families
- `DreamFlowExpansionEditor`: editor module that registers expansion asset types and factories
- `UDreamQuestFlowAsset`
- `UDreamDialogueFlowAsset`
- `UDreamStoryFlowAsset`
- `UDreamEncounterFlowAsset`
- Quest nodes:
  - `UDreamFlowQuestTaskNode`
  - `UDreamFlowQuestConditionNode`
  - `UDreamFlowQuestCompleteNode`
- Dialogue nodes:
  - `UDreamFlowDialogueLineNode`
  - `UDreamFlowDialogueChoiceNode`
  - `UDreamFlowDialogueEndNode`
- Story nodes:
  - `UDreamFlowStoryBeatNode`
  - `UDreamFlowStoryBranchNode`
  - `UDreamFlowStoryEndingNode`
- Encounter nodes:
  - `UDreamFlowEncounterSetupNode`
  - `UDreamFlowEncounterWaveNode`
  - `UDreamFlowEncounterResolveNode`

## Plugin layout

- `DreamFlow` stays focused on the generic graph framework, executor, node base types, validation, debugger, and shared editor.
- `DreamFlowExpansion` contains domain-specific flow asset subclasses and domain-specific node families.
- `DreamFlowExpansionEditor` registers the Content Browser asset types so these expansion assets still open in the standard DreamFlow editor.

This keeps the core plugin reusable while letting you grow domain packs independently.

## Included flow families

### Quest

Quest flows are intended for objective chains, requirement gates, and terminal reward/completion handling.

- `UDreamFlowQuestTaskNode` represents a concrete objective or step
- `UDreamFlowQuestConditionNode` represents a quest-state or gameplay gate
- `UDreamFlowQuestCompleteNode` is a terminal completion node

### Dialogue

Dialogue flows are intended for authored conversations and branching responses.

- `UDreamFlowDialogueLineNode` represents a spoken or narrated line
- `UDreamFlowDialogueChoiceNode` represents a response branch
- `UDreamFlowDialogueEndNode` is a terminal conversation close

### Story

Story flows are intended for narrative progression, chapter planning, and consequence-heavy branching.

- `UDreamFlowStoryBeatNode` represents a narrative beat
- `UDreamFlowStoryBranchNode` represents a story decision point with named output pins
- `UDreamFlowStoryEndingNode` closes a story thread or chapter

This family is useful when you want a flow that is broader than pure dialogue and more narrative-facing than quest logic.

### Encounter

Encounter flows are intended for combat beats, scripted challenge pacing, and gameplay set-piece orchestration.

- `UDreamFlowEncounterSetupNode` defines encounter setup and framing
- `UDreamFlowEncounterWaveNode` represents an active encounter phase
- `UDreamFlowEncounterResolveNode` is a terminal resolution node

This family is useful for combat, stealth, arena, survival, or boss-phase style content.

## How to use

1. Enable both `DreamFlow` and `DreamFlowExpansion`.
2. In Content Browser create one of these asset types:
   `Dream Quest Flow`, `Dream Dialogue Flow`, `Dream Story Flow`, or `Dream Encounter Flow`.
3. Open the asset in the standard DreamFlow editor.
4. Add expansion nodes from the graph context menu, class picker, or palette.
5. Use `Create Node Class` if you want a Blueprint implementation derived from one of the expansion node bases.

All expansion asset types reuse the same DreamFlow graph editor, debugger, validation panel, variables panel, and node details workflow.

## Asset compatibility

Each expansion node family narrows `SupportedFlowAssetType` to its matching flow asset subclass.

That means:

- Quest nodes only appear in `UDreamQuestFlowAsset`
- Dialogue nodes only appear in `UDreamDialogueFlowAsset`
- Story nodes only appear in `UDreamStoryFlowAsset`
- Encounter nodes only appear in `UDreamEncounterFlowAsset`

Generic `DreamFlow` core nodes still remain available anywhere they are compatible.

## Migration from old modules

Older versions of the project exposed quest and dialogue through the `DreamFlowQuest` and `DreamFlowDialogue` modules inside the `DreamFlow` plugin.

Those runtime modules have been removed and replaced by `DreamFlowExpansion`.

To preserve existing assets, the plugin now ships class redirects for:

- `DreamFlowDialogue -> DreamFlowExpansion`
- `DreamFlowQuest -> DreamFlowExpansion`

So existing assets referencing:

- `UDreamDialogueFlowAsset`
- `UDreamQuestFlowAsset`
- old dialogue node classes
- old quest node classes

should continue to resolve after loading the new plugin layout.

## Recommended extension pattern

If you want to add more domain packs later, treat `DreamFlowExpansion` as the pattern:

- create a new `UDreamFlowAsset` subclass for the domain
- create a focused node family derived from `UDreamFlowNode`
- set `SupportedFlowAssetType` in the node constructor
- register the asset type in an editor module
- keep reusable framework features inside core `DreamFlow`

This keeps the base framework stable while letting feature packs evolve independently.
