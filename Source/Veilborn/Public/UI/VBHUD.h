// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VBHUD.generated.h"

class UMainMenu;
class UInteractionWidget;
struct FInteractableData;
class UPlayerMainWidget;

/**
 * 
 */
UCLASS()
class VEILBORN_API AVBHUD : public AHUD
{
	GENERATED_BODY()

public:
	AVBHUD();

	//===========PlayerMainWidget 관리=======================
	//// 사용자 인터페이스에 대한 포인터 선언
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UPlayerMainWidget> MainWidgetClass;
//
//	void ShowPlayerMainWidget();
//
protected:
//
//	// 실제 사용자 인터페이스 객체
	UPlayerMainWidget* MainWidgetInstance;



public:

	//=================상호작용 , 인벤토리 UI=================
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UInteractionWidget>	InteractionWidgetClass;

	//true: 메뉴 표시 중, false: 숨김
	bool bIsMenuVisible;

	// ==============================
	// **인벤토리 UI 관련 함수**
	// ==============================
	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	// ==============================
	// **상호작용 UI 관련 함수**
	// ==============================

	void ShowInteractionWidget();
	void HideInteractionWidget();
	void UpdateInteractionWidget(const FInteractableData* InteractableData);

protected:
	UPROPERTY()
	UMainMenu* MainMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	virtual void BeginPlay() override;
};
