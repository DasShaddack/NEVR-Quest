// Fill out your copyright notice in the Description page of Project Settings.


#include "TextLogger.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATextLogger::ATextLogger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATextLogger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATextLogger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool ATextLogger::FileSaveString(FString SaveTextB)
{
	FString fileName = FString(TEXT("TrialData/EventLog-"));

	fileName += FDateTime::UtcNow().ToString();
	fileName += ".txt";

	FString path = FPaths::ProjectLogDir();
	#if PLATFORM_ANDROID
		extern FString GFilePathBase;
		path = FString("/sdcard/UE4Game/NEVR_Quest/NEVR_Quest/Saved/");
	#endif

	return FFileHelper::SaveStringToFile(SaveTextB, *(path + fileName));
}

bool ATextLogger::FileSaveSurvey(FString SaveTextB)
{
	FString fileName = FString(TEXT("TrialData/Survey-"));

	fileName += FDateTime::UtcNow().ToString();
	fileName += ".txt";

	FString path = FPaths::ProjectLogDir();
#if PLATFORM_ANDROID
	extern FString GFilePathBase;
	path = FString("/sdcard/UE4Game/NEVR_Quest/NEVR_Quest/Saved/");
#endif

	return FFileHelper::SaveStringToFile(SaveTextB, *(path + fileName));
}
