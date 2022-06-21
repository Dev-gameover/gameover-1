#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameOverGameModeBase.generated.h"

UCLASS()
class GAMEOVER_API AGameOverGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AGameOverGameModeBase();

	virtual void Tick(float DeltaTime) override;
};
