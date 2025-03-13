// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractionInterface.h"
#include "Pickup.generated.h"

class UDataTable;
class UItemBase;

UCLASS()
class VEILBORN_API APickup : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	void InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity);
	void InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity);

	FORCEINLINE UItemBase* GetItemData() { return ItemRef; };

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Reference")
	TObjectPtr<UItemBase> ItemRef;

	UPROPERTY(VisibleInstanceOnly, Category = "Pickup | Interaction Data")
	FInteractableData InstanceInteractableData;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	int32 ItemQuantity;

	//UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	//FDataTableRowHandle ItemRowHandle;



	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	UDataTable* ItemDataTable;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Initialization")
	FName DesiredItemID;

	virtual void Interact(APlayerCharacter* PlayerCharacter) override;
	void TakePickup(APlayerCharacter* Taker);
	void UpdateInteractableData();

//#if WITH_EDITOR
//	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
//#endif
};
