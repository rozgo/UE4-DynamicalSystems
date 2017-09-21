#include "NetAvatar.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetAvatar::UNetAvatar()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetAvatar::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ANetClient> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		NetClient = *ActorItr;
		break;
	}

	LastUpdateTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	if (IsNetProxy) {
		NetClient->RegisterAvatar(this);
	}
	else if (IsValid(NetClient)) {
		NetClient->Avatar = this;
	}
}

void UNetAvatar::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	if (!IsNetProxy && IsValid(NetClient)) {
        NetID = 200 + NetClient->NetIndex;
		LastUpdateTime = CurrentTime;
	}

	if (CurrentTime > LastUpdateTime + 1) {
		AController* Controller = Cast<AController>(GetOwner());
		if (IsValid(Controller)) {
			APawn* Pawn = Controller->GetPawn();
			if (IsValid(Pawn)) {
				Pawn->Destroy();
			}
			Controller->Destroy();
		}
	}
}

