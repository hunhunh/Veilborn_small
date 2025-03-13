// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Items/ItemBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr; //������ null ��ȯ
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStact(UItemBase* ItemIn) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result =
		InventoryContents.FindByPredicate([&ItemIn](const UItemBase* InventoryItem)
			{
				return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
			}))
	{
		return *Result;
	}
	return nullptr;
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;

		//handle non-stackable items
		if (!InputItem->NumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem);
		}

		//handle stackable
		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			//return added all result
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Successfully added {0} {1} to the inventory."),
				InitialRequestedAddAmount, InputItem->TextData.Name
			));
		}

		//return added partial
		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
				FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"),
				InputItem->TextData.Name, StackableAmountAdded
			));
		}

		if (StackableAmountAdded <= 0)
		{
			//return added none result
			return FItemAddResult::AddedNone(FText::Format(
				FText::FromString(
					"Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
				InputItem->TextData.Name
			));
		}
	}
	check(false);
	return FItemAddResult::AddedNone(FText::FromString("TryAddItem fallthrough error. GetOwner() check somehow failed."));

}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(ItemIn->Quantity, DesiredAmountToRemove);
	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmountToRemove);

	InventoryTotalWeight -= ActualAmountToRemove * ItemIn->GetItemSinleWeight();

	OnInventoryUpdated.Broadcast();

	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItems(UItemBase* ItemIn)
{
	// �������� ���԰� ��ȿ���� Ȯ��
	if (FMath::IsNearlyZero(ItemIn->GetItemSinleWeight()) || ItemIn->GetItemSinleWeight() < 0)
	{
		//return added none
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString(
				"Could not add {0} to the inventory. Item has invalid weight value."), ItemIn->TextData.Name));
	}

	// �������� �߰��� ��� �κ��丮 ���� �ѵ��� �ʰ��ϴ��� Ȯ��
	if (InventoryTotalWeight + ItemIn->GetItemSinleWeight() > GetWeightCapacity())
	{
		//return added none
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString(
				"Could not add {0} to the inventory. Item would overflow weight limit."), ItemIn->TextData.Name));

	}

	// �κ��丮 ���� ������ �ʰ��Ǵ��� Ȯ��
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		//return added none
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString(
				"Could not add {0} to the inventory. All inventory slots are full."), ItemIn->TextData.Name));

	}

	AddNewItem(ItemIn, 1);
	//return added all result
	return FItemAddResult::AddedAll(1, FText::Format(
		FText::FromString("Successfully added a single {0} to the inventory."), ItemIn->TextData.Name
	));
}

int32 UInventoryComponent::HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(ItemIn->GetItemStackWeight()))
	{
		return 0;
	}

	//�߰��� ������ ����
	int32 AmountToDistribute = RequestedAddAmount;

	//���� ������ ã�Ƽ� �߰��� �� �ִ��� Ȯ��
	UItemBase* ExistingItemStack = FindNextPartialStact(ItemIn);

	//���� ������ ������ �ִ��� ä���
	while (ExistingItemStack)
	{
		//�ش� ������ �� ä����� �󸶳� �� �ʿ����� ���
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);

		//���� ������ ����Ͽ� �߰� ������ ������ ���
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		//�κ��丮 ���� ������ �ʰ����� ������ �߰� ����
		if (WeightLimitAddAmount > 0)
		{
			//���� ������ ���� ����
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);

			//�� ���� ������Ʈ
			InventoryTotalWeight += (ExistingItemStack->GetItemSinleWeight() * WeightLimitAddAmount);

			//���� ������ ���� ������Ʈ
			AmountToDistribute -= WeightLimitAddAmount;
			ItemIn->SetQuantity(AmountToDistribute);

			//���� ���� ������ �ʰ��� ��� �߰� �۾� �ߴ�
			if (InventoryTotalWeight + ExistingItemStack->GetItemSinleWeight() > InventoryWeightCapacity)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			// �Ϻθ� �߰��߰ų�, �ƿ� �߰��� �� ���� ��� ó��
			if (AmountToDistribute != RequestedAddAmount)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			// �߰� �Ұ����� ��� (���� �ʰ�)
			return 0;
		}
		// ��� �������� �߰������� ����
		if (AmountToDistribute <= 0)
		{
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}

		// �߰��� ������ ������ ������ �ִ��� Ȯ��
		ExistingItemStack = FindNextPartialStact(ItemIn);
	}

	//���ο� ���� ���� ���� ���� Ȯ��
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		// �� ���Կ� �߰� ������ ������ ���� ���ѿ� �°� ���
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);

		if (WeightLimitAddAmount > 0)
		{
			// �Ϻθ� �߰��� ���, ���� ���� ���� �� �� ���� ����
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				AmountToDistribute -= WeightLimitAddAmount;
				ItemIn->SetQuantity(AmountToDistribute);

				// �������� ���纻�� �����Ͽ� ���ο� �������� �߰�
				AddNewItem(ItemIn->CreateItemCopy(), WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}

			// ��ü�� �߰��� ���
			AddNewItem(ItemIn, AmountToDistribute);
			return RequestedAddAmount;
		}

		// �� ������ ������ ���� �ʰ��� �߰��� �� ���� ���
		return RequestedAddAmount - AmountToDistribute;
	}

	//�κ��丮 ������ ���� á���� �߰� �Ұ���
	return 0;
}

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSinleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount)
{
	const int32 AddAmountToMakeFullStack = StackableItem->NumericData.MaxStackSize - StackableItem->Quantity;

	return FMath::Min(AddAmountToMakeFullStack, InitialRequestedAddAmount);
}

void UInventoryComponent::AddNewItem(UItemBase* Item, const int32 AmountToAdd)
{
	UItemBase* NewItem;

	if (Item->bIsCopy || Item->bIsPickup)
	{
		// �������� �̹� ���纻�̰ų� �ʵ忡�� ȹ��� ��� �״�� ���
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// ���� �������� �����Ͽ� �� ������ ����
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}

