#pragma once

#include "CoreMinimal.h"
#include "NetClient.h"
#include "Runtime/Online/Voice/Public/VoiceModule.h"
#include "NetVoice.generated.h"

UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API UNetVoice : public UActorComponent
{
    GENERATED_BODY()
    
public:
    
    UNetVoice();
    
    virtual void BeginPlay() override;
    
    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
    
    TSharedPtr <class IVoiceCapture> VoiceCapture;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetRigidBody")
    ANetClient* NetClient = NULL;
    
};
