#include "NetVoice.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetVoice::UNetVoice()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetVoice::BeginPlay()
{
	Super::BeginPlay();
    
    NetClient->RegisterVoice(this);
    
    VoiceCapture = FVoiceModule::Get().CreateVoiceCapture();
    VoiceCapture->Start();
    
    StartPlaying.test_and_set();
    
    OptimalFramesPerBuffer            = 160;
    SampleRate                        = 16000;
    
    SoundStream.reset(NewObject<USoundWaveProcedural>());
    
    SoundStream->SampleRate = SampleRate;
    SoundStream->NumChannels = 1;
    SoundStream->Duration = INDEFINITELY_LOOPING_DURATION;
    SoundStream->SoundGroup = SOUNDGROUP_Default;
    SoundStream->bLooping = false;
    
//    SoundStream->OnSoundWaveProceduralUnderflow.BindLambda
//    ([this](USoundWaveProcedural* Wave, const int32 SamplesNeeded) {
//        FillAudio(Wave, SamplesNeeded);
//    });
    
    UGameplayStatics::PlaySound2D(GetWorld(), SoundStream.get(),
                                  .5 /* volume multiplier */,
                                  1  /* pitch multiplier */,
                                  0);
}

void UNetVoice::Say(uint8* Bytes, uint32 Count)
{
    SoundStream->QueueAudio(Bytes, Count);
}

const size_t MaxBytes = 1024 * 100;
uint8 Buf[MaxBytes];

void UNetVoice::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!IsValid(NetClient)) return;
    
    uint32 BytesAvailable = 0;
    EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
    while (CaptureState == EVoiceCaptureState::Ok && BytesAvailable > 0)
    {
        uint32 BytesRead = 0;
        VoiceCapture->GetVoiceData(Buf, MaxBytes, BytesRead);
//        UE_LOG(LogTemp, Warning, TEXT("NetVoice BytesAvailable: %i BytesRead: %i"), BytesAvailable, BytesRead);
        NetClient->Say(Buf, BytesRead);
        CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
    }
}

void UNetVoice::FillAudio(USoundWaveProcedural* Wave, const int32 SamplesNeeded)
{
    // Unreal engine uses a fixed sample size.
    static const uint32 SAMPLE_SIZE = sizeof(uint16);
    
    if (!StartPlaying.test_and_set())
    {
        PlayCursor = 0;
        bPlayingSound = true;
    }
    
    // We're using only one channel.
    const uint32 OptimalSampleCount = OptimalFramesPerBuffer;
    const uint32 SampleCount = FMath::Min<uint32>(OptimalSampleCount, SamplesNeeded);
    
    // If we're not playing, fill the buffer with zeros for silence.
    if (!bPlayingSound)
    {
        const uint32 ByteCount = SampleCount * SAMPLE_SIZE;
        Buffer.resize(ByteCount);
        FMemory::Memset(&Buffer[0], 0, ByteCount);
        Wave->QueueAudio(&Buffer[0], ByteCount);
        return;
    }
    
    uint32 Fraction = 1; // Fraction of a second to play.
    float Frequency = 1046.5;
    uint32 TotalSampleCount = SampleRate / Fraction;
    
    Buffer.resize(SampleCount * SAMPLE_SIZE);
    int16* data = (int16*) &Buffer[0];
    
    // Generate a sine wave which slowly fades away.
    for (size_t i = 0; i < SampleCount; ++i)
    {
        float x = (float)(i + PlayCursor) / SampleRate;
        float v = (float)(TotalSampleCount - i - PlayCursor) / TotalSampleCount;
        data[i] = sin(Frequency * x * 2.f * PI) * v * MAX_int16;
    }
    
    PlayCursor += SampleCount;
    
    if (PlayCursor >= TotalSampleCount)
    {
        bPlayingSound = false;
    }
    
    SoundStream->QueueAudio((const uint8*)data, SampleCount * SAMPLE_SIZE);
}


