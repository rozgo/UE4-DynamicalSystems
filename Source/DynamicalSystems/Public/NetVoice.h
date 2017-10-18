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
	virtual void BeginDestroy() override;
    
    virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetVoice")
    ANetClient* NetClient = NULL;
    
    void Say(uint8* Bytes, uint32 Count);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetVoice")
	USoundWaveProcedural* SoundStream;

	uint32 Activity = 0;
    
private:
    
    TSharedPtr <class IVoiceCapture> VoiceCapture;

    uint32 SampleRate;
	uint32 SayActivity = 0;
};
