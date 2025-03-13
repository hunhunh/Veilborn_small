// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Pickup.h"
#include "Items/ItemBase.h"
#include "Player/PlayerCharacter.h"
#include "Inventory/InventoryComponent.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetSimulatePhysics(true);
	SetRootComponent(PickupMesh);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	InitializePickup(UItemBase::StaticClass(), ItemQuantity);
}

void APickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

		ItemRef = NewObject<UItemBase>(this, BaseClass);

		ItemRef->ID = ItemData->ID;
		ItemRef->ItemType = ItemData->ItemType;
		ItemRef->ItemQuality = ItemData->ItemQuality;
		ItemRef->NumericData = ItemData->NumericData;
		ItemRef->TextData = ItemData->TextData;
		ItemRef->AssetData = ItemData->AssetData;

		ItemRef->NumericData.bIsStackable = ItemData->NumericData.MaxStackSize > 1;
		InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemRef = ItemToDrop;

	InQuantity <= 0 ? ItemRef->SetQuantity(1) : ItemRef->SetQuantity(InQuantity);
	ItemRef->NumericData.Weight = ItemToDrop->GetItemSinleWeight();
	ItemRef->OwningInventory = nullptr;
	PickupMesh->SetStaticMesh(ItemToDrop->AssetData.Mesh);

	UpdateInteractableData();
}

void APickup::BeginFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickup::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}

void APickup::Interact(APlayerCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::TakePickup(APlayerCharacter* Taker)
{
	if (!IsPendingKillPending())
	{
		if (ItemRef)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemRef);

				switch (AddResult.OperationResult)
				{
				case EItemAddResult::IAR_NoItemAdded:
					break;

				case EItemAddResult::IAR_PartialAmountItemAdded:
					UpdateInteractableData();
					//Widget Update
					Taker->UpdateInteractionWidget();
					break;

				case EItemAddResult::IAR_AllItemAdded:
					Destroy();
					break;
				}

				UE_LOG(LogTemp, Warning, TEXT("%s"), *AddResult.ResultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Player inventory component is null"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Pickup internal itemRef was somehow null"));
		}
	}
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::PickUp;
	InstanceInteractableData.Action = ItemRef->TextData.InteractionText;
	InstanceInteractableData.Name = ItemRef->TextData.Name;
	InstanceInteractableData.Quantity = ItemRef->Quantity;
	InteractableData = InstanceInteractableData;
}

//void APickup::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeChainProperty(PropertyChangedEvent);
//
//	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
//
//	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredItemID))
//	{
//		if (ItemDataTable)
//		{
//			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
//			{
//				PickupMesh->SetStaticMesh(ItemData->AssetData.Mesh);
//			}
//		}
//	}
//}

