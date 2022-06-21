// Fill out your copyright notice in the Description page of Project Settings.


#include "MainBlueprintFunctionLibrary.h"


bool UMainBlueprintFunctionLibrary::ServerStatus()
{
	return false;
}

FString UMainBlueprintFunctionLibrary::HttpPost()
{
	return FString();
}

FString UMainBlueprintFunctionLibrary::HttpGet()
{
	return FString();
}

FString UMainBlueprintFunctionLibrary::JsonEncode(TSharedPtr<FJsonObject> JsonObject)
{
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
	return JsonString;
}
