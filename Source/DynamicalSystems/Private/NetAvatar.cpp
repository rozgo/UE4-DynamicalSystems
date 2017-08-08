#include "NetAvatar.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetAvatar::UNetAvatar()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetAvatar::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (IsValid(NetClient)) {
		NetClient->Avatar = this;
	}
}

