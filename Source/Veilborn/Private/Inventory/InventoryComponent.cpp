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
	return nullptr; //없으면 null 반환
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
	// 아이템의 무게가 유효한지 확인
	if (FMath::IsNearlyZero(ItemIn->GetItemSinleWeight()) || ItemIn->GetItemSinleWeight() < 0)
	{
		//return added none
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString(
				"Could not add {0} to the inventory. Item has invalid weight value."), ItemIn->TextData.Name));
	}

	// 아이템을 추가할 경우 인벤토리 무게 한도를 초과하는지 확인
	if (InventoryTotalWeight + ItemIn->GetItemSinleWeight() > GetWeightCapacity())
	{
		//return added none
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString(
				"Could not add {0} to the inventory. Item would overflow weight limit."), ItemIn->TextData.Name));

	}

	// 인벤토리 슬롯 개수가 초과되는지 확인
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

	//추가할 아이템 개수
	int32 AmountToDistribute = RequestedAddAmount;

	//기존 스택을 찾아서 추가할 수 있는지 확인
	UItemBase* ExistingItemStack = FindNextPartialStact(ItemIn);

	//기존 스텍이 있으면 최대한 채우기
	while (ExistingItemStack)
	{
		//해당 스텍을 꽉 채우려면 얼마나 더 필요한지 계산
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);

		//무게 제한을 고려하여 추가 가능한 개수를 계산
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);

		//인벤토리 무게 제한을 초과하지 않으면 추가 가능
		if (WeightLimitAddAmount > 0)
		{
			//기존 스텍의 개수 증가
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);

			//총 무게 업데이트
			InventoryTotalWeight += (ExistingItemStack->GetItemSinleWeight() * WeightLimitAddAmount);

			//남은 아이템 개수 업데이트
			AmountToDistribute -= WeightLimitAddAmount;
			ItemIn->SetQuantity(AmountToDistribute);

			//만약 무게 제한을 초과할 경우 추가 작업 중단
			if (InventoryTotalWeight + ExistingItemStack->GetItemSinleWeight() > InventoryWeightCapacity)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			// 일부만 추가했거나, 아예 추가할 수 없는 경우 처리
			if (AmountToDistribute != RequestedAddAmount)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			// 추가 불가능한 경우 (무게 초과)
			return 0;
		}
		// 모든 아이템을 추가했으면 종료
		if (AmountToDistribute <= 0)
		{
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}

		// 추가로 동일한 아이템 스택이 있는지 확인
		ExistingItemStack = FindNextPartialStact(ItemIn);
	}

	//새로운 스택 생성 가능 여부 확인
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		// 새 슬롯에 추가 가능한 개수를 무게 제한에 맞게 계산
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);

		if (WeightLimitAddAmount > 0)
		{
			// 일부만 추가할 경우, 남은 개수 조정 후 새 스택 생성
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				AmountToDistribute -= WeightLimitAddAmount;
				ItemIn->SetQuantity(AmountToDistribute);

				// 아이템의 복사본을 생성하여 새로운 스택으로 추가
				AddNewItem(ItemIn->CreateItemCopy(), WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute;
			}

			// 전체를 추가할 경우
			AddNewItem(ItemIn, AmountToDistribute);
			return RequestedAddAmount;
		}

		// 빈 슬롯은 있지만 무게 초과로 추가할 수 없는 경우
		return RequestedAddAmount - AmountToDistribute;
	}

	//인벤토리 슬롯이 가득 찼으면 추가 불가능
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
		// 아이템이 이미 복사본이거나 필드에서 획득된 경우 그대로 사용
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else
	{
		// 기존 아이템을 복사하여 새 아이템 생성
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	InventoryTotalWeight += NewItem->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}

