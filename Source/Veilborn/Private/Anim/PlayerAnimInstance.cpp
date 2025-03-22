// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/PlayerAnimInstance.h"
#include "Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwningActor());
	if (IsValid(PlayerCharacter))
	{
		CharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsValid(PlayerCharacter))
	{
		FVector Velocity = CharacterMovement->Velocity;
		FRotator Rotation = PlayerCharacter->GetActorRotation();
		FVector UnrotateVector = Rotation.UnrotateVector(Velocity);

		Vertical = UnrotateVector.X;
		Horizontal = UnrotateVector.Y;

		Speed = PlayerCharacter->GetVelocity().Size2D();

		auto Acceleration = CharacterMovement->GetCurrentAcceleration();
		bShouldMove = Speed > 3.f && Acceleration != FVector::Zero();

		bIsFalling = CharacterMovement->IsFalling();
	}

}
