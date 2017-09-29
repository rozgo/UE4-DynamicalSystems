#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNodeUtils.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimNode_CopyBoneClamped.h"
#include "AnimGraphNode_CopyBoneClamped.generated.h"

UCLASS(MinimalAPI)
class UAnimGraphNode_CopyBoneClamped : public UAnimGraphNode_SkeletalControlBase
{
    GENERATED_UCLASS_BODY()
    
    UPROPERTY(EditAnywhere, Category=Settings)
    FAnimNode_CopyBoneClamped Node;
    
public:
    // UEdGraphNode interface
    virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
    virtual FText GetTooltipText() const override;
    // End of UEdGraphNode interface
    
protected:
    // UAnimGraphNode_SkeletalControlBase interface
    virtual FText GetControllerDescription() const override;
    virtual const FAnimNode_SkeletalControlBase* GetNode() const override { return &Node; }
    // End of UAnimGraphNode_SkeletalControlBase interface
    
private:
    /** Constructing FText strings can be costly, so we cache the node's title */
    FNodeTitleTextTable CachedNodeTitles;
};
