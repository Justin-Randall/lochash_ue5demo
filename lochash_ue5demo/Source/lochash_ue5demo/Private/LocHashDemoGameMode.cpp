// Fill out your copyright notice in the Description page of Project Settings.


#include "LocHashDemoGameMode.h"
#include "LocHashDemoPlayerController.h"
#include "BouncingBall.h"
#include "LocHashDemoHUD.h"

ALocHashDemoGameMode::ALocHashDemoGameMode()
{
	PlayerControllerClass = ALocHashDemoPlayerController::StaticClass();

	// Set the HUD class to our custom HUD
	HUDClass = ALocHashDemoHUD::StaticClass();

	// can tick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bUseLocationHash = true;
}

void ALocHashDemoGameMode::BeginPlay()
{
	Super::BeginPlay();
	// Spawn the bounding box actor
	FActorSpawnParameters SpawnParams;
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);

	SpawnBalls();
}

void ALocHashDemoGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bUseLocationHash)
	{
		if (bDebugDraw && bDrawHashBoxes)
		{
			const auto HalfHashPrecision = HashPrecision * 0.5f;
			for (const auto& CenterPoint : QuantizedBucketCoordinates)
			{
				// Each point in the bucket is the center of a bounding cube representing
				//  bucket in the hash table. We will draw a bounding box around each.
				// The box extents are HashPrecision/2 in each direction.
				FVector BoxExtent = FVector(HalfHashPrecision);
				FVector BoxLocation = FVector(CenterPoint.X + HalfHashPrecision, CenterPoint.Y + HalfHashPrecision, CenterPoint.Z + HalfHashPrecision);
				DrawDebugBox(GetWorld(), BoxLocation, BoxExtent, FColor::Green, false, -1, 0, 10.0f);
			}
		}

		QuantizedBucketCoordinates.Empty();
	}
	CollisionChecks = 0;
	CollisionDetections = 0;
}

void ALocHashDemoGameMode::SpawnBalls()
{
	const int NumBalls = 50;
	const float MaxVelocity = 250.0f;
	float HalfBoxSize = BoxSize * 0.5f;

	for (int32 i = 0; i < NumBalls; ++i)
	{
		// Generate a random location and velocity
		FVector SpawnLocation = FVector(FMath::RandRange(-HalfBoxSize, HalfBoxSize), FMath::RandRange(-HalfBoxSize, HalfBoxSize), FMath::RandRange(-HalfBoxSize, HalfBoxSize));
		FVector SpawnVelocity = FVector(FMath::RandRange(-MaxVelocity, MaxVelocity), FMath::RandRange(-MaxVelocity, MaxVelocity), FMath::RandRange(-MaxVelocity, MaxVelocity));

		// Spawn the ball
		FActorSpawnParameters SpawnParams;
		ABouncingBall* Ball = GetWorld()->SpawnActor<ABouncingBall>(ABouncingBall::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (Ball)
		{
			Ball->InitializeBall(*this, SpawnVelocity, LocHash);
			Balls.Add(Ball);
		}
	}
}

void ALocHashDemoGameMode::AddQuantizedBucketCoordinates(const TArray<FVector>& QuantizedCoordinateList)
{
	if (!bUseLocationHash)
		return;

	for (const FVector& QuantizedBucketCoordinate : QuantizedCoordinateList)
	{
		QuantizedBucketCoordinates.Add(QuantizedBucketCoordinate);
	}
}

void ALocHashDemoGameMode::Reset()
{
	LocHash.clear();
	CollisionChecks = 0;
	for (ABouncingBall* Ball : Balls)
	{
		if (Ball)
		{
			Ball->Destroy();
		}
	}
	Balls.Empty();
	bUseLocationHash = true;
	bDebugDraw = false;
	bDrawHashBoxes = false;

	SpawnBalls();
}

void ALocHashDemoGameMode::ToggleUseLocationHash()
{
	bUseLocationHash = !bUseLocationHash;
	if (!bUseLocationHash)
	{
		LocHash.clear();
	}
}

void ALocHashDemoGameMode::ToggleDebugDraw()
{
	bDebugDraw = !bDebugDraw;
}

void ALocHashDemoGameMode::ToggleDrawHashBoxes()
{
	bDrawHashBoxes = !bDrawHashBoxes;
}
