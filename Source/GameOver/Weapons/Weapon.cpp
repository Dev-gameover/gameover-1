// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	Scene->SetupAttachment(RootComponent);
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	SkeletalMesh->SetupAttachment(Scene);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SkeletalMesh);
	Camera->AddRelativeRotation(FRotator(0, 90, 0));

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("Fire");
	ParticleSystem->SetupAttachment(SkeletalMesh, "Shot");
	ParticleSystem->SetRelativeLocation(FVector(22, 0, 0));

	Name = "Default Name";
	AmmoMax = 30;
	Damage = 100;
	Speed = 600;

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, Ammo);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	Speed = 60 / Speed;
	AmmoMax = Ammo;
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::Shot(bool IsAim)
{
	FVector Location = SkeletalMesh->GetSocketLocation("Shot");
	FRotator Rotation = SkeletalMesh->GetSocketRotation("Shot");
	if (!IsAim) {
		float min = -0.5;
		float max = 0.5;
		Rotation.Yaw += FMath::RandRange(min, max);
		Rotation.Pitch += FMath::RandRange(min, max);
	}
	if (IsValid(Projectile)) {
		GetWorld()->SpawnActor<AActor>(Projectile, Location, Rotation);
	}
	ParticleSystem->SetTemplate(MuzzleFlash);
	Ammo--;
	UGameplayStatics::SpawnSoundAtLocation(this, SoundShot, Location, Rotation);
}