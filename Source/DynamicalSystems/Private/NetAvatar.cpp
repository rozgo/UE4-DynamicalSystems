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
	else {
		NetClient->Avatar = this;
	}
}

void UNetAvatar::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	NetID = 200 + NetClient->NetIndex;

	float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	if (!IsNetProxy) {
		LastUpdateTime = CurrentTime;
	}

	if (CurrentTime > LastUpdateTime + 2) {
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

