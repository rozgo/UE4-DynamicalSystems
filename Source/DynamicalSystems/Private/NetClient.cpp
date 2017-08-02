#include "NetClient.h"
#include "NetRigidBody.h"
#include "DynamicalSystemsPrivatePCH.h"

ANetClient::ANetClient()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ANetClient::RegisterRigidBody(UNetRigidBody* RigidBody)
{
    UE_LOG(LogTemp, Warning, TEXT("ANetClient::RegisterRigidBody %s"), *RigidBody->GetOwner()->GetName());
    NetRigidBodies.Add(RigidBody);
}

void ANetClient::RebuildConsensus()
{
    int Count = NetClients.Num();
    FRandomStream Rnd(Count);
    
    TArray<FString> MappedClients;
    NetClients.GetKeys(MappedClients);
    MappedClients.Sort();
    
    NetRigidBodies.Sort([](const UNetRigidBody& LHS, const UNetRigidBody& RHS) {
        return LHS.NetID > RHS.NetID; });
    for (auto It = NetRigidBodies.CreateConstIterator(); It; ++It) {
        (*It)->NetOwner = Rnd.RandRange(0, Count);
    }
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
        NetClients.Add(Uuid, -1);
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
    
    {
        TArray<int> DeleteList;
        for (int Idx=0; Idx<NetRigidBodies.Num(); ++Idx) {
            if (!IsValid(NetRigidBodies[Idx])) {
                DeleteList.Add(Idx);
                continue;
            }
        }
        for (int Idx=0; Idx<DeleteList.Num(); ++Idx) {
            NetRigidBodies.RemoveAt(DeleteList[Idx]);
        }
        if (DeleteList.Num() > 0) {
            return;
        }
    }
    
    {
        TArray<FString> DeleteList;
        for (auto& Elem : NetClients) {
            if (Elem.Value > 0 && Elem.Value < CurrentTime) {
                DeleteList.Add(Elem.Key);
            }
        }
        for (auto& Key : DeleteList) {
            NetClients.Remove(Key);
        }
        if (DeleteList.Num() > 0) {
            return;
        }
    }
    
    static char Msg[512];
    
    if (CurrentTime > LastTime + 1) {
        Msg[0] = 0; // Ping
        strncpy(&Msg[1], TCHAR_TO_UTF8(*Uuid), 36);
        rd_netclient_msg_push(Client, Msg, 37);
        LastTime = CurrentTime;
    }
    
    uint32_t size = rd_netclient_msg_pop(Client, Msg);
    if (size > 0) {
        if (Msg[0] == 0) { // Ping
            char UuidStr[37];
            strncpy(UuidStr, &Msg[1], 36);
            UuidStr[36] = 0;
            FString Key(UuidStr);
            NetClients.Add(Key, CurrentTime + 5);
            RebuildConsensus();
            UE_LOG(LogTemp, Warning, TEXT("Ping: %s"), *Key);
        }
    }
    
//    UE_LOG(LogTemp, Warning, TEXT("ANetClient::NetRigidBodies %i"), NetRigidBodies.Num());

    
}


