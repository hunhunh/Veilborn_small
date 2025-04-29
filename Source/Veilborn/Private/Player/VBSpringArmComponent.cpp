// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VBSpringArmComponent.h"

UVBSpringArmComponent::UVBSpringArmComponent()
{
	MinArmLength = 150.f;
}

void UVBSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);

	if (TargetArmLength < MinArmLength)
	{
		TargetArmLength = MinArmLength;
	}
}
