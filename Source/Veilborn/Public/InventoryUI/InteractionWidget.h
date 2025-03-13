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
	//������ ����� �̸�ǥ��
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> NameText;

	//�ݱ� �Ǵ� ����ϱ� �׼� ǥ��
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> ActionText;

	//������ ���� ǥ��
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> QuantityText;

	//�ش� ��ȣ�ۿ� Ű ǥ��
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UTextBlock> KeyPressText;

	//���൵ ǥ��
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	TObjectPtr<UProgressBar> InteractionProgressBar;

	//��ȣ�ۿ� ���� �ð�
	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "Interaction | Interactable Data")
	float CurrentInteractionDuration;

	UFUNCTION(Category = "Interaction Widget | Interactable Data")
	float UpdateInteractionProgress();

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
};
