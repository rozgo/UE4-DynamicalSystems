#include "NetVoice.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetVoice::UNetVoice()
{
	PrimaryComponentTick.bCanEverTick = true;
	SampleBuffer = new TCircularBuffer<int16>(16384);
}

void UNetVoice::BeginPlay()
{
	Super::BeginPlay();

	if (NetClient == NULL) {
		for (TActorIterator<ANetClient> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			NetClient = *ActorItr;
			break;
		}
	}

	if (!IsValid(NetClient)) {
		return;
	}
    
    NetClient->RegisterVoice(this);
    
    VoiceCapture = FVoiceModule::Get().CreateVoiceCapture();
	if (VoiceCapture.IsValid()) {
		VoiceCapture->Start();
	}

    SampleRate                        = 16000;

	SoundStream = NewObject<USoundWaveProcedural>();
    
    SoundStream->SampleRate = SampleRate;
    SoundStream->NumChannels = 1;
    SoundStream->Duration = INDEFINITELY_LOOPING_DURATION;
    SoundStream->SoundGroup = SOUNDGROUP_Default;
    SoundStream->bLooping = false;
    
    UGameplayStatics::PlaySound2D(GetWorld(), SoundStream, 1, 1, 0);
}

void UNetVoice::BeginDestroy()
{
	Super::BeginDestroy();
}

uint16_t swap_uint16( uint16_t val ) 
{
	return (val << 8) | (val >> 8 );
}

void UNetVoice::Say(uint8* Bytes, uint32 Count)
{
	//if (!IsValid(SoundStream)) {
	//UE_LOG(LogTemp, Warning, TEXT("UNetVoice::Say Bytes: %i"), Count);
	for (uint32 I = 0; I < Count; I+=2) {
		uint16 Sample = *((uint16*)(Bytes + I));
		int16 Num = swap_uint16(Sample);
		SampleBufferIdx = SampleBuffer->GetNextIndex(SampleBufferIdx);
		(*SampleBuffer)[SampleBufferIdx] = Num;
	}
		
	//UE_LOG(LogTemp, Warning, TEXT("UNetVoice::Say %u"), SayActivity);

	SoundStream->QueueAudio(Bytes, Count);
}

const size_t MaxBytes = 1024 * 100;
uint8 Buf[MaxBytes];

void UNetVoice::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	//UE_LOG(LogTemp, Warning, TEXT("UNetVoice::TickComponent %i"), GFrameCounter);
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!IsValid(NetClient) || !VoiceCapture.IsValid()) return;
    
	Activity = 0;

    uint32 BytesAvailable = 0;
    EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
    while (CaptureState == EVoiceCaptureState::Ok && BytesAvailable > 0)
    {
        uint32 BytesRead = 0;
        VoiceCapture->GetVoiceData(Buf, MaxBytes, BytesRead);
//        UE_LOG(LogTemp, Warning, TEXT("NetVoice BytesAvailable: %i BytesRead: %i"), BytesAvailable, BytesRead);
        NetClient->Say(Buf, BytesRead);
        CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
		for (uint32 I = 0; I < BytesRead; I+=2) {
			uint16 Sample = *((uint16*)(Buf + I));
			int16 Num = swap_uint16(Sample);
			SampleBufferIdx = SampleBuffer->GetNextIndex(SampleBufferIdx);
			(*SampleBuffer)[SampleBufferIdx] = Num;
		}
    }

	for (uint32 I = 0; I < 16384; ++I) {
		Activity += abs((*SampleBuffer)[I]);
	}

	for (uint32 I = 0; I < 4096 * DeltaTime; ++I) {
		SampleBufferIdx = SampleBuffer->GetNextIndex(SampleBufferIdx);
		(*SampleBuffer)[SampleBufferIdx] = 0;
	}

	//Activity = Activity / 16384 + 1;

	UE_LOG(LogTemp, Warning, TEXT("UNetVoice::Activity %u"), Activity);
}



