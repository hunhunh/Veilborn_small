// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class APlayerCharacter;
class UInventoryComponent;
class UInventoryItemSlot;
class UWrapBox;
class UGridPanel;
class UTextBlock;

/**
 * 
 */
UCLASS()
class VEILBORN_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()
	

public:
	//인벤토리 UI를 새로고침하는 함수
	UFUNCTION()
	void RefreshInventory();

	//아이템 슬롯을 표시할 래핑 박스
	UPROPERTY(meta = (BindWidget))
	UWrapBox* InventoryPanel;

	//현재 인벤토리의 총 무게 정보를 표시
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	//현제 인벤토리의 사용 슬롯 개수/최대 슬롯 개수를 표시
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	APlayerCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryRef;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventorySlotClass;



protected:
	//인벤토리 정보를 UI에 표시(무게 및 슬롯 개수정보 Update)
	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
