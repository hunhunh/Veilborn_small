// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	//인벤토리가 가득 차 있거나 특정 아이템을 저장할 수 없는 상태.
	IAR_NoItemAdded UMETA(DisplayName = "No item added"),
	//플레이어가 10개를 주웠는데, 인벤토리에 5개밖에 들어갈 공간이 없어서 5개만 추가됨.
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial amount of item added"),
	//아이템이 전부 정상적으로 추가된 경우.
	IAR_AllItemAdded UMETA(DisplayName = "All of item added"),
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() : ActualAmountAdded(0), OperationResult(EItemAddResult::IAR_NoItemAdded), ResultMessage(FText::GetEmpty())
	{
	};
	// 실제로 추가된 아이템 개수
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded;

	// 아이템 추가 결과 (EItemAddResult 사용)
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult OperationResult;

	// 결과 메시지
	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorText;
		return AddedNoneResult;
	};
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorText;
		return AddedPartialResult;
	};
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VEILBORN_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	//검색기능 (Item의 양을 추가할 떄 Inventory에 이미 있는 유형과 일치하는 것이 있는지 확인)
	UFUNCTION(Category = "Inventory")
	UItemBase* FindMatchingItem(UItemBase* ItemIn) const;

	//ID로 항목을 찾는데 사용
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextItemByID(UItemBase* ItemIn) const;

	//인벤토리에서 동일한 ID를 가진 아이템 중에서, 아직 최대 개수(Full Stack)가 아닌 아이템을 찾는 기능
	UFUNCTION(Category = "Inventory")
	UItemBase* FindNextPartialStact(UItemBase* ItemIn) const;

	//Item 추가하는 기능
	UFUNCTION(Category = "Inventory")
	FItemAddResult HandleAddItem(UItemBase* InputItem);

	//Item의 단일 Instance를 제거(항목을 삭제)
	UFUNCTION(Category = "Inventory")
	void RemoveSingleInstanceOfItem(UItemBase* ItemToRemove);

	//제거할 Item의 양(부분적인 양을 삭제)
	UFUNCTION(Category = "Inventory")
	int32 RemoveAmountOfItem(UItemBase* ItemIn, int32 DesiredAmountToRemove);

	//ItemIn의 스택을 분리하여 새로운 스택을 만듦.
	UFUNCTION(Category = "Inventory")
	void SplitExistingStack(UItemBase* ItemIn, const int32 AmountToSplit);

	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };

	UFUNCTION(Category = "Inventory")
	FORCEINLINE float GetWeightCapacity() const { return InventoryWeightCapacity; };

	UFUNCTION(Category = "Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };

	UFUNCTION(Category = "Inventory")
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; };


	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };

	UFUNCTION(Category = "Inventory")
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };

	FOnInventoryUpdated OnInventoryUpdated;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float InventoryTotalWeight; //현제 Inventory 무게

	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	int32 InventorySlotsCapacity;

	UPROPERTY(EditInstanceOnly, Category = "Inventory")
	float InventoryWeightCapacity; //Inventory 최대 허용 무게

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<TObjectPtr<UItemBase>> InventoryContents;


	//스텍이 불가능한 아이템 추가 처리
	FItemAddResult HandleNonStackableItems(UItemBase* ItemIn);

	//인벤토리에 스택 가능한 아이템을 추가하는 기능
	// (기존 스택이 있다면 최대한 기존 스택에 추가하고, 없거나 꽉 차 있다면 새로운 스택을 추가)
	int32 HandleStackableItems(UItemBase* ItemIn, int32 RequestedAddAmount);

	//무게 제한에 맞춰 추가 가능한 개수 반환
	int32 CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount);
	// 주어진 아이템 스택(StackableItem)이 최대 스택 크기에 도달하려면 몇 개가 더 필요한지 계산
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount);

	void AddNewItem(UItemBase* Item, const int32 AmountToAdd);
};
