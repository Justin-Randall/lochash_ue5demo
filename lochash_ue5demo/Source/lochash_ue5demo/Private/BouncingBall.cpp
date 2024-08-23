// Fill out your copyright notice in the Description page of Project Settings.


#include "BouncingBall.h"
#include "LocHashDemoGameMode.h"
#include "Engine/World.h"
#include "Components/LineBatchComponent.h"

// Sets default values
ABouncingBall::ABouncingBall()
{
	// Set this actor to call Tick() every frame.  
	// TODO : May want to change this to false if we want to control the ball's movement in a different way
	PrimaryActorTick.bCanEverTick = true;

	// Create the ball mesh component
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;

	// Load the sphere mesh from the starter content
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Game/StarterContent/Shapes/Sphere_LocHash"));
	if (!SphereMesh.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("Could not load sphere mesh"));
		return; // this should be FATAL
	}

	BallMesh->SetStaticMesh(SphereMesh.Object);
}

// Called when the game starts or when spawned
void ABouncingBall::BeginPlay()
{
	Super::BeginPlay();

	if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		RootComp->SetSimulatePhysics(false);
		RootComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called every frame
void ABouncingBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GameMode)
	{
		return;
	}

	// Update the ball's position based on its velocity
	FVector NewLocation = GetActorLocation() + (BallVelocity * DeltaTime);
	float BoxSize = GameMode->GetBoxSize() * 0.5f;


	// Function to calculate reflection vector
	auto Reflect = [](const FVector& Velocity, const FVector& Normal) -> FVector {
		return Velocity - 2 * FVector::DotProduct(Velocity, Normal) * Normal;
		};

	// Iteration: check for bounding area
	// and reverse the velocity if the ball hits the wall. Use angle of incidence = angle of reflection on any wall
	// e.g. calculate the normal of the wall and reflect the velocity of the ball off of the normal
	if (NewLocation.X > BoxSize || NewLocation.X < -BoxSize)
	{
		FVector Normal = FVector(1, 0, 0);
		if (NewLocation.X < -BoxSize) Normal = -Normal;
		BallVelocity = Reflect(BallVelocity, Normal);
		NewLocation.X = FMath::Clamp(NewLocation.X, -BoxSize, BoxSize) - FMath::Sign(BallVelocity.X) * 0.1f;
	}
	if (NewLocation.Y > BoxSize || NewLocation.Y < -BoxSize)
	{
		FVector Normal = FVector(0, 1, 0);
		if (NewLocation.Y < -BoxSize) Normal = -Normal;
		BallVelocity = Reflect(BallVelocity, Normal);
		NewLocation.Y = FMath::Clamp(NewLocation.Y, -BoxSize, BoxSize) - FMath::Sign(BallVelocity.Y) * 0.1f;
	}
	if (NewLocation.Z > BoxSize || NewLocation.Z < -BoxSize)
	{
		FVector Normal = FVector(0, 0, 1);
		if (NewLocation.Z < -BoxSize) Normal = -Normal;
		BallVelocity = Reflect(BallVelocity, Normal);
		NewLocation.Z = FMath::Clamp(NewLocation.Z, -BoxSize, BoxSize) - FMath::Sign(BallVelocity.Z) * 0.1f;
	}


	bool UseLocationHash = GameMode->GetUseLocationHash();

	if (UseLocationHash && LocationHash)
	{
		const auto MyRadius = GetRootComponent()->Bounds.GetSphere().W;

		// Query the location hash for other Bouncing Balls to see if there are collisions
		const auto& PotentiallyInteractingObjects = LocationHash->Query(LocationHashKeys);
		for (auto Object : PotentiallyInteractingObjects)
		{
			if (Object == this)
			{
				continue;
			}

			ABouncingBall* OtherBall = Cast<ABouncingBall>(Object);
			if (!OtherBall)
			{
				continue;
			}

			CollideWithOtherBall(*OtherBall, NewLocation);
		}

		NewLocation = GetActorLocation() + (BallVelocity * DeltaTime);
		LocationHashKeys = LocationHash->Move(*this, NewLocation);

		// for debug drawing
		// Let the game mode know:
		GameMode->AddQuantizedBucketCoordinates(LocationHashKeys);
	}
	else
	{
#if 1
		// demo the n^2 collision detection
		const auto& AllBouncingBalls = GameMode->GetBalls();
		for (auto OtherBall : AllBouncingBalls)
		{
			if (OtherBall == this)
			{
				continue;
			}

			if (!OtherBall)
			{
				continue;
			}

			const auto OtherLocation = OtherBall->GetActorLocation();
			const auto Distance = FVector::Dist(NewLocation, OtherLocation);
			const auto MyRadius = GetRootComponent()->Bounds.GetSphere().W;
			const auto OtherRadius = OtherBall->GetRootComponent()->Bounds.GetSphere().W;

			CollideWithOtherBall(*OtherBall, NewLocation);
		}
#endif
	}
	SetActorLocation(NewLocation);
}

void ABouncingBall::CollideWithOtherBall(ABouncingBall& OtherBall, FVector& NewLocation)
{
	if (&OtherBall == this)
		return;

	if (!GameMode)
		return;

	// Calculate the normal of the collision
	const auto MyLocation = GetActorLocation();
	const auto OtherLocation = OtherBall.GetActorLocation();
	const auto MyRadius = GetRootComponent()->Bounds.GetSphere().W;
	const auto OtherRadius = OtherBall.GetRootComponent()->Bounds.GetSphere().W;
	const auto Distance = FVector::Dist(NewLocation, OtherLocation);

	// Demonstrate the number of collision checks
	GameMode->AddCollisionChecks(1);
	if (GameMode->GetDebugDraw())
	{
		// debugging weird collisions after switching modes
		//const auto useLocationHash = GameMode->GetUseLocationHash();
		//if (useLocationHash)
		//{
		//	check(useLocationHash && Distance * 0.5 <= HashPrecision);
		//}

		bool shouldDraw = (GameMode->GetDebugLinesDrawn() < 500);
		if (!shouldDraw)
		{
			// 1 in 10 chance it should draw anyway
			shouldDraw = FMath::RandRange(0, 9) == 0;
		}
		if (shouldDraw)
		{
			UWorld* World = GetWorld();
			World->PersistentLineBatcher->DrawLine(MyLocation, OtherLocation, FLinearColor::Red, 0, 3.0f, 0.1f);
			GameMode->AddDebugLineDrawn();
		}
	}

	if (Distance <= MyRadius + OtherRadius)
	{
		const auto Normal = (MyLocation - OtherLocation).GetSafeNormal();

		// Reflect the velocity of both balls off of the normal
		BallVelocity = FMath::GetReflectionVector(BallVelocity, Normal);
		OtherBall.BallVelocity = FMath::GetReflectionVector(OtherBall.BallVelocity, -Normal);
		GameMode->AddCollisionDetections(1);
	}
}

// Initialize the ball with a given velocity
void ABouncingBall::InitializeBall(ALocHashDemoGameMode& DemoGameMode, const FVector& InitVelocity, FLocationHash& LocHash)
{
	GameMode = &DemoGameMode;
	BallVelocity = InitVelocity;
	LocationHash = &LocHash;

	// Add the ball to the location hash
	LocationHashKeys = LocHash.Add(*this);
}
