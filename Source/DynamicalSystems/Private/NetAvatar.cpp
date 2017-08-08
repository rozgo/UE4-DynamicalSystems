#include "NetAvatar.h"
#include "DynamicalSystemsPrivatePCH.h"

void UNetAvatar::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (IsValid(NetClient)) {
		NetClient->Avatar = this;
	}
}

