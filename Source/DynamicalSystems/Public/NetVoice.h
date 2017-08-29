#pragma once

#include <memory>
#include <atomic>
#include <vector>
#include "CoreMinimal.h"
#include "NetClient.h"
#include "Sound/SoundWaveProcedural.h"
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
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetRigidBody")
    ANetClient* NetClient = NULL;
    
    void Say(uint8* Bytes, uint32 Count);
    
private:
    
    TSharedPtr <class IVoiceCapture> VoiceCapture;
    
    void FillAudio(USoundWaveProcedural* Wave, const int32 SamplesNeeded);
    
    uint32 OptimalFramesPerBuffer;
    uint32 SampleRate;
    std::unique_ptr<USoundWaveProcedural> SoundStream;
    
    std::vector<uint8> Buffer;
    bool bPlayingSound;
    uint32 PlayCursor;
    
    std::atomic_flag StartPlaying;
};
