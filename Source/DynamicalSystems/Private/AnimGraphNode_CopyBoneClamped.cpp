#ifdef UE_EDITOR

#include "AnimGraphNode_CopyBoneClamped.h"
#include "DynamicalSystemsPrivatePCH.h"

#define LOCTEXT_NAMESPACE "A3Nodes"

UAnimGraphNode_CopyBoneClamped::UAnimGraphNode_CopyBoneClamped(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

FText UAnimGraphNode_CopyBoneClamped::GetControllerDescription() const
{
    return LOCTEXT("CopyBoneClamped", "Copy Bone Clamped");
}

FText UAnimGraphNode_CopyBoneClamped::GetTooltipText() const
{
    return LOCTEXT("AnimGraphNode_CopyBoneClamped_Tooltip", "The Copy Bone control copies the Transform data or any component of it - i.e. Translation, Rotation, or Scale - from one bone to another. Clamped.");
}

FText UAnimGraphNode_CopyBoneClamped::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    if ((TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle) && (Node.TargetBone.BoneName == NAME_None) && (Node.SourceBone.BoneName == NAME_None))
    {
        return GetControllerDescription();
    }
    // @TODO: the bone can be altered in the property editor, so we have to
    //        choose to mark this dirty when that happens for this to properly work
    else //if (!CachedNodeTitles.IsTitleCached(TitleType, this))
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("ControllerDescription"), GetControllerDescription());
        Args.Add(TEXT("SourceBoneName"), FText::FromName(Node.SourceBone.BoneName));
        Args.Add(TEXT("TargetBoneName"), FText::FromName(Node.TargetBone.BoneName));
        
        if (TitleType == ENodeTitleType::ListView || TitleType == ENodeTitleType::MenuTitle)
        {
            CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_CopyBoneClamped_ListTitle", "{ControllerDescription} - Source Bone: {SourceBoneName} - Target Bone: {TargetBoneName}"), Args), this);
        }
        else
        {
            CachedNodeTitles.SetCachedTitle(TitleType, FText::Format(LOCTEXT("AnimGraphNode_CopyBoneClamped_Title", "{ControllerDescription}\nSource Bone: {SourceBoneName}\nTarget Bone: {TargetBoneName}"), Args), this);
        }
    }
    return CachedNodeTitles[TitleType];
}

#undef LOCTEXT_NAMESPACE

#endif // UE_EDITOR
