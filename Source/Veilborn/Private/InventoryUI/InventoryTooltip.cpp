// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUI/InventoryTooltip.h"
#include "Components/TextBlock.h"
#include "Items/ItemBase.h"
#include "InventoryUI/InventoryItemSlot.h"

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	if (const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemRef())
	{
		switch (ItemBeingHovered->ItemQuality)
		{
		case EItemQuality::Shoddy:
			ItemType->SetColorAndOpacity(FLinearColor::Gray);
			break;
		case EItemQuality::Common:
			ItemType->SetColorAndOpacity(FLinearColor::Blue);
			break;

		default:;
		}

		switch (ItemBeingHovered->ItemType)
		{
		case EItemType::Armor:
			break;

		case EItemType::Weapon:
			break;

		case EItemType::Consumable:
			ItemType->SetText(FText::FromString("Consumable"));
			DamageValue->SetVisibility(ESlateVisibility::Collapsed);
			ArmorRating->SetVisibility(ESlateVisibility::Collapsed);
			break;

		default:;
		}

		ItemName->SetText(ItemBeingHovered->TextData.Name);
		DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.DamageValue));
		ArmorRating->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.ArmorRating));
		UsageText->SetText(ItemBeingHovered->TextData.UsageText);
		ItemDescription->SetText(ItemBeingHovered->TextData.Description);
		//StackWeight->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));



		const FString WeightInfo =
		{ "Weight: " + FString::SanitizeFloat(ItemBeingHovered->GetItemStackWeight()) };

		StackWeight->SetText(FText::FromString(WeightInfo));

		if (ItemBeingHovered->NumericData.bIsStackable)
		{
			const FString StackInfo =
			{ "Max stack size: " + FString::FromInt(ItemBeingHovered->NumericData.MaxStackSize) };

			MaxStackSize->SetText(FText::FromString(StackInfo));
		}
		else
		{
			MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}