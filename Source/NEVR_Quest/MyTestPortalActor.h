// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/SceneCapture2D.h"
#include "Classes/Components/SceneCaptureComponent2D.h"

#include "MyTestPortalActor.generated.h"


UCLASS()
class AMyTestPortalActor : public AActor
{
	GENERATED_BODY()


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RDSceneCapComp")
		USceneCaptureComponent2D* RDSceneCapComp2D;

	// Sets default values for this actor's properties
	AMyTestPortalActor();



	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		float CalculateFOV(APlayerController* playerController);

	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		void UpdatePortalVPMParameters(UMaterialInstanceDynamic* material, FTransform PlayerCameraXForm, APlayerController* playerController);


	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		FVector2D GetHMDResolution();

	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		FVector2D GetHMDEyeDists();

	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		void UpdateSSCProjs(USceneCaptureComponent2D* eye, bool isLeftEye);


	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		FVector GetOffsetLeft();
	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		FVector GetOffsetRight();
	UFUNCTION(BlueprintCallable, Category = "NonEuclidean")
		FRotator GetRotRight();
};