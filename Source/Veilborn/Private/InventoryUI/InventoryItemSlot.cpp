// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUI/InventoryItemSlot.h"
#include "InventoryUI/InventoryTooltip.h"
#include "Items/ItemBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryUI/DragItemVisual.h"
#include "Interface/ItemDragDropOperation.h"

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemRef)
	{
		switch (ItemRef->ItemQuality)
		{
		case EItemQuality::Shoddy:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;

		case EItemQuality::Common:
			ItemBorder->SetBrushColor(FLinearColor::Blue);
			break;

		default:
			break;
		}
	}

	ItemIcon->SetBrushFromTexture(ItemRef->AssetData.Icon);

	if (ItemRef->NumericData.bIsStackable)
	{
		ItemQuantity->SetText(FText::AsNumber(ItemRef->Quantity));
	}
	else
	{
		ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ToolTipClass)
	{
		UInventoryTooltip* ToolTip = CreateWidget<UInventoryTooltip>(this, ToolTipClass);
		ToolTip->InventorySlotBeingHovered = this;
		SetToolTip(ToolTip);
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	//오른쪽 클릭 시 하위 메뉴
	//todo..

	return Reply.Unhandled();
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DragItemVisualClass)
	{
		const TObjectPtr<UDragItemVisual> DragVisual = CreateWidget<UDragItemVisual>(this, DragItemVisualClass);

		DragVisual->ItemIcon->SetBrushFromTexture(ItemRef->AssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());
		DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemRef->Quantity));

		ItemRef->NumericData.bIsStackable ? DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemRef->Quantity))
			: DragVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

		UItemDragDropOperation* DragItem = NewObject<UItemDragDropOperation>();
		DragItem->SourceItem = ItemRef;
		DragItem->SourceInventory = ItemRef->OwningInventory;

		DragItem->DefaultDragVisual = DragVisual;
		DragItem->Pivot = EDragPivot::TopLeft;

		OutOperation = DragItem;
	}
}

bool UInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
