#pragma once

#include "CoreMinimal.h"
#include "NetClient.generated.h"

class UNetRigidBody;
class UNetAvatar;
class UNetVoice;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSystemFloatMsgDecl, int32, System, int32, Id, float, Value);

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
	void RegisterAvatar(UNetAvatar* Avatar);
    void RegisterVoice(UNetVoice* Voice);
    void Say(uint8* Bytes, uint32 Count);

	UFUNCTION(BlueprintCallable, Category="NetClient")
    void SendSystemFloat(int32 System, int32 Id, float Value);

	UPROPERTY(BlueprintAssignable)
	FSystemFloatMsgDecl OnSystemFloatMsg;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
    FString Local;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetClient")
	FString Server = "127.0.0.1:8080";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
	FString Uuid;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
	int NetIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
	TArray<UNetRigidBody*> NetRigidBodies;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
	TArray<UNetAvatar*> NetAvatars;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
    TArray<UNetVoice*> NetVoices;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
    TMap<FString, float> NetClients;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
    TArray<FString> MappedClients;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
	UNetAvatar* Avatar;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NetClient")
    bool ConsensusReached = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NetClient")
    FColor ChosenColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetClient")
	int MissingAvatar = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetClient|Debug")
	bool MirrorSyncY = false;
};
