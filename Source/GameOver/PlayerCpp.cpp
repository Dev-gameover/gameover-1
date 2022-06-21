// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCpp.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerCpp::APlayerCpp()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetMesh(), "Neck");
	Camera->SetRelativeLocation(FVector(13, -21, 11));
	Camera->bUsePawnControlRotation = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	CameraDeath = CreateDefaultSubobject<UCameraComponent>("CameraDeath");
	CameraDeath->SetupAttachment(SpringArm);
	SpringArm->AddRelativeRotation(FRotator(0,-20,0));
	SpringArm->SetupAttachment(RootComponent);
	MovementPtr = Cast<UCharacterMovementComponent>(GetCharacterMovement());

	MaxSpeedWalk = 600;
	MaxSpeedRun = 1000;
	IsAim = false;
	IsDead = false;
	Health = 100;
	HealthMax = 100;
}

void APlayerCpp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCpp, IsAim);
	DOREPLIFETIME(APlayerCpp, IsReload);
	DOREPLIFETIME(APlayerCpp, Health);
	DOREPLIFETIME(APlayerCpp, IsDead);
	DOREPLIFETIME(APlayerCpp, PlayerRotationY);
}

// Called when the game starts or when spawned
void APlayerCpp::BeginPlay()
{
	Super::BeginPlay();
	WeaponRef = GetWorld()->SpawnActor<AWeapon>(WeaponDef, FVector(0, 0, 0), FRotator(0, 0, 0));
	WeaponRef->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "RightHandSocket");
}

// Called every frame
void APlayerCpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCpp::Shot);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerCpp::ShotStop);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerCpp::Aim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerCpp::AimStop);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerCpp::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerCpp::RunStop);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCpp::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCpp::Turn);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCpp::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCpp::MoveRight);
}

void APlayerCpp::MoveForward(float Value) {
	if (!IsDead) {
		AddMovementInput(GetActorForwardVector(), Value);
	}
}
void APlayerCpp::MoveRight(float Value) {
	if (!IsDead) {
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APlayerCpp::LookUp(float Value) {
	AddControllerPitchInput(Value);
	ServerSetRotationY(GetOwner()->GetActorRotation().Pitch);
}
void APlayerCpp::Turn(float Value) {
	if (!IsDead) {
		AddControllerYawInput(Value);
	}
}

void APlayerCpp::ServerSetRotationY_Implementation(float Value) {
	PlayerRotationY = Value;
}

void APlayerCpp::MulticastPlayDeath_Implementation() {
	PlayAnimMontage(AnimDeath);
	Camera->SetActive(false);
	CameraDeath->SetActive(true);
}

// Обновить здоровье игрока
void APlayerCpp::ServerSetHealth_Implementation(float Value) {
	Health = Value > 0 ? Value : 0;
	if (Health == 0) {
		IsDead = true;
		MulticastPlayDeath();
	}
}

void APlayerCpp::ServerSetAim_Implementation(bool Value) {
	IsAim = Value;
}

void APlayerCpp::Run() {
	if (!IsAim) {
		SetMaxWalk(MaxSpeedRun);
	}
}
void APlayerCpp::RunStop() {
	SetMaxWalk(MaxSpeedWalk);
}

void APlayerCpp::ServerSetMaxWalk_Implementation(float Speed) {
	SetMaxWalk(Speed);
}

void APlayerCpp::SetMaxWalk(float Speed) {
	if (!HasAuthority()) {
		ServerSetMaxWalk(Speed);
	}
	MovementPtr->MaxWalkSpeed = Speed;
}

void APlayerCpp::Aim() {
	if (!IsReload && !IsDead) {
		SetCamera(WeaponRef);
		SetMaxWalk(MaxSpeedWalk);
		ServerSetAim(true);
	}
}

void APlayerCpp::AimStop() {
	SetCamera(this);
	ServerSetAim(false);
}

void APlayerCpp::SetCamera(AActor* Target) {
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController)) {
		PlayerController->SetViewTargetWithBlend(Target, 0.1f);
	}
}

void APlayerCpp::Shot() {
	if (!IsReload && !IsDead) {
		GetWorldTimerManager().SetTimer(TimerHandleShot, this, &APlayerCpp::ServerShot, WeaponRef->Speed, true, 0);
	} 
}

void APlayerCpp::ShotStop() {
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleShot);
}

// Стрельба и перезарядка
void APlayerCpp::ServerShot_Implementation() {
	if (WeaponRef->Ammo > 0) {
		MulticastShot();
		float  LineTraceDistance = 10000.f;
		FVector Start, End;
		FVector CameraLocation = Camera->GetComponentLocation();
		FRotator CameraRotation = Camera->GetComponentRotation();

		Start = CameraLocation;
		End = CameraLocation + (CameraRotation.Vector() * LineTraceDistance);

		const FName TraceTag("MyTraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;

		FCollisionQueryParams TraceParams(FName("InteractTrace"), true, this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = true;

		// Отрисовка выстрела
		// TraceParams.TraceTag = TraceTag;

		FHitResult HitDetails = FHitResult(ForceInit);

		bool bIsHit = GetWorld()->LineTraceSingleByChannel(
			HitDetails,
			Start,
			End,
			ECC_GameTraceChannel13,
			TraceParams
		);

		if (bIsHit) {
			APlayerCpp* Player = Cast<APlayerCpp>(HitDetails.GetActor());
			if (IsValid(Player) && !Player->IsDead) {
				Player->ServerSetHealth(Player->Health - WeaponRef->Damage);
			}
		}
	}
}

// Стрельба для Multicast
void APlayerCpp::MulticastShot_Implementation() {

	WeaponRef->Shot(IsAim);
	PlayAnimMontage(WeaponRef->MontageGunplay);

	int* AmmoWeapon = Ammo.Find(WeaponRef->GetClass());
	bool IsReloadDelay = GetWorldTimerManager().IsTimerPending(TimerWeaponReload);

	if (WeaponRef->Ammo == 0 && AmmoWeapon != 0 && !IsReloadDelay) {
		ShotStop();
		float time = PlayAnimMontage(WeaponRef->MontageReload);
		GetWorld()->GetTimerManager().SetTimer(TimerWeaponReload, this, &APlayerCpp::WeaponReload, time, false);
		FVector Location = WeaponRef->SkeletalMesh->GetSocketLocation("Shot");
		FRotator Rotation = WeaponRef->SkeletalMesh->GetSocketRotation("Shot");
		UGameplayStatics::SpawnSoundAtLocation(this, WeaponRef->SoundReload, Location, Rotation);
		SetCamera(this);
		IsReload = true;
	}

}

// Перезарядка оружия
void APlayerCpp::WeaponReload() {
	int* AmmoWeapon = Ammo.Find(WeaponRef->GetClass());
	int AmmoNext = *AmmoWeapon - WeaponRef->AmmoMax;
	int AmmoNew = 0;
	if (AmmoNext > 0) {
		WeaponRef->Ammo = *AmmoWeapon - AmmoNext;
		AmmoNew = AmmoNext;
	}
	else {
		WeaponRef->Ammo = WeaponRef->AmmoMax + AmmoNext;
	}
	Ammo.Remove(WeaponRef->GetClass());
	Ammo.Add(WeaponRef->GetClass(), AmmoNew);
	IsReload = false;
}

