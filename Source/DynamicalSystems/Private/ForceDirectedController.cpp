#include "DynamicalSystems.h"
#include "DynamicalUtil.h"
// #include "ForceDirectedController.h"
#include <random>


UForceDirectedController::UForceDirectedController()
{
	PrimaryComponentTick.bCanEverTick = true;
	OnCalculateSubstep.BindUObject(this, &UForceDirectedController::SubstepTick);
}

void UForceDirectedController::DistributeNodes( float Radius )
{
	std::default_random_engine Generator;
	std::uniform_real_distribution<float> Distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
	AActor* Owner = GetOwner();
	if (Owner && Owner->IsA(AStaticMeshActor::StaticClass())) {
		AStaticMeshActor* Node = static_cast<AStaticMeshActor*>(Owner);
		FVector Center = Node->GetActorLocation();
		for (auto It = Edges.CreateIterator(); It; ++It) {
			AStaticMeshActor* NodeB = (*It);
			if (NodeB) {
				float Theta = 2 * PI * Distribution(Generator);
				float Phi = FMath::Acos(1 - 2 * Distribution(Generator));
				FVector Unit(FMath::Sin(Phi) * FMath::Cos(Theta), FMath::Sin(Phi) * FMath::Sin(Theta), FMath::Cos(Phi));
				FVector Position = Center + Unit * Radius;
				NodeB->SetActorLocation(Position);
			}
		}
	}
}

AStaticMeshActor* GetStaticMeshActor( UActorComponent* AC )
{
	AStaticMeshActor* Node = NULL;
	AActor* Owner = AC->GetOwner();
	if (Owner && Owner->IsA(AStaticMeshActor::StaticClass())) {
		Node = static_cast<AStaticMeshActor*>(Owner);
	}
	return Node;
}

void UForceDirectedController::BeginPlay()
{
	Super::BeginPlay();		
	AStaticMeshActor* Node = GetStaticMeshActor(this);
	if (Node) {
		UStaticMeshComponent* SMC = Node->GetStaticMeshComponent();
		if (SMC) {
			PxRigidBody = SMC->GetBodyInstance()->GetPxRigidBody_AssumesLocked();
			if (Root) {
				Nodes.Empty();
				GatherNodes(Node);
			}
		}
	}
	//DistributeNodes(10);
}

//TODO: prevent infinite recursion
void UForceDirectedController::GatherNodes( AStaticMeshActor* Current )
{
	Nodes.Add(Current);
	UForceDirectedController* FDC = Current->FindComponentByClass<UForceDirectedController>();
	if (FDC) {
		for (auto It = FDC->Edges.CreateIterator(); It; ++It) {
			AStaticMeshActor* Node = (*It);
			if (Node) {
				GatherNodes(Node);
			}
		}
	}
}

//TODO: prevent infinite recursion
void UForceDirectedController::DrawEdges( AStaticMeshActor* NodeA )
{
	UForceDirectedController* FDC = NodeA->FindComponentByClass<UForceDirectedController>();
	if (FDC) {
		for (auto It = FDC->Edges.CreateIterator(); It; ++It) {
			AStaticMeshActor* NodeB = (*It);
			if (NodeB) {
				DrawDebugLine(GetWorld(), NodeA->GetActorLocation(), NodeB->GetActorLocation(), FColor::White);
			}
		}
	}
}

physx::PxRigidBody* UForceDirectedController::GetPxRigidBody( AStaticMeshActor* Actor ) {
	UForceDirectedController* FDC = Actor->FindComponentByClass<UForceDirectedController>();
	physx::PxRigidBody* RigidBody = NULL;
	if (FDC) {
		RigidBody = FDC->PxRigidBody;
	}
	return RigidBody;
}

FVector GetPxRigidBodyLocation( physx::PxRigidBody* RigidBody ) {
	physx::PxTransform PTransform = RigidBody->getGlobalPose();
	return FVector(PTransform.p.x, PTransform.p.y, PTransform.p.z);
}

//TODO: prevent infinite recursion
void UForceDirectedController::EdgeForce( AStaticMeshActor* NodeA )
{
	UForceDirectedController* FDC = NodeA->FindComponentByClass<UForceDirectedController>();
	if (FDC) {
		physx::PxRigidBody* RigidBodyA = GetPxRigidBody(NodeA);
		if (RigidBodyA) {
			FVector PositionA = GetPxRigidBodyLocation(RigidBodyA);
			for (auto It = FDC->Edges.CreateIterator(); It; ++It) {
				AStaticMeshActor* NodeB = *It;
				if (NodeB) {
					physx::PxRigidBody* RigidBodyB = GetPxRigidBody(NodeB);
					if (RigidBodyB) {
						FVector PositionB = GetPxRigidBodyLocation(RigidBodyB);
						FVector D = PositionB - PositionA;
						float Displacement = Length - D.Size();
						FVector Direction = D.GetSafeNormal();
						FVector ForceA = Direction * Stiffness * Displacement * -0.5;
						FVector ForceB = Direction * Stiffness * Displacement *  0.5;
						RigidBodyA->addForce(physx::PxVec3(ForceA.X, ForceA.Y, ForceA.Z), physx::PxForceMode::eFORCE);
						RigidBodyB->addForce(physx::PxVec3(ForceB.X, ForceB.Y, ForceB.Z), physx::PxForceMode::eFORCE);
						EdgeForce(NodeB);
					}
				}
			}
		}
	}
}

void UForceDirectedController::SubstepTick( float DeltaTime, FBodyInstance* BodyInstanceA )
{
	for (auto ItA = Nodes.CreateIterator(); ItA; ++ItA) {
		AStaticMeshActor* NodeA = *ItA;
		if (NodeA) {
			physx::PxRigidBody* RigidBodyA = GetPxRigidBody(NodeA);
			if (RigidBodyA) {
				FVector PositionA = GetPxRigidBodyLocation(RigidBodyA);
				for (auto ItB = Nodes.CreateIterator(); ItB; ++ItB) {
					AStaticMeshActor* NodeB = *ItB;
					if (NodeB && NodeA != NodeB) {
						physx::PxRigidBody* RigidBodyB = GetPxRigidBody(NodeB);
						if (RigidBodyB) {
							FVector PositionB = GetPxRigidBodyLocation(RigidBodyB);
							FVector D = PositionA - PositionB;
							float Distance = D.Size();
							FVector Direction = D.GetSafeNormal();
							FVector ForceA = (Direction * Repulsion) / (Distance * Distance *  0.5);
							FVector ForceB = (Direction * Repulsion) / (Distance * Distance * -0.5);
							RigidBodyA->addForce(physx::PxVec3(ForceA.X, ForceA.Y, ForceA.Z), physx::PxForceMode::eFORCE);
							RigidBodyB->addForce(physx::PxVec3(ForceB.X, ForceB.Y, ForceB.Z), physx::PxForceMode::eFORCE);
						}
					}
				}
			}
		}
	}
	AStaticMeshActor* Node = GetStaticMeshActor(this);
	if (Node) {
		EdgeForce(Node);
	}
}

void UForceDirectedController::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	AStaticMeshActor* Node = GetStaticMeshActor(this);
	if (Node) {
		Node->GetStaticMeshComponent()->GetBodyInstance()->AddCustomPhysics(OnCalculateSubstep);
		DrawEdges(Node);
	}
}

