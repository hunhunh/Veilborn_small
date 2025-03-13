// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WeaponActor.h"
#include "Enemy/EnemyBase.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	WeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));

	
		
	WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnOverlapBegin);
	
	WeaponCollision->SetupAttachment(WeaponMesh);
	SetRootComponent(WeaponMesh);
	Damage = 10.f;
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
	OffCollision();
	
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponActor::EquipWeapon(ACharacter* Character, FName SocketName)
{
	if (Character && WeaponMesh)
	{
		// 무기를 캐릭터의 특정 소켓에 부착
		AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void AWeaponActor::OnCollision()
{
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
}

void AWeaponActor::OffCollision()
{
	WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);

	if (Enemy)
	{
		Enemy->TakeDamage(Damage);
	}
}



