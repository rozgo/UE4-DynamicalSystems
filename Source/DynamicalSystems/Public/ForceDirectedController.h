#pragma once

#include "CoreMinimal.h"
#include "PhysXIncludes.h"
#include "PhysicsPublic.h"
#include "Runtime/Engine/Private/PhysicsEngine/PhysXSupport.h"
#include "ForceDirectedController.generated.h"


UCLASS( ClassGroup=(DynamicalSystems), meta=(BlueprintSpawnableComponent) )
class DYNAMICALSYSTEMS_API UForceDirectedController : public UActorComponent
{
	GENERATED_BODY()

public:	

	UForceDirectedController();

	virtual void BeginPlay() override;
	
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ForceDirectedController")
    bool Root = false;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ForceDirectedController")
	float Repulsion = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ForceDirectedController")
	float Stiffness = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ForceDirectedController")
	float Length = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ForceDirectedController")
	TArray<AStaticMeshActor*> Edges;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ForceDirectedController")
	TArray<AStaticMeshActor*> Nodes;

	UFUNCTION(BlueprintCallable, Category = "ForceDirectedController")
	void DistributeNodes(float Radius);

private:

	FCalculateCustomPhysics OnCalculateSubstep;

	physx::PxRigidBody* PxRigidBody;
	physx::PxRigidBody* GetPxRigidBody(AStaticMeshActor* Actor);

	void DrawEdges(AStaticMeshActor* Current);
	void GatherNodes(AStaticMeshActor* Current);
	void EdgeForce(AStaticMeshActor* Current);
	void SubstepTick(float DeltaTime, FBodyInstance* BodyInstance);
};
