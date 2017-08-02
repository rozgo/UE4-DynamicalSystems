#include "NetRigidBody.h"
#include "DynamicalSystemsPrivatePCH.h"

UNetRigidBody::UNetRigidBody()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UNetRigidBody::BeginPlay()
{
	Super::BeginPlay();
    
    NetClient->RegisterRigidBody(this);
}

void UNetRigidBody::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
    
    AActor* Actor = GetOwner();
//    if (IsValid(Actor)) {
    
        FVector drawPos = Actor->GetActorLocation() + FVector(0, 0, 100);
        FColor drawColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f).ToFColor(true);
        float drawDuration = 0.0f;
        bool drawShadow = false;
        DrawDebugString(GEngine->GetWorldFromContextObject(this),
                        drawPos,
                        *FString::Printf(TEXT("%s[%d]"), TEXT("test"), 12345),
                        NULL, drawColor, drawDuration, drawShadow);

//    }
    
    DrawDebugSphere(
                    GetWorld(),
                    drawPos,
                    24, 
                    6,
                    FColor(255,0,0),
                    false,
                    0,
                    200
                    );
    
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

