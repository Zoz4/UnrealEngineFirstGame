// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWaveSpawnerActor.h"

// Sets default values
AMyWaveSpawnerActor::AMyWaveSpawnerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("UBillboardComponent"));
	FVector Origin = GetActorLocation();
	float range = 1000.0f;
	SpawnLocations = TArray<FVector>{
		Origin + FVector(range, 0.0f, 0.0f),
		Origin + FVector(-range, 0.0f, 0.0f),
		Origin + FVector(0.0, range, 0.0f),
		Origin + FVector(0.0, -range, 0.0f),
		Origin + FVector(range, range, 0.0f),
		Origin + FVector(range, -range, 0.0f),
		Origin + FVector(-range, range, 0.0f),
		Origin + FVector(-range, -range, 0.0f),
	};
	WaveNames.Add("Wave 1"); WaveNames.Add("Wave 2"); WaveNames.Add("Wave 3"); WaveNames.Add("WIN");
	
	for (int i = 0; i < WaveNames.Num()-1; ++i)
	{
		WaveEnemyNums.Add(WaveNames[i], 2 * i + 1);
	}
	WaveEnemyNums.Add(WaveNames[WaveNames.Num() - 1], 0);

}

// Called when the game starts or when spawned
void AMyWaveSpawnerActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyWaveSpawnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyWaveSpawnerActor::SpawnWaves()
{

	UWorld* World = GetWorld();
	if (World)
	{
		FRotator SpawnRotation = FRotator::ZeroRotator;
		// for loop
		if (CurrentWave < WaveNames.Num())
		{
			FString WaveName = WaveNames[CurrentWave++];
			for (int i = 0; i < WaveEnemyNums[WaveName]; ++i);
			{
				int32 index = FMath::RandRange(0, SpawnLocations.Num() - 1);
				AMyEnemyCharacter* EnemyCharacter = World->SpawnActor<AMyEnemyCharacter>(AMyEnemyCharacter::StaticClass(), SpawnLocations[index], SpawnRotation);
			}
		}
	}
}

