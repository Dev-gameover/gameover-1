// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "MainBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAMEOVER_API UMainBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()



public:
	FHttpModule* Http;

	UFUNCTION(BlueprintCallable, category = "http")
		static bool ServerStatus();
	UFUNCTION(BlueprintCallable, category = "http")
		static FString HttpPost();
	UFUNCTION(BlueprintCallable, category = "http")
		static FString HttpGet();

	static FString JsonEncode(TSharedPtr<FJsonObject> JsonObject);


protected:
	FString Apiurl = "http://api.loc/api.php";
};
