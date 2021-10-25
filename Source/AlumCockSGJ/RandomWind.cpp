// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomWind.h"
#include  "DrawDebugHelpers.h"
#include "Profiler/Public/ProfilerCommon.h"

// Sets default values
ARandomWind::ARandomWind()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARandomWind::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARandomWind::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LoopWindCreation();
}
void ARandomWind::CreateWindDirection()
{
	
	FVector Location;
	FRotator Rotation;
	FHitResult HitResult;

	Location = GetActorLocation();
	Rotation = LineRotation;
	
	FVector Start = Location;
	FVector End = Start + (Rotation.Vector()*2000);
	FCollisionQueryParams TraceParams;
	GetWorld()->LineTraceSingleByChannel(HitResult,Start,End,ECC_Visibility,TraceParams);
	DrawDebugLine(GetWorld(),Start,End,FColor::Red,false, 0.1f);
	
}
void ARandomWind::LoopWindCreation()
{
	for( float i=0;i<=360; ++i)
	{
		
		LineRotation =  FRotator(0,i,0);
		
		CreateWindDirection();
	}
	
	
	}
