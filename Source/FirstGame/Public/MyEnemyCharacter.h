// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyEnemyAIController.h"




#include "MyEnemyCharacter.generated.h"




UCLASS()
class FIRSTGAME_API AMyEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	void Push();
	UFUNCTION()
	void OnPushEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackStarted(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	void ApplyDamage(AActor* DamagedActor, float BaseDamage);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void ChasePlayer();
	
protected:
private:
	void DestoryCharacter();


public:
	friend class AMyEnemyAIController;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UArrowComponent* ArrowComp;
private:
	UClass* CharacterAnimationClass;
	class UAnimMontage* PushMontage;
	inline void LoadAssets();

	bool bCanPush = true;
	float Health = 11.0f;
	float Damage = 10.0f;
	FTimerHandle DestoryTimerHandle;
	FScriptDelegate PushDelegateSubscriber;
	FScriptDelegate AttackStartDelegateSubscriber;
};
