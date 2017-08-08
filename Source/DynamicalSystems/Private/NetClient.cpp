#include "NetClient.h"
#include "NetAvatar.h"
#include "NetRigidBody.h"
#include "RustyDynamics.h"
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
    
    MappedClients.Empty();
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
    LastPingTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    LastBodyTime = LastPingTime;
    if (Client == NULL) {
        const char* addr = std::string("127.0.0.1:8080").c_str();
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
    
    float CurrentPingTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    float CurrentBodyTime = CurrentPingTime;
    
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
            RebuildConsensus();
            return;
        }
    }
    
    {
        TArray<FString> DeleteList;
        for (auto& Elem : NetClients) {
            if (Elem.Value > 0 && Elem.Value < CurrentPingTime) {
                DeleteList.Add(Elem.Key);
            }
        }
        for (auto& Key : DeleteList) {
            NetClients.Remove(Key);
        }
        if (DeleteList.Num() > 0) {
            RebuildConsensus();
            return;
        }
    }
    
    static char Msg[512];
    
    if (CurrentPingTime > LastPingTime + 1) {
        Msg[0] = 0; // Ping
        strncpy(&Msg[1], TCHAR_TO_UTF8(*Uuid), 36);
        rd_netclient_msg_push(Client, Msg, 37);
        LastPingTime = CurrentPingTime;
    }
    
    if (CurrentBodyTime > LastBodyTime + 0.1) {
        TArray<RigidBodyPack> BodyPacks;
        for (int Idx=0; Idx<NetRigidBodies.Num(); ++Idx) {
            UNetRigidBody* Body = NetRigidBodies[Idx];
            if (IsValid(Body) && MappedClients.Num() > Body->NetOwner && MappedClients[Body->NetOwner] == this->Uuid) {
                AActor* Actor = Body->GetOwner();
                if (IsValid(Actor)) {
                    FVector LinearVelocity;
                    FVector Location = Actor->GetActorLocation();
                    UStaticMeshComponent* StaticMesh = Actor->FindComponentByClass<UStaticMeshComponent>();
                    if (StaticMesh) {
                        LinearVelocity = StaticMesh->GetBodyInstance()->GetUnrealWorldVelocity();
                        RigidBodyPack pack = {(uint8_t)Body->NetID,
                            Location.X, Location.Y, Location.Z, 1,
                            LinearVelocity.X, LinearVelocity.Y, LinearVelocity.Z, 0,
                        };
                        BodyPacks.Add(pack);
                    }
                }
            }
        }
        
        RustVec WorldRigidBodies;
        WorldRigidBodies.vec_ptr = BodyPacks.Num() > 0 ? (uint64_t)&BodyPacks[0] : 0;
        WorldRigidBodies.vec_cap = BodyPacks.Num() * 2;
        WorldRigidBodies.vec_len = BodyPacks.Num();
        WorldPack WorldPack;
        WorldPack.rigidbodies = WorldRigidBodies;
		if (IsValid(Avatar)) {
			WorldPack.avatar.id = Avatar->NetID;
			FVector Location = Avatar->GetOwner()->GetActorLocation();
			FQuat Rotation = Avatar->GetOwner()->GetActorRotation().Quaternion();
			WorldPack.avatar.px = Location.X;
			WorldPack.avatar.py = Location.Y;
			WorldPack.avatar.pz = Location.Z;
			WorldPack.avatar.pw = 1;
			WorldPack.avatar.rx = Rotation.X;
			WorldPack.avatar.ry = Rotation.Y;
			WorldPack.avatar.rz = Rotation.Z;
			WorldPack.avatar.rw = Rotation.W;
		}
        rd_netclient_push_world(Client, &WorldPack);

        LastBodyTime = CurrentBodyTime;
    }
    
    uint32_t size = rd_netclient_msg_pop(Client, Msg);
    if (size > 0) {
        if (Msg[0] == 0) { // Ping
            char UuidStr[37];
            strncpy(UuidStr, &Msg[1], 36);
            UuidStr[36] = 0;
            FString Key(UuidStr);
            NetClients.Add(Key, CurrentPingTime + 5);
            RebuildConsensus();
        }
        else if (Msg[0] == 1) { // World
            WorldPack* WorldPack = rd_netclient_dec_world(&Msg[1], size - 1);
            uint64_t NumOfBodies = WorldPack->rigidbodies.vec_len;
            RigidBodyPack* Bodies = (RigidBodyPack*)WorldPack->rigidbodies.vec_ptr;
            for (auto Idx=0; Idx<NumOfBodies; ++Idx) {
                FVector Location(Bodies[Idx].px, -Bodies[Idx].py, Bodies[Idx].pz);
                FVector LinearVelocity(Bodies[Idx].lx, -Bodies[Idx].ly, Bodies[Idx].lz);
                uint32_t NetID = Bodies[Idx].id;
                UNetRigidBody** NetRigidBody = NetRigidBodies.FindByPredicate([NetID](const UNetRigidBody* Item) {
                    return IsValid(Item) && Item->NetID == NetID;
                });
                if (NetRigidBody != NULL && *NetRigidBody != NULL) {
                    (*NetRigidBody)->SyncTarget = true;
                    (*NetRigidBody)->TargetLocation = Location;
                    (*NetRigidBody)->TargetLinearVelocity = LinearVelocity;
                }
            }
			if (IsValid(Avatar)) {
				FVector Location(WorldPack->avatar.px, WorldPack->avatar.py, WorldPack->avatar.pz);
				FQuat Orientation(WorldPack->avatar.rx, WorldPack->avatar.ry, WorldPack->avatar.rz, WorldPack->avatar.rw);
				Avatar->GetOwner()->SetActorLocation(Location);
				Avatar->GetOwner()->SetActorRotation(Orientation);
			}
            rd_netclient_drop_world(WorldPack);
        }
    }    
}


