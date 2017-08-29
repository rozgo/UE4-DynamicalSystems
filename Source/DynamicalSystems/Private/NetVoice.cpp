#include "NetVoice.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetVoice::UNetVoice()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetVoice::BeginPlay()
{
	Super::BeginPlay();
    VoiceCapture = FVoiceModule::Get().CreateVoiceCapture();
    VoiceCapture->Start();
}

const size_t MaxBytes = 1024 * 100;
uint8 Buf[MaxBytes];

void UNetVoice::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!IsValid(NetClient)) return;
    
    uint32 BytesAvailable = 0;
    EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
    if (CaptureState == EVoiceCaptureState::Ok && BytesAvailable > 0)
    {
        uint32 BytesRead = 0;
        VoiceCapture->GetVoiceData(Buf, MaxBytes, BytesRead);
        NetClient->Say(Buf, BytesRead);
        UE_LOG(LogTemp, Warning, TEXT("NetVoice BytesAvailable: %i BytesRead: %i"), BytesAvailable, BytesRead);
    }
}

