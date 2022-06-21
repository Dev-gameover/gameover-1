// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Weapons/Weapon.h"
#include "Camera/CameraComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "PhysicsEngine/PhysicsSettings.h"

#include "PlayerCpp.generated.h"

UCLASS()
class GAMEOVER_API APlayerCpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCpp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FTimerHandle TimerHandleShot;
	FTimerHandle TimerWeaponReload;
	UCharacterMovementComponent* MovementPtr;

	float MaxSpeedWalk;
	float MaxSpeedRun;

	UPROPERTY(Replicated) bool IsReload;

	UPROPERTY(Replicated, BlueprintReadOnly) bool IsAim;
	UPROPERTY(Replicated, BlueprintReadOnly) bool IsDead;
	UPROPERTY(Replicated, BlueprintReadOnly) float PlayerRotationY;

	UPROPERTY(Replicated, BlueprintReadOnly, EditAnywhere)
		float Health;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float HealthMax;
		
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon") TMap<TSubclassOf<AWeapon>, int> Ammo;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon") TSubclassOf<AWeapon> WeaponDef;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
		AWeapon* WeaponRef;

	UPROPERTY(EditAnyWhere, Category = "Components") UCameraComponent* Camera;
	UPROPERTY(EditAnyWhere, Category = "Components") UCameraComponent* CameraDeath;
	UPROPERTY(EditAnyWhere, Category = "Components") USpringArmComponent* SpringArm;

	UPROPERTY(EditAnyWhere, Category = "Anim")
		UAnimMontage* AnimDeath;

	UFUNCTION(Server, Reliable) void ServerSetAim(bool Value);
	UFUNCTION(Server, Reliable) void ServerShot();
	UFUNCTION(Server, Reliable) void ServerSetMaxWalk(float Speed);
	UFUNCTION(Server, Reliable) void ServerSetHealth(float Value);
	UFUNCTION(Server, Reliable) void ServerSetRotationY(float Value);
	
	UFUNCTION(NetMulticast, Reliable) void MulticastShot();
	UFUNCTION(NetMulticast, Reliable) void MulticastPlayDeath();

	void Shot();
	void ShotStop();
	void WeaponReload();
	void Aim();
	void AimStop();
	void SetCamera(AActor* Target);
	void SetMaxWalk(float Speed);
	void Run();
	void RunStop();
	void LookUp(float Value);
	void Turn(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);
};
