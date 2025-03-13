// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/AttackNotifyState.h"
#include "Player/PlayerCharacter.h"
#include "Items/WeaponActor.h"

void UAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	/*if (Player)
	{
		Weapon = Cast<AWeaponActor>(Player->GetWeapon());
		if (Weapon)
		{
			Weapon->OnCollision();
		}
	}*/
		
	
}

void UAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	/*if (Player)
	{
		Weapon = Cast<AWeaponActor>(Player->GetWeapon());
		if (Weapon)
		{
			Weapon->OffCollision();
		}
	}*/
	
}
