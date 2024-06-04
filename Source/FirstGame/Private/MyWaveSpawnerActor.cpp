// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWaveSpawnerActor.h"

// Sets default values
AMyWaveSpawnerActor::AMyWaveSpawnerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("UBillboardComponent"));

	static ConstructorHelpers::FObjectFinder<USoundWave> SoundWaveAsset(TEXT("/Script/Engine.SoundWave'/Game/Audio/Wave.Wave'"));
	if (SoundWaveAsset.Object)
	{
		ClearStageSound = SoundWaveAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound Wave Not Found!"));
	}
	WaveNames.Add("START");
	WaveNames.Add("Wave 1"); WaveNames.Add("Wave 2"); WaveNames.Add("Wave 3");
	WaveNames.Add("WIN");
	
	WaveEnemyNums.Add(WaveNames[0], 0);
	for (int i = 1; i < WaveNames.Num()-1; ++i)
	{
		WaveEnemyNums.Add(WaveNames[i], 2 * i - 1);
	}
	WaveEnemyNums.Add(WaveNames[WaveNames.Num() - 1], 0);

	SpawnEnemiesDelegate.BindUObject(this, &AMyWaveSpawnerActor::SpawnWaves);

}

// Called when the game starts or when spawned
void AMyWaveSpawnerActor::BeginPlay()
{
	Super::BeginPlay();
	FVector Origin = BillboardComponent->GetComponentLocation();
	UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), Origin.X, Origin.Y, Origin.Z);
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
	SpawnWaves();
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
		UGameplayStatics::PlaySound2D(World, ClearStageSound);
		++CurrentWave;
		FRotator SpawnRotation = FRotator::ZeroRotator;
		// for loop
		if (CurrentWave < WaveNames.Num())
		{

			FString WaveName = WaveNames[CurrentWave];
			int32 EnemyNums = *(WaveEnemyNums.Find(WaveName));


			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			AMyPlayerCharacter* MyPlayerCharacter = Cast<AMyPlayerCharacter>(PlayerController->GetPawn());
			FVector PlayerLocation = MyPlayerCharacter->GetActorLocation();

			for (int32 i = 0; i < EnemyNums; ++i)
			{
				int32 index = FMath::RandRange(0, SpawnLocations.Num() - 1);

				FVector SpawnLocation = SpawnLocations[index];
				SpawnLocation.X += PlayerLocation.X;
				SpawnLocation.Y += PlayerLocation.Y;

				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				World->SpawnActor<AMyEnemyCharacter>(AMyEnemyCharacter::StaticClass(), SpawnLocation, SpawnRotation, SpawnParameters);

				
			}
		}
	}
}

