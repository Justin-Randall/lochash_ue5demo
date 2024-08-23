// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "LocHashDemoPlayerController.generated.h"

/**
 *
 */
UCLASS()
class LOCHASH_UE5DEMO_API ALocHashDemoPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ALocHashDemoPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleUseLocationHashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleDebugDrawAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleDrawHashBoxesAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SpawnBallsAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ResetAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* GameplayInputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleQuitGameAction;

protected:
	virtual void BeginPlay() override;

	void ToggleUseLocationHash();
	void ToggleDebugDraw();
	void ToggleDrawHashBoxes();
	void SpawnBalls();
	void Reset();
	void QuitGame();
};
