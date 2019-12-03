#include "MyTestPortalActor.h"
#include "Kismet/GameplayStatics.h"


#include "Classes/Components/SceneCaptureComponent2D.h"
#include "Classes/Engine/TextureRenderTarget2D.h"
#include "Classes/Camera/CameraComponent.h"
#include "IXRTrackingSystem.h"
#include "IHeadMountedDisplay.h"

// Sets default values
AMyTestPortalActor::AMyTestPortalActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RDSceneCapComp2D = NULL;
}

// Called when the game starts or when spawned
void AMyTestPortalActor::BeginPlay()
{
	Super::BeginPlay();
	//this->SetTickGroup(TG_PostPhysics);
}

// Called every frame
void AMyTestPortalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//this->SetTickGroup(TG_PostPhysics);


}

float AMyTestPortalActor::CalculateFOV(APlayerController* playerController)
{
	float fov = 90.0f;

	if (playerController != NULL)
	{
		if (playerController->PlayerCameraManager != NULL)
		{
			fov = playerController->PlayerCameraManager->GetFOVAngle();
		}
	}

	// FOV changes when we have a VR Headset enabled
	if (GEngine->XRSystem->GetHMDDevice()->IsHMDEnabled() && GEngine->IsStereoscopic3D())
	{
		float HFOV, VFOV;
		GEngine->XRSystem->GetHMDDevice()->GetFieldOfView(HFOV, VFOV);
		if (VFOV > 0 && HFOV > 0)
		{
			fov = FMath::Max(HFOV, VFOV);
			// AspectRatio won't be used until bConstrainAspectRatio is set to true,
			// but it doesn't really matter since HMD calcs its own projection matrix.
			//OutViewInfo.AspectRatio = HFOV / VFOV;
			//OutViewInfo.bConstrainAspectRatio = true;
		}
	}

	return fov;
}

void AMyTestPortalActor::UpdatePortalVPMParameters(UMaterialInstanceDynamic* material, FTransform PlayerCameraXForm, APlayerController* playerController)
{
	float captureSizeX = RDSceneCapComp2D->TextureTarget->GetSurfaceWidth();
	float captureSizeY = RDSceneCapComp2D->TextureTarget->GetSurfaceHeight();
	const FTransform& Transform = PlayerCameraXForm;
	FMatrix ViewMatrix = Transform.ToInverseMatrixWithScale();
	FVector ViewLocation = Transform.GetTranslation();
	// swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
	ViewMatrix = ViewMatrix * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));
	const float FOV = RDSceneCapComp2D->FOVAngle * (float)PI / 360.0f;

	// Build projection matrix
	float XAxisMultiplier;
	float YAxisMultiplier;

	if (captureSizeX > captureSizeY)
	{
		// if the viewport is wider than it is tall
		XAxisMultiplier = 1.0f;
		YAxisMultiplier = captureSizeX / captureSizeY;
	}
	else
	{
		// if the viewport is taller than it is wide
		XAxisMultiplier = captureSizeY / captureSizeX;
		YAxisMultiplier = 1.0f;
	}

	FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
		FOV,
		FOV,
		XAxisMultiplier,
		YAxisMultiplier,
		10,
		1000
	);

	const FMatrix ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;

	FVector Xaxis = ViewProjectionMatrix.GetColumn(0);
	FVector Yaxis = ViewProjectionMatrix.GetColumn(1);
	FVector Zaxis = ViewProjectionMatrix.GetColumn(2);
	FVector Waxis = ViewProjectionMatrix.GetColumn(3);

	float XaxisW = ViewProjectionMatrix.M[3][0];
	float YaxisW = ViewProjectionMatrix.M[3][1];
	float ZaxisW = ViewProjectionMatrix.M[3][2];
	float WaxisW = ViewProjectionMatrix.M[3][3];

	material->SetVectorParameterValue("PortalVPM_Xaxis", FLinearColor(Xaxis.X, Xaxis.Y, Xaxis.Z, XaxisW));
	material->SetVectorParameterValue("PortalVPM_Yaxis", FLinearColor(Yaxis.X, Yaxis.Y, Yaxis.Z, YaxisW));
	material->SetVectorParameterValue("PortalVPM_Zaxis", FLinearColor(Zaxis.X, Zaxis.Y, Zaxis.Z, ZaxisW));
	material->SetVectorParameterValue("PortalVPM_Waxis", FLinearColor(Waxis.X, Waxis.Y, Waxis.Z, WaxisW));
}

FVector2D AMyTestPortalActor::GetHMDResolution()
{
	if (GEngine->XRSystem->GetHMDDevice()->IsHMDEnabled() && GEngine->IsStereoscopic3D())
	{
		IHeadMountedDisplay::MonitorInfo info;
		GEngine->XRSystem->GetHMDDevice()->GetHMDMonitorInfo(info);
		return FVector2D(info.ResolutionX, info.ResolutionY);
	}



	return FVector2D(1920, 1080);
}

FVector2D AMyTestPortalActor::GetHMDEyeDists()
{
	float eyeDist = GEngine->XRSystem->GetHMDDevice()->GetInterpupillaryDistance();
	float lensDist = GEngine->XRSystem->GetHMDDevice()->GetLensCenterOffset();

	FVector2D dists = FVector2D(eyeDist, lensDist);

	return dists;
}


void AMyTestPortalActor::UpdateSSCProjs(USceneCaptureComponent2D* eye, bool isLeftEye)
{
	eye->bUseCustomProjectionMatrix = true;

	if (isLeftEye) {
		eye->CustomProjectionMatrix = GEngine->StereoRenderingDevice->GetStereoProjectionMatrix(eSSP_LEFT_EYE);
	}
	else {
		eye->CustomProjectionMatrix = GEngine->StereoRenderingDevice->GetStereoProjectionMatrix(eSSP_RIGHT_EYE);
	}
}


FVector AMyTestPortalActor::GetOffsetLeft()
{
	FRotator rotation = FRotator(0.0f);
	float worldToMeters = GEngine->XRSystem->GetWorldToMetersScale();
	FVector	location = FVector(0.0f, 0.0f, 0.0f);

	GEngine->StereoRenderingDevice->CalculateStereoViewOffset(eSSP_LEFT_EYE, rotation, worldToMeters, location);

	return location;
}

FVector AMyTestPortalActor::GetOffsetRight()
{
	FRotator rotation = FRotator(0.0f);
	float worldToMeters = GEngine->XRSystem->GetWorldToMetersScale();
	FVector	location = FVector(0.0f, 0.0f, 0.0f);

	GEngine->StereoRenderingDevice->CalculateStereoViewOffset(eSSP_RIGHT_EYE, rotation, worldToMeters, location);

	return location;
}

FRotator AMyTestPortalActor::GetRotRight()
{
	FRotator rotation = FRotator(0.0f);
	float worldToMeters = GEngine->XRSystem->GetWorldToMetersScale();
	FVector	location = FVector(0.0f, 0.0f, 0.0f);

	GEngine->StereoRenderingDevice->CalculateStereoViewOffset(eSSP_RIGHT_EYE, rotation, worldToMeters, location);

	return rotation;
}