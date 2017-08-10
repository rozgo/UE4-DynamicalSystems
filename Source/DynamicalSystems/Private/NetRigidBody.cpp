#include "NetRigidBody.h"
#include "RigidBodyController.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetRigidBody::UNetRigidBody()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetRigidBody::BeginPlay()
{
	Super::BeginPlay();
    
//AActor* Actor = GetOwner();
//if (IsValid(Actor)) {
//    TargetLocation = Actor->GetActorLocation();
//    UStaticMeshComponent* StaticMesh = Actor->FindComponentByClass<UStaticMeshComponent>();
//    if (StaticMesh) {
//        TargetLinearVelocity = StaticMesh->GetBodyInstance()->GetUnrealWorldVelocity();
//    }
//}

	AActor* Actor = GetOwner();
	TargetLocation = Actor->GetActorLocation();
	URigidBodyController* PIDController = Actor->FindComponentByClass<URigidBodyController>();
	if (PIDController && NetOwner != NetClient->NetIndex) {
		PIDController->TargetLocation = TargetLocation;
		//PIDController->TargetRotation = TargetRotation;
	}
    
    NetClient->RegisterRigidBody(this);
}

void UNetRigidBody::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	AActor* Actor = GetOwner();
	URigidBodyController* PIDController = Actor->FindComponentByClass<URigidBodyController>();
	if (PIDController && NetOwner != NetClient->NetIndex) {
		PIDController->Enabled = true;
		PIDController->TargetLocation = TargetLocation + TargetLinearVelocity * DeltaTime;
		//PIDController->TargetRotation = TargetRotation;
	}
	else {
		PIDController->Enabled = false;
	}
    
//AActor* Actor = GetOwner();
//if (SyncTarget && IsValid(Actor)) {
//    SyncTarget = false;
//	FVector CurrentLocation = Actor->GetActorLocation();
//	Actor->SetActorLocation(FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 20));
//    UStaticMeshComponent* StaticMesh = Actor->FindComponentByClass<UStaticMeshComponent>();
//    if (StaticMesh) {
//        StaticMesh->GetBodyInstance()->SetLinearVelocity(TargetLinearVelocity, false);
//    }
//}
    
    
    
    {
    
////    if (IsValid(Actor)) {
//    
////        FVector drawPos = Actor->GetActorLocation() + FVector(0, 0, 100);
//        FColor drawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
//        float drawDuration = 0.0f;
//        bool drawShadow = false;
//        DrawDebugString(GEngine->GetWorldFromContextObject(this),
//                        drawPos,
//                        *FString::Printf(TEXT("%s[%d]"), TEXT("test"), 12345),
//                        NULL, drawColor, drawDuration, drawShadow);
//
////    }
    
    {
    FRandomStream Rnd(NetID);
    
    DrawDebugSphere(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100), 20, 6,
                    FColor(Rnd.RandRange(0, 255),Rnd.RandRange(0, 255),Rnd.RandRange(0, 255)),
                    false, 0, 200);
    }
    
    {
    FRandomStream Rnd(NetOwner);
    
    DrawDebugPoint(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 150), 5,
                    FColor(Rnd.RandRange(0, 255),Rnd.RandRange(0, 255),Rnd.RandRange(0, 255)),
                    false, 0, 200);
    }
    
//    DrawDebugPoint(
//                   GetWorld(),
//                   drawPos,
//                   20,  					//size
//                   FColor(255,0,255),  //pink
//                   false,  				//persistent (never goes away)
//                   0.03, 					//point leaves a trail on moving object
//                   200
//                   );
    }
}

