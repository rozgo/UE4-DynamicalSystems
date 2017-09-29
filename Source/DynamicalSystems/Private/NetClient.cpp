#include "NetClient.h"
#include "NetAvatar.h"
#include "NetVoice.h"
#include "NetRigidBody.h"
#include "RustyDynamics.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
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

void ANetClient::RegisterAvatar(UNetAvatar* _Avatar)
{
    UE_LOG(LogTemp, Warning, TEXT("ANetClient::RegisterAvatar %s"), *_Avatar->GetOwner()->GetName());
    NetAvatars.Add(_Avatar);
}

void ANetClient::RegisterVoice(UNetVoice* Voice)
{
    UE_LOG(LogTemp, Warning, TEXT("ANetClient::RegisterVoice %s"), *Voice->GetOwner()->GetName());
    NetVoices.Add(Voice);
}

void ANetClient::Say(uint8* Bytes, uint32 Count)
{
    rd_netclient_vox_push(Client, Bytes, Count);
}

void ANetClient::RebuildConsensus()
{
    int Count = NetClients.Num();
    FRandomStream Rnd(Count);
    
    MappedClients.Empty();
    NetClients.GetKeys(MappedClients);
    MappedClients.Sort();

	NetIndex = MappedClients.IndexOfByKey(Uuid);
    
    NetRigidBodies.Sort([](const UNetRigidBody& LHS, const UNetRigidBody& RHS) {
        return LHS.NetID > RHS.NetID; });
    for (auto It = NetRigidBodies.CreateConstIterator(); It; ++It) {
        (*It)->NetOwner = Rnd.RandRange(0, Count);
    }
}

