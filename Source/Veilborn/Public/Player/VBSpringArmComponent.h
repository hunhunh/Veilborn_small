// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "VBSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class VEILBORN_API UVBSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
	
public:
	UVBSpringArmComponent();

protected:
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Camera")
	float MinArmLength;
};
