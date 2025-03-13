// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class APlayerCharacter;
struct FInteractableData;
class UTextBlock;
class UProgressBar;

/**
 * 
 */
UCLASS()
class VEILBORN_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Interaction | Player Reference")
	TObjectPtr<APlayerCharacter> PlayerReference;

	void UpdateWidget(const FInteractableData* InteractableData);

protected:
	//아이템 대상의 이름표시
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> NameText;

	//줍기 또는 사용하기 액션 표시
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> ActionText;

	//아이템 개수 표시
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> QuantityText;

	//해당 상호작용 키 표시
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> KeyPressText;

	//진행도 표시
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UProgressBar> InteractionProgressBar;

	//상호작용 진행 시간
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	float CurrentInteractionDuration;

	UFUNCTION(Category = "Interaction Widget | Interactable Data")
	float UpdateInteractionProgress();

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