void ANetClient::BeginPlay()
{
    Super::BeginPlay();
    
    bool bCanBindAll;
    TSharedPtr<class FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
    Local = localIp->ToString(true);
    UE_LOG(LogTemp, Warning, TEXT("GetLocalHostAddr %s"), *Local);
    
    LastPingTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
    LastBodyTime = LastPingTime;
    if (Client == NULL) {
        Client = rd_netclient_open(TCHAR_TO_ANSI(*Local), TCHAR_TO_ANSI(*Server));
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
    float CurrentBodyTime = CurrentTime;
    
	for (int Idx=0; Idx<NetRigidBodies.Num();) {
		if (!IsValid(NetRigidBodies[Idx])) {
			NetRigidBodies.RemoveAt(Idx);
		}
		else {
			++Idx;
		}
	}

    for (int Idx=0; Idx<NetAvatars.Num();) {
        if (!IsValid(NetAvatars[Idx])) {
            NetAvatars.RemoveAt(Idx);
        }
        else {
            ++Idx;
        }
    }
    
    for (int Idx=0; Idx<NetVoices.Num();) {
        if (!IsValid(NetVoices[Idx])) {
            NetVoices.RemoveAt(Idx);
        }
        else {
            ++Idx;
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
    }
    
    if (CurrentTime > LastPingTime + 1) {
		uint8 Msg[128];
        Msg[0] = 0; // Ping
        strncpy(&((char*)Msg)[1], TCHAR_TO_UTF8(*Uuid), 36);
        rd_netclient_msg_push(Client, Msg, 37);
        LastPingTime = CurrentTime;
    }
    
    if (CurrentBodyTime > LastBodyTime + 0.1) {

		WorldPack WorldPack;
		memset(&WorldPack, 0, sizeof(WorldPack));

        TArray<RigidBodyPack> BodyPacks;
        for (int Idx=0; Idx<NetRigidBodies.Num(); ++Idx) {
            UNetRigidBody* Body = NetRigidBodies[Idx];
            if (IsValid(Body) && Body->NetOwner == NetIndex) {
                AActor* Actor = Body->GetOwner();
                if (IsValid(Actor)) {
                    FVector LinearVelocity;
                    FVector Location = Actor->GetActorLocation();
                    UStaticMeshComponent* StaticMesh = Actor->FindComponentByClass<UStaticMeshComponent>();
                    if (StaticMesh) {
                        LinearVelocity = StaticMesh->GetBodyInstance()->GetUnrealWorldVelocity();
                        RigidBodyPack Pack = {(uint8_t)Body->NetID,
                            Location.X, Location.Y, Location.Z, 1,
                            LinearVelocity.X, LinearVelocity.Y, LinearVelocity.Z, 0,
                        };
                        BodyPacks.Add(Pack);
                    }
                }
            }
        }
        
        RustVec WorldRigidBodies;
        WorldRigidBodies.vec_ptr = BodyPacks.Num() > 0 ? (uint64_t)&BodyPacks[0] : 0;
        WorldRigidBodies.vec_cap = BodyPacks.Num();
        WorldRigidBodies.vec_len = BodyPacks.Num();
        WorldPack.rigidbodies = WorldRigidBodies;

		TArray<AvatarPack> AvatarPacks;
		if (IsValid(Avatar) && !Avatar->IsNetProxy) {
			FVector Locations[] = { Avatar->Location, Avatar->LocationHMD, Avatar->LocationHandL, Avatar->LocationHandR };
			FQuat Rotations[] = { Avatar->Rotation.Quaternion(), Avatar->RotationHMD.Quaternion(), Avatar->RotationHandL.Quaternion(), Avatar->RotationHandR.Quaternion() };
			for (auto I = 0; I < 4; ++I) {
				AvatarPack Pack = { (uint8_t)Avatar->NetID,
					Locations[I].X, Locations[I].Y, Locations[I].Z, 1,
					Rotations[I].X, Rotations[I].Y, Rotations[I].Z, Rotations[I].W,
				};
				AvatarPacks.Add(Pack);
			}
			RustVec AvatarParts;
			AvatarParts.vec_ptr = (uint64_t)&AvatarPacks[0];
			AvatarParts.vec_cap = 4;
			AvatarParts.vec_len = 4;
			WorldPack.avatarparts = AvatarParts;
		}

        rd_netclient_push_world(Client, &WorldPack);
        LastBodyTime = CurrentBodyTime;
    }
    
    RustVec* RustMsg = rd_netclient_msg_pop(Client);
	uint8* Msg = (uint8*)RustMsg->vec_ptr;
    if (RustMsg->vec_len > 0) {

        if (Msg[0] == 0) { // Ping
            char UuidStr[37];
            strncpy(UuidStr, &((char*)Msg)[1], 36);
            UuidStr[36] = 0;
            FString Key(UuidStr);
            NetClients.Add(Key, CurrentTime + 5);
            RebuildConsensus();
        }
        else if (Msg[0] == 1) { // World
            WorldPack* WorldPack = rd_netclient_dec_world(&Msg[1], RustMsg->vec_len - 1);
            uint64_t NumOfBodies = WorldPack->rigidbodies.vec_len;
            RigidBodyPack* Bodies = (RigidBodyPack*)WorldPack->rigidbodies.vec_ptr;
            for (auto Idx=0; Idx<NumOfBodies; ++Idx) {
                FVector Location(Bodies[Idx].px, (MirrorSyncY ? -1 : 1) * Bodies[Idx].py, Bodies[Idx].pz);
                FVector LinearVelocity(Bodies[Idx].lx, (MirrorSyncY ? -1 : 1) * Bodies[Idx].ly, Bodies[Idx].lz);
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
            uint64_t NumOfParts = WorldPack->avatarparts.vec_len;
            if (NumOfParts >= 4) {
                AvatarPack* Parts = (AvatarPack*)WorldPack->avatarparts.vec_ptr;
                uint32_t NetID = Parts[0].id;
                UNetAvatar** NetAvatar = NetAvatars.FindByPredicate([NetID](const UNetAvatar* Item) {
                    return IsValid(Item) && Item->NetID == NetID;
                });
                if (NetAvatar != NULL && *NetAvatar != NULL) {
                    (*NetAvatar)->LastUpdateTime = CurrentTime;
                    (*NetAvatar)->Location = FVector(Parts[0].px, Parts[0].py, Parts[0].pz);
                    (*NetAvatar)->Rotation = FRotator(FQuat(Parts[0].rx, Parts[0].ry, Parts[0].rz, Parts[0].rw));
                    (*NetAvatar)->LocationHMD = FVector(Parts[1].px, Parts[1].py, Parts[1].pz);
                    (*NetAvatar)->RotationHMD = FRotator(FQuat(Parts[1].rx, Parts[1].ry, Parts[1].rz, Parts[1].rw));
                    (*NetAvatar)->LocationHandL = FVector(Parts[2].px, Parts[2].py, Parts[2].pz);
                    (*NetAvatar)->RotationHandL = FRotator(FQuat(Parts[2].rx, Parts[2].ry, Parts[2].rz, Parts[2].rw));
                    (*NetAvatar)->LocationHandR = FVector(Parts[3].px, Parts[3].py, Parts[3].pz);
                    (*NetAvatar)->RotationHandR = FRotator(FQuat(Parts[3].rx, Parts[3].ry, Parts[3].rz, Parts[3].rw));
                }
                else {
                    MissingAvatar = (int)NetID;
                }
            }
            rd_netclient_drop_world(WorldPack);
        }
		else if (Msg[0] == 10) {
			uint8 MsgSystem = Msg[1];
			uint8 MsgId = Msg[2];
			float* MsgValue = (float*)(Msg + 3);
			UE_LOG(LogTemp, Warning, TEXT("Msg IN MsgSystem: %u MsgId: %u MsgValue: %f"), Msg[1], Msg[2], *MsgValue);
			OnSystemFloatMsg.Broadcast(MsgSystem, MsgId, *MsgValue);
		}
    }
	rd_netclient_msg_drop(RustMsg);
    
    RustVec* RustVox = rd_netclient_vox_pop(Client);
    uint8* Vox = (uint8*)RustVox->vec_ptr;
    if (RustVox->vec_len > 0) {
        //UE_LOG(LogTemp, Warning, TEXT("VOX incoming %i"), RustVox->vec_len);
        for (int Idx=0; Idx<NetVoices.Num(); ++Idx) {
            UNetVoice* Voice = NetVoices[Idx];
            Voice->Say(Vox, RustVox->vec_len);
        }
    }
    rd_netclient_vox_drop(RustVox);
}

void ANetClient::SendSystemFloat(int32 System, int32 Id, float Value)
{
	uint8 Msg[7];
	Msg[0] = 10;
	Msg[1] = (uint8)System;
	Msg[2] = (uint8)Id;

	//TODO: byte order
	uint8* fbytes = (uint8*)(&Value);
	Msg[3] = fbytes[0];
	Msg[4] = fbytes[1];
	Msg[5] = fbytes[2];
	Msg[6] = fbytes[3];

	UE_LOG(LogTemp, Warning, TEXT("Msg OUT MsgSystem: %u MsgId: %u MsgValue: %f"), Msg[1], Msg[2], Value);
	rd_netclient_msg_push(Client, Msg, 7);
}

