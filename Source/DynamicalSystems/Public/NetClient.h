#pragma once

#include "Engine.h"
#include "RustyDynamics.h"
#include "NetClient.generated.h"

UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API ANetClient : public AActor
{
	GENERATED_BODY()
    
    void* Client = NULL;
    
    float LastTime;

public:	

	ANetClient();

protected:
	virtual void BeginPlay() override;
    virtual void BeginDestroy() override;

public:
    virtual void Tick(float DeltaTime) override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetClient")
    FString Uuid;

};
