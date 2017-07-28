#include "NetClient.h"
#include "DynamicalSystemsPrivatePCH.h"

ANetClient::ANetClient()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANetClient::BeginPlay()
{
    Super::BeginPlay();
    LastTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    if (Client == NULL) {
        const char* addr = std::string("127.shit.0.0:8080").c_str();
        Client = rd_netclient_open(addr);
        char uuid[64];
        rd_netclient_uuid(Client, uuid);
        Uuid = FString(uuid);
        UE_LOG(LogTemp, Warning, TEXT("NetClient OPEN %s"), *Uuid);
    }
}

void ANetClient::BeginDestroy()
{
    Super::BeginDestroy();
    if (Client != NULL) {
        rd_netclient_drop(Client);
        Client = NULL;
        UE_LOG(LogTemp, Warning, TEXT("NetClient DROP %s"), *Uuid);
    }
}

void ANetClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    if (CurrentTime > LastTime + 1) {
        const char* msg = std::string("NetClient::Tick\n").c_str();
        rd_netclient_msg_push(Client, msg, 15);
        LastTime = CurrentTime;
    }
    
    char msg[512];
    memset(msg, 0, 512);
    uint32_t size = rd_netclient_msg_pop(Client, msg);
    if (size > 0) {
        FString Msg(msg);
        UE_LOG(LogTemp, Warning, TEXT("Msg: %s"), *Msg);
    }
    
}
