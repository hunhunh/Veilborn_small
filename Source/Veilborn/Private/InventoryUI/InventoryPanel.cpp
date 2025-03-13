// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUI/InventoryPanel.h"
#include "Player/PlayerCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Components/TextBlock.h"
#include "InventoryUI/InventoryItemSlot.h"
#include "Components/WrapBox.h"
#include "Interface/ItemDragDropOperation.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwningPlayerPawn());

	if (PlayerCharacter)
	{
		InventoryRef = PlayerCharacter->GetInventory();

		if (InventoryRef)
		{
			InventoryRef->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
			SetInfoText();
		}
	}
}

void UInventoryPanel::RefreshInventory()
{
	if (InventoryRef && InventorySlotClass)
	{
		InventoryPanel->ClearChildren();

		for (UItemBase* const& InventoryItem : InventoryRef->GetInventoryContents())
		{
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventorySlotClass);
			ItemSlot->SetItemRef(InventoryItem);
			InventoryPanel->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}

void UInventoryPanel::SetInfoText() const
{
	const FString WeightInfoValue{
		   FString::SanitizeFloat(InventoryRef->GetInventoryTotalWeight()) + "/" +
		   FString::SanitizeFloat(InventoryRef->GetWeightCapacity())
	};

	const FString CapacityInfoValue{
		FString::FromInt(InventoryRef->GetInventoryContents().Num()) + "/" +
		FString::FromInt(InventoryRef->GetSlotsCapacity())
	};

	WeightInfo->SetText(FText::FromString(WeightInfoValue));
	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}

bool UInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	// 유효한 아이템 데이터가 있고, 인벤토리 참조가 존재하는지 확인
	if (ItemDragDrop->SourceItem && InventoryRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on InventoryPanel."))

			return true;
	}

	return false;
}
