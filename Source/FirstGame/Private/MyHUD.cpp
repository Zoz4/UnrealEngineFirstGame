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
}
void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HealthBarWidgetClass)
	{
		HealthBarWidget = CreateWidget<UUserWidget>(GetWorld(), HealthBarWidgetClass);
		if (HealthBarWidget)
		{
			HealthBarWidget->AddToViewport();
		}
	}

}