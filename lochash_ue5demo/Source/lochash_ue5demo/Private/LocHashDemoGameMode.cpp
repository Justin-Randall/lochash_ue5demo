// Fill out your copyright notice in the Description page of Project Settings.


#include "LocHashDemoGameMode.h"
#include "LocHashDemoPlayerController.h"
#include "BouncingBall.h"
#include "LocHashDemoHUD.h"
#include "Engine/World.h"
#include "Components/LineBatchComponent.h"


void DrawPersistentBox(UWorld& World, const FVector& Center, const FVector& Extent, const FRotator& Rotation, const FColor& Color, float Thickness)
{
	FVector Vertex[8];
	FTransform Transform(Rotation, Center);
	Vertex[0] = Transform.TransformPosition(FVector(-Extent.X, -Extent.Y, -Extent.Z));
	Vertex[1] = Transform.TransformPosition(FVector(Extent.X, -Extent.Y, -Extent.Z));
	Vertex[2] = Transform.TransformPosition(FVector(Extent.X, Extent.Y, -Extent.Z));
	Vertex[3] = Transform.TransformPosition(FVector(-Extent.X, Extent.Y, -Extent.Z));
	Vertex[4] = Transform.TransformPosition(FVector(-Extent.X, -Extent.Y, Extent.Z));
	Vertex[5] = Transform.TransformPosition(FVector(Extent.X, -Extent.Y, Extent.Z));
	Vertex[6] = Transform.TransformPosition(FVector(Extent.X, Extent.Y, Extent.Z));
	Vertex[7] = Transform.TransformPosition(FVector(-Extent.X, Extent.Y, Extent.Z));

	// Batch the drawing of lines in a single function call
	const float LifeTime = 0.04f;
	World.PersistentLineBatcher->DrawBox(Center, Extent, Rotation.Quaternion(), Color, LifeTime, SDPG_World, Thickness);
}

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
		if (bDrawHashBoxes)
		{
			const auto HalfHashPrecision = HashPrecision * 0.5f;
			for (const auto& CenterPoint : QuantizedBucketCoordinates)
			{
				FVector BoxExtent = FVector(HalfHashPrecision);
				FVector BoxLocation = FVector(CenterPoint.quantized_[0] + HalfHashPrecision, CenterPoint.quantized_[1] + HalfHashPrecision, CenterPoint.quantized_[2] + HalfHashPrecision);
				DrawPersistentBox(*GetWorld(), BoxLocation, BoxExtent, FRotator::ZeroRotator, FColor::Green, 10.0f);
			}
		}

		QuantizedBucketCoordinates.Empty();
	}
	CollisionChecks = 0;
	CollisionDetections = 0;
	DebugLinesDrawn = 0;
}

void ALocHashDemoGameMode::SpawnBalls()
{
	const int NumBalls = 100;
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
			Ball->PrimaryActorTick.AddPrerequisite(this, this->PrimaryActorTick);
			Ball->InitializeBall(*this, SpawnVelocity, LocHash);
			Balls.Add(Ball);
		}
	}
}

void ALocHashDemoGameMode::AddQuantizedBucketCoordinates(const TArray<UQuantizedCoordinate>& QuantizedCoordinateList)
{
	if (!bUseLocationHash)
		return;

	for (const auto& QuantizedBucketCoordinate : QuantizedCoordinateList)
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
	LocHash.clear();
	if (bUseLocationHash)
	{
		for (ABouncingBall* Ball : Balls)
		{
			if (Ball)
			{
				Ball->ClearLocationHashKeys();
				LocHash.Add(*Ball);
			}
		}
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
