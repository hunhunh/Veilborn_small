// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.generated.h"

class UItemBase;
class UDragItemVisual;
class UInventoryTooltip;
class UTextBlock;
class UBorder;
class UImage;

/**
 * 
 */
UCLASS()
class VEILBORN_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetItemRef(UItemBase* ItemIn) { ItemRef = ItemIn; };
	FORCEINLINE UItemBase* GetItemRef() const { return ItemRef; };

protected:
	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot")
	UItemBase* ItemRef;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<UDragItemVisual> DragItemVisualClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<UInventoryTooltip> ToolTipClass;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(VisibleAnywhere, Category = "Inventory Slot", meta = (BindWidget))
	UTextBlock* ItemQuantity;

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
