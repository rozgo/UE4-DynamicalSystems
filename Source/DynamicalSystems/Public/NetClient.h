#pragma once

#include "CoreMinimal.h"
#include "NetClient.generated.h"

class UNetRigidBody;
class UNetAvatar;

UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API ANetClient : public AActor
{
	GENERATED_BODY()
    
    void* Client = NULL;
    
    float LastPingTime;
    float LastBodyTime;
    
    void RebuildConsensus();

public:	

	ANetClient();

protected:
	virtual void BeginPlay() override;
    virtual void BeginDestroy() override;

public:
    virtual void Tick(float DeltaTime) override;
    
    void RegisterRigidBody(UNetRigidBody* RigidBody);
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
    FString Uuid;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
    TArray<UNetRigidBody*> NetRigidBodies;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
    TMap<FString, float> NetClients;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
    TArray<FString> MappedClients;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
	UNetAvatar* Avatar;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
    bool ConsensusReached = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
    FColor ChosenColor;
};
