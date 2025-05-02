// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/VBSpringArmComponent.h"

UVBSpringArmComponent::UVBSpringArmComponent()
{
	MinArmLength = 400.f;
}

void UVBSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	// 먼저 부모(SpringArm)의 기본 충돌 처리
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);

	
}
