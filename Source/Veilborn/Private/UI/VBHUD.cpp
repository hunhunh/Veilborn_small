// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/VBHUD.h"
#include "Player/PlayerMainWidget.h"
#include "InventoryUI/MainMenu.h"
#include "InventoryUI/InteractionWidget.h"

AVBHUD::AVBHUD()
{
}


void AVBHUD::DisplayMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = true;
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AVBHUD::HideMenu()
{
	if (MainMenuWidget)
	{
		bIsMenuVisible = false;
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AVBHUD::ToggleMenu()
{
	if (bIsMenuVisible)
	{
		HideMenu();

		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();

		const FInputModeGameAndUI InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void AVBHUD::ShowInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AVBHUD::HideInteractionWidget()
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AVBHUD::UpdateInteractionWidget(const FInteractableData* InteractableData)
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}

void AVBHUD::BeginPlay()
{
	Super::BeginPlay();

	//PlayerMainWidget
	if (MainWidgetClass)
	{
		MainWidgetInstance = CreateWidget<UPlayerMainWidget>(GetWorld(), MainWidgetClass);
		if (MainWidgetInstance)
		{
			MainWidgetInstance->AddToViewport();
			MainWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}


	// **메인 메뉴 위젯 생성 및 초기화**
	if (MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	// **상호작용 위젯 생성 및 초기화**
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(-1);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
