// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractionInterface.generated.h"

class APlayerCharacter;

UENUM()
enum class EInteractableType : uint8
{
	PickUp UMETA(DisplayName = "PickUp"),
	NonPlayerCharacter UMETA(DisplayName = "NonPlayerCharacter"),
	Device UMETA(DisplayName = "Device"),
	Toggle UMETA(DisplayName = "Toggle"), //문
	Container UMETA(DisplayName = "Container"),
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() :
		InteractableType(EInteractableType::PickUp), Name(FText::GetEmpty()), Action(FText::GetEmpty()),
		Quantity(0), InteractionDuaration(0.0f)
	{

	};

	EInteractableType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText Name;

	UPROPERTY(EditInstanceOnly)
	FText Action;

	// 상호작용 가능한 오브젝트가 아이템(PickUp)일 경우 해당 아이템의 수량을 나타냄
	UPROPERTY(EditInstanceOnly)
	int8 Quantity;

	// 밸브, 문 등 일정 시간이 걸리는 상호작용을 위한 변수 (초 단위)
	UPROPERTY(EditInstanceOnly)
	float InteractionDuaration;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VEILBORN_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(APlayerCharacter* PlayerCharacter);

	FInteractableData InteractableData;
};
