// Fill out your copyright notice in the Description page of Project Settings.


#include "LocHashDemoHUD.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "LocHashDemoPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "LocHashDemoGameMode.h"

void ALocHashDemoHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GWorld)
	{
		float FPS = 1.0f / GWorld->GetDeltaSeconds();
		FString FPSText = FString::Printf(TEXT("FPS: %.2f"), FPS);
		DrawText(FPSText, FColor::Green, 50, 50, nullptr, 1.5f);
	}

	if (ToggleUseLocationHashAction)
	{
		FString KeyBinding = GetKeyBindingForInputAction(*ToggleUseLocationHashAction);
		if (!KeyBinding.IsEmpty())
		{
			FString TextToggleUseLocationHashAction = FString::Printf(TEXT("Press %s to toggle location hash"), *KeyBinding);
			DrawText(TextToggleUseLocationHashAction, FColor::White, 50, 70, nullptr, 1.5f);
		}
	}

	if (ToggleDebugDrawAction)
	{
		FString KeyBinding = GetKeyBindingForInputAction(*ToggleDebugDrawAction);
		if (!KeyBinding.IsEmpty())
		{
			FString TextDebugDrawAction = FString::Printf(TEXT("Press %s to toggle debug drawing"), *KeyBinding);
			DrawText(TextDebugDrawAction, FColor::White, 50, 90, nullptr, 1.5f);
		}
	}

	if (ToggleDrawHashBoxesAction)
	{
		FString KeyBinding = GetKeyBindingForInputAction(*ToggleDrawHashBoxesAction);
		if (!KeyBinding.IsEmpty())
		{
			FString TextDebugDrawAction = FString::Printf(TEXT("Press %s to toggle drawing hash boxes when debug draw is active"), *KeyBinding);
			DrawText(TextDebugDrawAction, FColor::White, 50, 110, nullptr, 1.5f);
		}
	}

	if (SpawnBallsAction)
	{
		FString KeyBinding = GetKeyBindingForInputAction(*SpawnBallsAction);
		if (!KeyBinding.IsEmpty())
		{
			FString TextDebugDrawAction = FString::Printf(TEXT("Press %s to spawn more balls"), *KeyBinding);
			DrawText(TextDebugDrawAction, FColor::White, 50, 130, nullptr, 1.5f);
		}
	}

	if (ResetAction)
	{
		FString KeyBinding = GetKeyBindingForInputAction(*ResetAction);
		if (!KeyBinding.IsEmpty())
		{
			FString TextDebugDrawAction = FString::Printf(TEXT("Press %s to reset the demo"), *KeyBinding);
			DrawText(TextDebugDrawAction, FColor::White, 50, 150, nullptr, 1.5f);
		}
	}

	auto World = GetWorld()->GetAuthGameMode();
	if (World)
	{
		if (ALocHashDemoGameMode* GameMode = Cast<ALocHashDemoGameMode>(World))
		{
			FString Text = FString::Printf(TEXT("Location Hash: %s"), GameMode->GetUseLocationHash() ? TEXT("Enabled") : TEXT("Disabled"));
			DrawText(Text, FColor::White, 50, 170, nullptr, 1.5f);

			Text = FString::Printf(TEXT("Debug Draw: %s"), GameMode->GetDebugDraw() ? TEXT("Enabled") : TEXT("Disabled"));
			DrawText(Text, FColor::White, 50, 190, nullptr, 1.5f);

			Text = FString::Printf(TEXT("Draw Hash Boxes: %s"), GameMode->GetDrawHashBoxes() ? TEXT("Enabled") : TEXT("Disabled"));
			DrawText(Text, FColor::White, 50, 210, nullptr, 1.5f);

			Text = FString::Printf(TEXT("Number of Balls: %d"), GameMode->GetBalls().Num());
			DrawText(Text, FColor::White, 50, 230, nullptr, 1.5f);

			Text = FString::Printf(TEXT("Collision Checks: %d"), GameMode->GetCollisionChecks());
			DrawText(Text, FColor::White, 50, 250, nullptr, 1.5f);

			Text = FString::Printf(TEXT("Collision Detections: %d"), GameMode->GetCollisionDetections());
			DrawText(Text, FColor::White, 50, 270, nullptr, 1.5f);
		}
	}
}

FString ALocHashDemoHUD::GetKeyBindingForInputAction(const UInputAction& InputAction) const
{
	FString KeyBinding;

	if (const ALocHashDemoPlayerController* PlayerController = Cast<ALocHashDemoPlayerController>(GetOwningPlayerController()))
	{
		if (const UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			TArray<FKey> Keys = Subsystem->QueryKeysMappedToAction(&InputAction);

			if (Keys.Num() > 0)
			{
				KeyBinding = Keys[0].GetDisplayName().ToString();
			}
		}
	}

	return KeyBinding;
}
