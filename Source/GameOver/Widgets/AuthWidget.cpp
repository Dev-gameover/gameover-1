// Fill out your copyright notice in the Description page of Project Settings.


#include "AuthWidget.h"
#include "Components/Button.h"


bool UAuthWidget::Initialize()
{
	Super::Initialize();
	StartGame->OnClicked.AddDynamic(this, &UAuthWidget::StartButtonClicked);
	Http = &FHttpModule::Get();

	return true;
}
void UAuthWidget::Log(FString message)
{
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::White, message);
}

void UAuthWidget::MyHttpCall()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField("login", "saaydev");
	JsonObject->SetStringField("password", "12345678");
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL("http://api.loc/api.php");
	Request->SetVerb("POST");
	Request->SetContentAsString(JsonString);
	Request->SetHeader("Content-type", "application/json");
	Request->OnProcessRequestComplete().BindUObject(this, &UAuthWidget::OnResponseReceived);
	Request->ProcessRequest();

}

void UAuthWidget::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		int32 recievedInt = JsonObject->GetIntegerField("name");
		TSharedPtr<FJsonObject> body = JsonObject->GetObjectField("post");
		GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Green, FString::FromInt(recievedInt));
	}
	this->Log("Received http request");
}


void UAuthWidget::StartButtonClicked()
{
	FString login = "login";
	FString password = "password";
	FString api = "http://localhost/api.php";
	FString action = "register";

	this->MyHttpCall();
	
}
