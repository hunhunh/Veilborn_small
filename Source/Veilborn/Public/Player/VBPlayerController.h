// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VBPlayerController.generated.h"

class UPlayerMainWidget;

/**
 * 
 */
UCLASS()
class VEILBORN_API AVBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	// 사용자 인터페이스에 대한 포인터 선언
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	//TSubclassOf<UPlayerMainWidget> MainWidgetClass;

	//// 실제 사용자 인터페이스 객체
	//UPlayerMainWidget* MainWidgetInstance;
	
};
