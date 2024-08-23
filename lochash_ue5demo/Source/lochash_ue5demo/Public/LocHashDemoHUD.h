#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InputAction.h"
#include "LocHashDemoHUD.generated.h"

UCLASS()
class LOCHASH_UE5DEMO_API ALocHashDemoHUD : public AHUD
{
	GENERATED_BODY()

public:
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

protected:
	virtual void DrawHUD() override;

private:
	FString GetKeyBindingForInputAction(const UInputAction& InputAction) const;
};
