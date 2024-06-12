// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"



AMyHUD::AMyHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WB_MyHealthBar.WB_MyHealthBar_C'"));

	if (HealthBarWidgetBPClass.Class)
	{
		HealthBarWidgetClass = HealthBarWidgetBPClass.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found Health Bar"));
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> WavesWidgetBPClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UI/WB_MyWavesHUD.WB_MyWavesHUD_C'"));
	if (WavesWidgetBPClass.Class)
	{
		WavesWidgetClass = WavesWidgetBPClass.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found Waves Widget"));
	}

}
void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidgetClass)
	{
		HealthBarWidget = CreateWidget<UUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport(-114514);
		}
	}
	if (WavesWidgetClass)
	{
		WavesWidget = CreateWidget<UUserWidget>(GetWorld(), WavesWidgetClass);
		if (WavesWidget)
		{
			WavesWidget->AddToViewport(-114514);
		}
	}

}