// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyAIController.h"
#include "MyEnemyCharacter.h"
void AMyEnemyAIController::MoveToLocation(const FVector& DestLocation)
{
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalLocation(DestLocation);
    MoveRequest.SetAcceptanceRadius(5.0f); // ���ýӽ�Ŀ���Ľ��ܰ뾶

    FNavPathSharedPtr NavPath;
    MoveTo(MoveRequest, &NavPath);
}

void AMyEnemyAIController::MoveToActor(AActor* TargetActor)
{
    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(5.0f); // ���ýӽ�Ŀ��Ľ��ܰ뾶

    FNavPathSharedPtr NavPath;
    MoveTo(MoveRequest, &NavPath);

}

void AMyEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    
    AMyEnemyCharacter* MyCharacter = Cast<AMyEnemyCharacter>(GetPawn());
    
    if (MyCharacter)
    {
    
        if(MyCharacter->Health > 0.0f)
            GetWorld()->GetTimerManager().SetTimer(PushTimeHandle, MyCharacter, &AMyEnemyCharacter::Push, 0.4f, false);
        GetWorld()->GetTimerManager().SetTimer(ChasePlayerTimeHandle, MyCharacter, &AMyEnemyCharacter::ChasePlayer, 1.0f, false);
   
    }
}

