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
	// ���� ���콺�� �÷����� �κ��丮 ����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryItemSlot> InventorySlotBeingHovered;

	// �������� �̸��� ǥ���ϴ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	// �������� ����
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemType;

	// ���� �������� ���ݷ� ���� ǥ���ϴ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageValue;

	// �� �������� ������ ǥ���ϴ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ArmorRating;

	// �������� ��� ����� �����ϴ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UsageText;

	// �������� �� ������ ǥ���ϴ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemDescription;

	// �������� �� ���Կ� �ִ� �� ������ ���� �� �ִ����� ǥ���ϴ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxStackSize;

	// ���� ������ �� ���Ը� ǥ���ϴ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StackWeight;

protected:
	virtual void NativeConstruct() override;
};
