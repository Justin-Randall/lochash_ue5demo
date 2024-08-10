// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BouncingBallLocationHash.h"
#include "BouncingBall.generated.h"

class ALocHashDemoGameMode;

UCLASS()
class LOCHASH_UE5DEMO_API ABouncingBall : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABouncingBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CollideWithOtherBall(ABouncingBall& OtherBall, FVector& NewLocation);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ClearLocationHashKeys() { LocationHashKeys.Empty(); }

	// Ball mesh component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BallMesh;

	FVector BallVelocity;

	void InitializeBall(ALocHashDemoGameMode& DemoGameMode, const FVector& InitialVelocity, FLocationHash& LocationHash);

private:
	// Note: Due to UnrealEngine construction rules, this has to be a pointer, not a reference :(
	FLocationHash* LocationHash;
	ALocHashDemoGameMode* GameMode;
	// LocationHash keys for this ball
	// This is an optimization to reduce the number of queries to the LocationHash
	TArray<UQuantizedCoordinate> LocationHashKeys;
};
