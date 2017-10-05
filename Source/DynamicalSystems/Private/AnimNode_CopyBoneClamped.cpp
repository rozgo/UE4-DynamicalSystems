#include "AnimNode_CopyBoneClamped.h"
#include "AnimationRuntime.h"
#include "Animation/AnimInstanceProxy.h"
#include "DynamicalSystemsPrivatePCH.h"

/////////////////////////////////////////////////////
// FAnimNode_CopyBoneClamped

FAnimNode_CopyBoneClamped::FAnimNode_CopyBoneClamped()
: ControlSpace(BCS_ComponentSpace)
{
}

void FAnimNode_CopyBoneClamped::GatherDebugData(FNodeDebugData& DebugData)
{
    FString DebugLine = DebugData.GetNodeName(this);
    
    DebugLine += "(";
    AddDebugNodeData(DebugLine);
    DebugLine += FString::Printf(TEXT(" Src: %s Dst: %s)"), *SourceBone.BoneName.ToString(), *TargetBone.BoneName.ToString());
    DebugData.AddDebugItem(DebugLine);
    
    ComponentPose.GatherDebugData(DebugData);
}

void FAnimNode_CopyBoneClamped::EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms)
{
    check(OutBoneTransforms.Num() == 0);
    
//    // Pass through if we're not doing anything.
//    if( !bCopyTranslation && !bCopyRotation && !bCopyScale )
//    {
//        return;
//    }
    
    // Get component space transform for source and current bone.
    const FBoneContainer& BoneContainer = Output.Pose.GetPose().GetBoneContainer();
    FCompactPoseBoneIndex SourceBoneIndex = SourceBone.GetCompactPoseIndex(BoneContainer);
    FCompactPoseBoneIndex TargetBoneIndex = TargetBone.GetCompactPoseIndex(BoneContainer);
    
    FTransform SourceBoneTM = Output.Pose.GetComponentSpaceTransform(SourceBoneIndex);
    FTransform CurrentBoneTM = Output.Pose.GetComponentSpaceTransform(TargetBoneIndex);
    
    if(ControlSpace != BCS_ComponentSpace)
    {
        // Convert out to selected space
        FAnimationRuntime::ConvertCSTransformToBoneSpace(Output.AnimInstanceProxy->GetComponentTransform(), Output.Pose, SourceBoneTM, SourceBoneIndex, ControlSpace);
        FAnimationRuntime::ConvertCSTransformToBoneSpace(Output.AnimInstanceProxy->GetComponentTransform(), Output.Pose, CurrentBoneTM, TargetBoneIndex, ControlSpace);
    }
    
    FVector SourceTranslation = SourceBoneTM.GetTranslation();
    FVector TargetTranslation = CurrentBoneTM.GetTranslation();
    
    FVector SourceToTarget = TargetTranslation - SourceTranslation;
    SourceToTarget = SourceToTarget.GetClampedToSize(MinDist, MaxDist);
    TargetTranslation = SourceTranslation + SourceToTarget;
    TargetTranslation.Z = fmax(Floor, TargetTranslation.Z);
    CurrentBoneTM.SetTranslation(TargetTranslation);


	//SourceTranslation.Z = fmax(Floor, SourceTranslation.Z);
	//CurrentBoneTM.SetTranslation(SourceTranslation);

    
//    // Copy individual components
//    if (bCopyTranslation)
//    {
//        CurrentBoneTM.SetTranslation( SourceBoneTM.GetTranslation() );
//    }
//
//    if (bCopyRotation)
//    {
//        CurrentBoneTM.SetRotation( SourceBoneTM.GetRotation() );
//    }
//
//    if (bCopyScale)
//    {
//        CurrentBoneTM.SetScale3D( SourceBoneTM.GetScale3D() );
//    }
    
    if(ControlSpace != BCS_ComponentSpace)
    {
        // Convert back out if we aren't operating in component space
        FAnimationRuntime::ConvertBoneSpaceTransformToCS(Output.AnimInstanceProxy->GetComponentTransform(), Output.Pose, CurrentBoneTM, TargetBoneIndex, ControlSpace);
    }
    
    // Output new transform for current bone.
    OutBoneTransforms.Add(FBoneTransform(TargetBoneIndex, CurrentBoneTM));
}

bool FAnimNode_CopyBoneClamped::IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones)
{
    // if both bones are valid
    return (TargetBone.IsValidToEvaluate(RequiredBones) && (TargetBone==SourceBone || SourceBone.IsValidToEvaluate(RequiredBones)));
}

void FAnimNode_CopyBoneClamped::InitializeBoneReferences(const FBoneContainer& RequiredBones)
{
    SourceBone.Initialize(RequiredBones);
    TargetBone.Initialize(RequiredBones);
}
