// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Player/VBPlayerController.h"
#include "Enemy/EnemyBase.h"
#include "Items/WeaponActor.h"
#include "UI/VBHUD.h"
#include "Inventory/InventoryComponent.h"
#include "Items/Pickup.h"
#include "Player/ComboActionData.h"
#include "Player/VBSpringArmComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(30.0f, 86.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 550.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<UVBSpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	CameraBoom->bDoCollisionTest = true;
	CameraBoom->ProbeSize = 15.f;

	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	


	bIsRun = false;

	//상호 작용
	
	InteractionCheckFrequency = 0.1;
	InteractionCheckDistance = 160.0f;

	//Inventory
	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(50.0f);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (AVBPlayerController* PlayerController = Cast<AVBPlayerController>(Controller))
	{
		EnableInput(PlayerController);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//if (WeaponClass)
	//{
	//	//무기 액터 생성
	//	EquippeddWeapon = GetWorld()->SpawnActor<AWeaponActor>(WeaponClass);

	//	if (EquippeddWeapon)
	//	{
	//		//무기 장착
	//		EquippeddWeapon->EquipWeapon(this, "WeaponSocket");
	//	}
	//}
	
	HUD = Cast<AVBHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			float Speed = 300.f; //직선

			if (bIsRun)
			{
				Speed = 600.f;
				if (MovementVector.Y < 0) // 뒤로 가기
				{
					Speed = 300.f;
				}
				else if (MovementVector.X != 0) //좌우
				{
					Speed = 300.f;
					if (MovementVector.Y > 0)
					{
						Speed = 600.f;
					}
				}
			}
			else
			{
				if (MovementVector.Y < 0) // 뒤로 가기
				{
					Speed = 150.f;
				}
				else if (MovementVector.X != 0) //좌우
				{
					Speed = 175.f;
				}
				else if (MovementVector.X != 0 && MovementVector.Y > 0) //대각으로 가기
				{
					Speed = 200.f;
				}
			}
			

			GetCharacterMovement()->MaxWalkSpeed = Speed;

			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Run(const FInputActionValue& Value)
{
	bIsRun = true;
}

void APlayerCharacter::RunStop(const FInputActionValue& Value)
{
	bIsRun = false;
}

void APlayerCharacter::CameraBoomIn()
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength - 100.f, 300.f, 800.f);
}

void APlayerCharacter::CameraBoomOut()
{
	CameraBoom->TargetArmLength = FMath::Clamp(CameraBoom->TargetArmLength + 100.f, 300.f, 800.f);
}

void APlayerCharacter::Attack()
{
	if (!bIsRun && !(GetCharacterMovement()->IsFalling()))
	{
		ProcessComboCommand();
	}
}

void APlayerCharacter::ProcessComboCommand()
{
	if (CurrentCombo == 0)
	{
		ComboActionBegin();
		return;
	}

	if (!ComboTimerHandle.IsValid())
	{
		HasNextComboCommand = false;
	}
	else
	{
		HasNextComboCommand = true;
	}
}

void APlayerCharacter::ComboActionBegin()
{
	CurrentCombo = 1;

	const float AttackSpeedRate = 1.0f;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	
	AnimInstance->Montage_Play(AttackAnim,AttackSpeedRate);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &APlayerCharacter::ComboActionEnd);
	
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AttackAnim);

	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void APlayerCharacter::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);
	CurrentCombo = 0;

}

void APlayerCharacter::SetComboCheckTimer()
{
	int32 ComboIdx = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIdx));

	const float AttackSpeedRate = 1.0f;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIdx] / ComboActionData->FrameRate) / AttackSpeedRate;
	
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &APlayerCharacter::ComboCheck, ComboEffectiveTime, false);
	}


}

void APlayerCharacter::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	
	if (HasNextComboCommand)
	{
		UAnimInstance* AnimInstaance = GetMesh()->GetAnimInstance();

		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstaance->Montage_JumpToSection(NextSection, AttackAnim);

		SetComboCheckTimer();
		HasNextComboCommand = false;
	}
}

//void APlayerCharacter::ApplyDamage(AActor* Actor, float Damage)
//{
//	if (AEnemyBase* Enemy = Cast<AEnemyBase>(Actor))
//	{
//		Enemy->TakeDamage(Damage);
//	}
//}

void APlayerCharacter::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
}

void APlayerCharacter::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadAnim, 1.0f);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}


}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		//Running
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::RunStop);
		
		//Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Attack);
		
		//시점변경
		EnhancedInputComponent->BindAction(CamaraInAction, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraBoomIn);

		EnhancedInputComponent->BindAction(CamaraOutAction, ETriggerEvent::Triggered, this, &APlayerCharacter::CameraBoomOut);

		//채집
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerCharacter::EndInteract);


		//인벤토리
		EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleMenu);

	}
}

void APlayerCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector TraceStart{ GetPawnViewLocation() };
	FVector TraceEnd{ TraceStart + (GetViewRotation().Vector() * InteractionCheckDistance) };

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0)
	{
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		FHitResult TraceHit;

		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{

				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}



	NoInteractableFound();
}

void APlayerCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();
}

void APlayerCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
			EndInteract();
		}

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void APlayerCharacter::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("F Started"));

	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuaration, 0.1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction, this, &APlayerCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDuaration, false);
			}
		}
	}
}

void APlayerCharacter::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("F Ended"));

	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void APlayerCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void APlayerCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void APlayerCharacter::ToggleMenu()
{
	HUD->ToggleMenu();
}

void APlayerCharacter::DropItem(UItemBase* ItemToDrop, int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		FActorSpawnParameters SpawnParms;
		SpawnParms.Owner = this;
		SpawnParms.bNoFail = true;
		SpawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() * 50.0f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);

		const int32 RemovedQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(APickup::StaticClass(), SpawnTransform, SpawnParms);

		Pickup->InitializeDrop(ItemToDrop, RemovedQuantity);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null."));
	}
}

