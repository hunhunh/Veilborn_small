// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/PlayerAnimInstance.h"
#include "Player/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
		Velocity = CharacterMovement->Velocity;
		FRotator Rotation = PlayerCharacter->GetActorRotation();
		FVector UnrotateVector = Rotation.UnrotateVector(Velocity);

		Vertical = UnrotateVector.X;
		Horizontal = UnrotateVector.Y;

		Speed = PlayerCharacter->GetVelocity().Size2D();
		Direction = UAnimInstance::CalculateDirection(Velocity, Rotation);

		auto Acceleration = CharacterMovement->GetCurrentAcceleration();
		bShouldMove = Speed > 0.3f && Acceleration != FVector::Zero();

		bIsFalling = CharacterMovement->IsFalling();

		AimRotation = PlayerCharacter->GetBaseAimRotation();
		FRotator RotFromX = UKismetMathLibrary::MakeRotFromX(Velocity);

		FRotator DeltaRotation = AimRotation - RotFromX;
		DeltaRotation.Normalize();

		YawOffset = DeltaRotation.Yaw;
	
		

		bIsRunning = PlayerCharacter->IsRunning();
	}

}
