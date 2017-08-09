#pragma once

#include "CoreMinimal.h"
#include "NetClient.h"
#include "NetAvatar.generated.h"

UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API UNetAvatar : public UActorComponent
{
	GENERATED_BODY()

public:

	UNetAvatar();

	virtual void BeginPlay() override;

	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	float LastUpdateTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	ANetClient* NetClient = NULL;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	int NetID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetAvatar")
	FRotator Rotation;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
	bool IsNetProxy = false;
};
