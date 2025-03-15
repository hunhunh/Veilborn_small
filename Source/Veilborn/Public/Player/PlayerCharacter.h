// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/InteractionInterface.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AWeaponActor;
class AVBHUD;
class UInventoryComponent;
class UItemBase;

USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() : CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f)
	{

	};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

USTRUCT(BlueprintType)
struct FPlayerStat{

	GENERATED_BODY()
	
	//최대체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;

	//최대 스테미너
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 100.0f;

	//방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Defense = 10.0f;

};

UCLASS()
class VEILBORN_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CamaraInAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CamaraOutAction;
	
public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerStat Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* AttackAnim;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeaponActor> WeaponClass;*/

private:

	/*UPROPERTY()
	AWeaponActor* EquippeddWeapon;*/
	
public:

	/*AWeaponActor* GetWeapon(){return EquippeddWeapon;}*/
	
    void CameraBoomIn();

	void CameraBoomOut();

	/*void Attack();

	void ApplyDamage(AActor* Actor, float Damage);*/
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	//===========상호작용(채집 시스탬)==============
protected:
	UPROPERTY(VisibleAnywhere, Category = "Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	//상호작용 키
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	//상호작용 확인 빈도
	float InteractionCheckFrequency;

	//상호작용 확인 거리
	float InteractionCheckDistance;

	FTimerHandle TimerHandle_Interaction;

	FInteractionData InteractionData;

	//플레이어 주변의 상호작용 가능한 오브젝트 탐색
	void PerformInteractionCheck();
	//새로운 상호작용 가능한 오브젝트를 감지하고 설정
	void FoundInteractable(AActor* NewInteractable);
	//상호작용 가능한 오브젝트가 없을 때 처리
	void NoInteractableFound();
	//플레이어가 상호작용을 시도할 때 실행
	void BeginInteract();
	//플레이어의 상호작용 중단
	void EndInteract();
	//플레이어가 상호작용을 완료했을 때 실행
	void Interact();
	
	public:
		FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

	
		//Widget
protected:
	UPROPERTY()
	TObjectPtr<AVBHUD> HUD;

	//Inventory
protected:
	UPROPERTY(VisibleAnywhere, Category = "Character | Inventory")
	TObjectPtr<UInventoryComponent> PlayerInventory;

public:
	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; };

	//InventoryWidget
public:
	void UpdateInteractionWidget() const;

	//Input Inventory toggle menu
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ToggleMenuAction;

	void ToggleMenu();

	//Drop Item
public:
	void DropItem(UItemBase* ItemToDrop, int32 QuantityToDrop);
};
