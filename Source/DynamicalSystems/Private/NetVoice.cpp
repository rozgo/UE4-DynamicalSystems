#include "NetVoice.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetVoice::UNetVoice()
{
	PrimaryComponentTick.bCanEverTick = true;
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

void UNetVoice::Say(uint8* Bytes, uint32 Count)
{
	//if (!IsValid(SoundStream)) {
	//UE_LOG(LogTemp, Warning, TEXT("UNetVoice::Say Bytes: %i"), Count);
	SoundStream->QueueAudio(Bytes, Count);
	for (uint32 I = 0; I < Count; ++I) {
		if (Bytes[I] > SayActivity) {
			SayActivity += (Bytes[I] - SayActivity) * 1.0f;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("UNetVoice::Say %i %f"), GFrameCounter, SayActivity);
}

const size_t MaxBytes = 1024 * 100;
uint8 Buf[MaxBytes];

void UNetVoice::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	//UE_LOG(LogTemp, Warning, TEXT("UNetVoice::TickComponent %i"), GFrameCounter);
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (!IsValid(NetClient) || !VoiceCapture.IsValid()) return;
    
	Activity = 0;
	uint32 ActivityCount = 0;

    uint32 BytesAvailable = 0;
    EVoiceCaptureState::Type CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
    while (CaptureState == EVoiceCaptureState::Ok && BytesAvailable > 0)
    {
        uint32 BytesRead = 0;
        VoiceCapture->GetVoiceData(Buf, MaxBytes, BytesRead);
//        UE_LOG(LogTemp, Warning, TEXT("NetVoice BytesAvailable: %i BytesRead: %i"), BytesAvailable, BytesRead);
        NetClient->Say(Buf, BytesRead);
        CaptureState = VoiceCapture->GetCaptureState(BytesAvailable);
		for (uint32 I = 0; I < BytesRead; ++I) {
			if (Buf[I] > Activity) {
				Activity += (Buf[I] - Activity) * 1.0f;
			}
		}
    }

	Activity = fmaxf(Activity, SayActivity);
	//UE_LOG(LogTemp, Warning, TEXT("UNetVoice::Activity %u"), Activity);

	SayActivity = 0;
}



