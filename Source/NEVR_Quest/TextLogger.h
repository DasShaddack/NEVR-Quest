// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TextLogger.generated.h"

UCLASS()
class NEVR_QUEST_API ATextLogger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATextLogger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Save")
		bool FileSaveString(FString SaveTextB);

	UFUNCTION(BlueprintCallable, Category = "Save")
		bool FileSaveSurvey(FString SaveTextB);

};
