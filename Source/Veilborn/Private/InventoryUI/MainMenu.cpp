// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUI/MainMenu.h"
#include "Player/PlayerCharacter.h"
#include "Interface/ItemDragDropOperation.h"
#include "Items/ItemBase.h"

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());
}

bool UMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* DropItem = Cast<UItemDragDropOperation>(InOperation);

	if (PlayerCharacter && DropItem->SourceItem)
	{
		PlayerCharacter->DropItem(DropItem->SourceItem, DropItem->SourceItem->Quantity);
		return true;
	}

	return false;
}
