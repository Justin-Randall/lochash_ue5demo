// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BouncingBallLocationHash.h"
#include "LocHashDemoGameMode.generated.h"

class ABouncingBall;

/**
 *
 */
UCLASS()
class LOCHASH_UE5DEMO_API ALocHashDemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALocHashDemoGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	float GetBoxSize() const { return BoxSize; }
	bool GetUseLocationHash() const { return bUseLocationHash; }
	bool GetDebugDraw() const { return bDebugDraw; }
	size_t GetDebugLinesDrawn() const { return DebugLinesDrawn; }
	void AddDebugLineDrawn() { DebugLinesDrawn++; }
	bool GetDrawHashBoxes() const { return bDrawHashBoxes; }
	TArray<ABouncingBall*>& GetBalls() { return Balls; }
	void AddQuantizedBucketCoordinates(const TArray<UQuantizedCoordinate>& QuantizedBucketCoordinates);
	void AddCollisionChecks(size_t NumChecks) { CollisionChecks += NumChecks; }
	void AddCollisionDetections(size_t NumDetections) { CollisionDetections += NumDetections; }
	size_t GetCollisionChecks() const { return CollisionChecks; }
	size_t GetCollisionDetections() const { return CollisionDetections; }

	UFUNCTION()
	void Reset();

	UFUNCTION()
	void ToggleUseLocationHash();

	UFUNCTION()
	void ToggleDebugDraw();

	UFUNCTION()
	void ToggleDrawHashBoxes();

	UFUNCTION()
	void SpawnBalls(); // spawn 50 and FLocationHash must already be initialized

private:
	FLocationHash LocHash;
	TSet<UQuantizedCoordinate> QuantizedBucketCoordinates;
	TArray<ABouncingBall*> Balls;
	float BoxSize = 15000.0f;
	size_t CollisionChecks = 0;
	size_t CollisionDetections = 0;
	size_t DebugLinesDrawn = 0;
	bool bUseLocationHash = true;
	bool bDebugDraw = false;
	bool bDrawHashBoxes = false;
};
