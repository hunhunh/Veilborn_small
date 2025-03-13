// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTooltip.generated.h"

class UInventoryItemSlot;
class UTextBlock;
class UItemBase;

/**
 * 
 */
UCLASS()
class VEILBORN_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 현재 마우스를 올려놓은 인벤토리 슬롯
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryItemSlot> InventorySlotBeingHovered;

	// 아이템의 이름을 표시하는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	// 아이템의 유형
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemType;

	// 무기 아이템의 공격력 값을 표시하는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageValue;

	// 방어구 아이템의 방어력을 표시하는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ArmorRating;

	// 아이템의 사용 방법을 설명하는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UsageText;

	// 아이템의 상세 설명을 표시하는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescription;

	// 아이템이 한 슬롯에 최대 몇 개까지 쌓일 수 있는지를 표시하는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxStackSize;

	// 현재 스택의 총 무게를 표시하는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StackWeight;

protected:
	virtual void NativeConstruct() override;
};
