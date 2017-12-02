// Copyright Evohunt 2017

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

#define OUT

/// Sets default values for this component's properties
UGrabber::UGrabber()
{
	/// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	/// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


/// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
	
}

/// Look for attached Physics Handle
void UGrabber::FindPhysicsHandleComponent() {

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr) {

		UE_LOG(LogTemp, Error, TEXT("%s missing pysics handle component"), *GetOwner()->GetName());

	}

}

/// Look for attached Input Component (only appears at run time)
void UGrabber::SetupInputComponent() {

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent) {

		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);

	}
	else {

		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName());

	}

}

void UGrabber::Grab() {

	// Try and reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent(); /// Gets the mesh in our case
	auto ActorHit = HitResult.GetActor();

	// If we hit something then attach a physics handle
	if (ActorHit) {
		
		if (!PhysicsHandle) { return; }
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None, // No bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true // allow rotation
		);

	}
}

void UGrabber::Release() {

	if (!PhysicsHandle) { return; }
	PhysicsHandle->ReleaseComponent();

}

/// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) { return; }
	/// if the physics handle is attached
	if (PhysicsHandle->GrabbedComponent) {
	
		/// move the object that we are holding
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());

	}
	
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach() {

	/// Line-Trace out to reach distance
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		ECC_PhysicsBody
	);

	return HitResult;

}

FVector UGrabber::GetReachLineStart() {

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;

}

FVector UGrabber::GetReachLineEnd() {

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * reach;

}