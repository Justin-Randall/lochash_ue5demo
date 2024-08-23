// Fill out your copyright notice in the Description page of Project Settings.


#include "LocHashDemoPlayerController.h"
#include "LocHashDemoGameMode.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "LocHashDemoHUD.h"

ALocHashDemoPlayerController::ALocHashDemoPlayerController()
{
	// Load the input action and mapping context
	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleUseLocationHashActionObj(TEXT("/Game/Input/IA_ToggleUseLocationHash.IA_ToggleUseLocationHash"));
	if (ToggleUseLocationHashActionObj.Succeeded())
	{
		ToggleUseLocationHashAction = ToggleUseLocationHashActionObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleDebugDrawActionObj(TEXT("/Game/Input/IA_DebugDraw.IA_DebugDraw"));
	if (ToggleDebugDrawActionObj.Succeeded())
	{
		ToggleDebugDrawAction = ToggleDebugDrawActionObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleDrawHashBoxesActionObj(TEXT("/Game/Input/IA_DrawHashBoxes.IA_DrawHashBoxes"));
	if (ToggleDrawHashBoxesActionObj.Succeeded())
	{
		ToggleDrawHashBoxesAction = ToggleDrawHashBoxesActionObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> SpawnBallsActionObj(TEXT("/Game/Input/IA_SpawnBalls.IA_SpawnBalls"));
	if (SpawnBallsActionObj.Succeeded())
	{
		SpawnBallsAction = SpawnBallsActionObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ResetActionObj(TEXT("/Game/Input/IA_Reset.IA_Reset"));
	if (ResetActionObj.Succeeded())
	{
		ResetAction = ResetActionObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> GameplayInputMappingContextObj(TEXT("/Game/Input/IMC_GamePlay.IMC_GamePlay"));
	if (GameplayInputMappingContextObj.Succeeded())
	{
		GameplayInputMappingContext = GameplayInputMappingContextObj.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ToggleQuitGameActionObj(TEXT("/Game/Input/IA_QuitGame.IA_QuitGame"));
	if (ToggleQuitGameActionObj.Succeeded())
	{
		ToggleQuitGameAction = ToggleQuitGameActionObj.Object;
	}

}

void ALocHashDemoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(GameplayInputMappingContext, 1);
	}

	// Bind the input action
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleUseLocationHashAction, ETriggerEvent::Started, this, &ALocHashDemoPlayerController::ToggleUseLocationHash);
		EnhancedInputComponent->BindAction(ToggleDebugDrawAction, ETriggerEvent::Started, this, &ALocHashDemoPlayerController::ToggleDebugDraw);
		EnhancedInputComponent->BindAction(ToggleDrawHashBoxesAction, ETriggerEvent::Started, this, &ALocHashDemoPlayerController::ToggleDrawHashBoxes);
		EnhancedInputComponent->BindAction(SpawnBallsAction, ETriggerEvent::Started, this, &ALocHashDemoPlayerController::SpawnBalls);
		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Started, this, &ALocHashDemoPlayerController::Reset);
		EnhancedInputComponent->BindAction(ToggleQuitGameAction, ETriggerEvent::Started, this, &ALocHashDemoPlayerController::QuitGame);
	}

	if (ALocHashDemoHUD* HUD = Cast<ALocHashDemoHUD>(GetHUD()))
	{
		HUD->ToggleUseLocationHashAction = ToggleUseLocationHashAction;
		HUD->ToggleDebugDrawAction = ToggleDebugDrawAction;
		HUD->ToggleDrawHashBoxesAction = ToggleDrawHashBoxesAction;
		HUD->SpawnBallsAction = SpawnBallsAction;
		HUD->ResetAction = ResetAction;
	}
}

void ALocHashDemoPlayerController::ToggleUseLocationHash()
{
	// Get the game mode
	ALocHashDemoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALocHashDemoGameMode>();
	if (GameMode)
	{
		GameMode->ToggleUseLocationHash();
	}
}

void ALocHashDemoPlayerController::ToggleDebugDraw()
{
	// Get the game mode
	ALocHashDemoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALocHashDemoGameMode>();
	if (GameMode)
	{
		GameMode->ToggleDebugDraw();
	}
}

void ALocHashDemoPlayerController::ToggleDrawHashBoxes()
{
	ALocHashDemoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALocHashDemoGameMode>();
	if (GameMode)
	{
		GameMode->ToggleDrawHashBoxes();
	}
}

void ALocHashDemoPlayerController::SpawnBalls()
{
	ALocHashDemoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALocHashDemoGameMode>();
	if (GameMode)
	{
		GameMode->SpawnBalls();
	}
}

void ALocHashDemoPlayerController::Reset()
{
	ALocHashDemoGameMode* GameMode = GetWorld()->GetAuthGameMode<ALocHashDemoGameMode>();
	if (GameMode)
	{
		GameMode->Reset();
	}
}

void ALocHashDemoPlayerController::QuitGame()
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}
