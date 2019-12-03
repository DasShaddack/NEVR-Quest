// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SceneCaptureComponent2D.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"

#include "VRSceneCaptureComponent2D.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class UVRSceneCaptureComponent2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()

public:

	// Toggles applying late HMD positional / rotational updates to the capture
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bTrackLocalHMDOrCamera;

	// If is an HMD enabled capture, is this the left eye
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsLeftEye;

	virtual void UpdateSceneCaptureContents(FSceneInterface* Scene) override
	{

		// Apply eye offset
		// Apply eye matrix
		// Apply late update

		if (bTrackLocalHMDOrCamera)
		{

			EStereoscopicPass StereoPass = bIsLeftEye ? EStereoscopicPass::eSSP_LEFT_EYE : eSSP_RIGHT_EYE;

			FQuat Orientation = FQuat::Identity;
			FVector Position = FVector::ZeroVector;

			if (GEngine->XRSystem.IsValid() && GEngine->IsStereoscopic3D() && GEngine->XRSystem->IsHeadTrackingAllowed() && GEngine->XRSystem->HasValidTrackingPosition())
			{
				GEngine->XRSystem->GetCurrentPose(0, Orientation, Position);
				FRotator Rotation = Orientation.Rotator();

				float WorldToMeters = GetWorld() ? GetWorld()->GetWorldSettings()->WorldToMeters : 100.0f;

				GEngine->StereoRenderingDevice->CalculateStereoViewOffset(StereoPass, Rotation, WorldToMeters /*1.0f?*/, Position);

				this->bUseCustomProjectionMatrix = true;

				float ActualFOV = 90.0f;
				if (GEngine->XRSystem.IsValid())
				{
					float HMDVerticalFOV, HMDHorizontalFOV;
					GEngine->XRSystem->GetHMDDevice()->GetFieldOfView(HMDHorizontalFOV, HMDVerticalFOV);
					if (HMDHorizontalFOV > 0)
					{
						ActualFOV = HMDHorizontalFOV;
					}
				}

				this->CustomProjectionMatrix = GEngine->StereoRenderingDevice->GetStereoProjectionMatrix(StereoPass);
				this->CaptureStereoPass = StereoPass;
			}
			else
			{
				this->bUseCustomProjectionMatrix = false;
				this->CaptureStereoPass = EStereoscopicPass::eSSP_FULL;

				APlayerController* Player = GetWorld()->GetFirstPlayerController();
				if (Player != nullptr && Player->IsLocalController())
				{
					if (Player->GetPawn())
					{
						for (UActorComponent* CamComponent : Player->GetPawn()->GetComponentsByClass(UCameraComponent::StaticClass()))
						{
							UCameraComponent * CameraComponent = Cast<UCameraComponent>(CamComponent);

							if (CameraComponent != nullptr && CameraComponent->bIsActive)
							{
								FTransform trans = CameraComponent->GetRelativeTransform();

								Orientation = trans.GetRotation();
								Position = trans.GetTranslation();
								break;
							}
						}
					}
				}
			}

			this->SetRelativeLocationAndRotation(Position, Orientation);
		}
		else
		{
			this->bUseCustomProjectionMatrix = false;
			this->CaptureStereoPass = EStereoscopicPass::eSSP_FULL;
		}

		// This pulls from the GetComponentToWorld so setting just prior to it should have worked    
		Super::UpdateSceneCaptureContents(Scene);
	}

};