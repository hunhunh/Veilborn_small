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
	Toggle UMETA(DisplayName = "Toggle"), //��
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

	// ��ȣ�ۿ� ������ ������Ʈ�� ������(PickUp)�� ��� �ش� �������� ������ ��Ÿ��
	UPROPERTY(EditInstanceOnly)
	int8 Quantity;

	// ���, �� �� ���� �ð��� �ɸ��� ��ȣ�ۿ��� ���� ���� (�� ����)
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
