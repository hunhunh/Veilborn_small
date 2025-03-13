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

	//===========PlayerMainWidget ����=======================
	//// ����� �������̽��� ���� ������ ����
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UPlayerMainWidget> MainWidgetClass;
//
//	void ShowPlayerMainWidget();
//
protected:
//
//	// ���� ����� �������̽� ��ü
	UPlayerMainWidget* MainWidgetInstance;



public:

	//=================��ȣ�ۿ� , �κ��丮 UI=================
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UMainMenu> MainMenuClass;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UInteractionWidget>	InteractionWidgetClass;

	//true: �޴� ǥ�� ��, false: ����
	bool bIsMenuVisible;

	// ==============================
	// **�κ��丮 UI ���� �Լ�**
	// ==============================
	void DisplayMenu();
	void HideMenu();
	void ToggleMenu();

	// ==============================
	// **��ȣ�ۿ� UI ���� �Լ�**
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
