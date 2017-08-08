#pragma once

#include "CoreMinimal.h"
#include "NetClient.h"
#include "NetAvatar.generated.h"

UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API UNetAvatar : public UActorComponent
{
	GENERATED_BODY()

public:

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	ANetClient* NetClient = NULL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	int NetID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FVector LocationHMD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FRotator RotationHMD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FVector LocationHandL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FRotator RotationHandL;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FVector LocationHandR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FRotator RotationHandR;

};
