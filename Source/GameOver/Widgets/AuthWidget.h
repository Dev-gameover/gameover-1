// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "AuthWidget.generated.h"


UCLASS()
class GAMEOVER_API UAuthWidget : public UUserWidget
{
	GENERATED_BODY()
		virtual bool Initialize();
		UPROPERTY(meta = (BindWidget))
		class UButton* StartGame;

		UFUNCTION()
			void StartButtonClicked();
		UFUNCTION()
		void Log(FString message);
public:
	FHttpModule* Http;

	/* The actual HTTP call */
	UFUNCTION()
		void MyHttpCall();

	/*Assign this function to call when the GET request processes sucessfully*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
