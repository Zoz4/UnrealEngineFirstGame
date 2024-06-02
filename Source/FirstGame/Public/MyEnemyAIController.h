// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "MyPlayerCharacter.h"

#include "MyEnemyAIController.generated.h"

/**
 * 
 */

UCLASS()
class FIRSTGAME_API AMyEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	void MoveToLocation(const FVector& DestLocation);
	void MoveToActor(AActor* TargetActor);
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	FTimerHandle PushTimeHandle;
	FTimerHandle ChasePlayerTimeHandle;
};
