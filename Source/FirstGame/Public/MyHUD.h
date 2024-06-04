// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "MyHUD.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTGAME_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	

public:
	friend class AMyEnemyCharacter;
	friend class AMyPlayerCharacter;
	AMyHUD();
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> WavesWidgetClass;

	UUserWidget* HealthBarWidget;
	UUserWidget* WavesWidget;
};
