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

void UNetVoice::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!IsValid(NetClient)) return;
    
    const size_t MaxBytes = 1024;
    
    uint32 BytesAvailable = 0;
    EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
    if (CaptureState == EVoiceCaptureState::Ok && BytesAvailable > 0)
    {
        uint8 Buf[MaxBytes];
        memset(Buf, 0, MaxBytes);
        uint32 ReadBytes = 0;
        VoiceCapture->GetVoiceData(Buf, MaxBytes, ReadBytes);
        
        uint32 Samples = ReadBytes / 2;
        float* SampleBuf = new float[Samples];
        
        int16_t Sample;
        for (uint32 I = 0; I < Samples; ++I)
        {
            Sample = (Buf[I * 2 + 1] << 8) | Buf[I * 2];
            SampleBuf[I] = float(Sample) / 32768.0f;
        }
        
        UE_LOG(LogTemp, Warning, TEXT("Voice Samples %i"), Samples);
        
        delete[] SampleBuf;
    }
}

