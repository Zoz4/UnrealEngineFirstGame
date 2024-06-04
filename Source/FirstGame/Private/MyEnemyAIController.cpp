// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyAIController.h"
#include "MyEnemyCharacter.h"
void AMyEnemyAIController::MoveToLocation(const FVector& DestLocation)
{
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(DestLocation);
    MoveRequest.SetAcceptanceRadius(5.0f); // 设置接近目标点的接受半径

    FNavPathSharedPtr NavPath;
    MoveTo(MoveRequest, &NavPath);
}

void AMyEnemyAIController::MoveToActor(AActor* TargetActor)
{
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(5.0f); // 设置接近目标的接受半径

    FNavPathSharedPtr NavPath;
    MoveTo(MoveRequest, &NavPath);

}

void AMyEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    
    AMyEnemyCharacter* MyEnemyCharacter = Cast<AMyEnemyCharacter>(GetPawn());
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AMyPlayerCharacter* MyPlayerCharacter = Cast<AMyPlayerCharacter>(PlayerController->GetPawn());
    if (MyEnemyCharacter&& MyPlayerCharacter&& MyPlayerCharacter->Health>=0.0f)
    {
       
        if(MyEnemyCharacter->Health > 0.0f && MyPlayerCharacter->GetDistanceTo(MyEnemyCharacter)<120.0f)
            GetWorld()->GetTimerManager().SetTimer(PushTimeHandle, MyEnemyCharacter, &AMyEnemyCharacter::Push, 0.4f, false);
        GetWorld()->GetTimerManager().SetTimer(ChasePlayerTimeHandle, MyEnemyCharacter, &AMyEnemyCharacter::ChasePlayer, 1.0f, false);
   
    }
}

