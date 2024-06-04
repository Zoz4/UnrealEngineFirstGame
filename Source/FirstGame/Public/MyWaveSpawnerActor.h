// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/BillboardComponent.h"
#include "MyEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


#include "MyWaveSpawnerActor.generated.h"

DECLARE_DELEGATE(FSpawnEnemiesSignore);


UCLASS()
class FIRSTGAME_API AMyWaveSpawnerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWaveSpawnerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

public:
	void SpawnWaves();
protected:
private:

public:
	FSpawnEnemiesSignore SpawnEnemiesDelegate;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentWave = 0;
protected:

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> WaveNames;
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, int32> WaveEnemyNums;

private:
	TArray<FVector> SpawnLocations;
	UBillboardComponent* BillboardComponent;

	USoundWave* ClearStageSound;

};
