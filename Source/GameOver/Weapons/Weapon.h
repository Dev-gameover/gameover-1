// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"

#include "Weapon.generated.h"

UCLASS()
class GAMEOVER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame

	virtual void Tick(float DeltaTime) override;
	int AmmoMax;

	UFUNCTION()
		void Shot(bool IsAim);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		TSubclassOf<AActor> Projectile;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UParticleSystem* MuzzleFlash;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		class UParticleSystemComponent* ParticleSystem;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)
		class USceneComponent* Scene;

	UPROPERTY(EditAnywhere, Category = "Settings") UAnimMontage* MontageReload;
	UPROPERTY(EditAnywhere, Category = "Settings") UAnimMontage* MontageGunplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		int Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float Speed;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Settings")
		int Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		USoundBase* SoundShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		USoundBase* SoundReload;
};
