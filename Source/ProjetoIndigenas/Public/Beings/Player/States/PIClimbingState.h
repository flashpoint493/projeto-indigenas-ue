﻿#pragma once

#include "CoreMinimal.h"
#include "Misc/Vectors.h"
#include "Beings/Shared/States/PIStateBase.h"
#include "Beings/Player/PICharacterAnimInstance.h"

class UCharacterMovementComponent;
class APIClimbableTree;

struct PROJETOINDIGENAS_API FPIClimbingStateData
{
	const float CapsuleRadius;
	const float CapsuleRadiusAcceleration;
	const float MovementSpeedAcceleration;
	const float RotationAcceleration;

	FPIClimbingStateData(
		const float& capsuleRadius,
		const float& capsuleRadiusAcceleration,
		const float& movementSpeedAcceleration,
		const float& rotationAcceleration)
		: CapsuleRadius(capsuleRadius),
		  CapsuleRadiusAcceleration(capsuleRadiusAcceleration),
		  MovementSpeedAcceleration(movementSpeedAcceleration),
		  RotationAcceleration(rotationAcceleration)
	{ }
};

UENUM(BlueprintType)
enum class EPIClimbingState : uint8
{
	None,
	StartClimbing,
	HangingOnTreeIdle,
	HangingOnTreeMoving,
	EndClimbing
};

typedef FPIAnimatedStateBaseWithData<UPICharacterAnimInstance, FPIClimbingStateData> FPIClimbingStateBase;

class PROJETOINDIGENAS_API FPIClimbingState : public FPIClimbingStateBase
{
	TWeakObjectPtr<UCharacterMovementComponent> _characterMovement;
	
	FAcceleratedVector2D _acceleratedLocation;
	FAcceleratedVector _acceleratedDirection;
	
	FAcceleratedValue _acceleratedMovementSpeed;
	FAcceleratedValue _acceleratedCapsuleRadius;

	EPICharacterAnimationState* _characterAnimState;

	EPIClimbingState _currentState;

	float _inputValue;
	FVector _startDirection;
	bool _isAtTop;
	bool _isAtBottom;

	void SetInputY(float y);
	void UpdateMovementSpeed();
	void UpdateTargetLocation();
	void SynchronizeCharacterLocation(const float& deltaSeconds);

	void ClimbingStarted();
	void ClimbingEnded();

	void ClampLocationToPath();
	void SetTreeCameraCollision(ECollisionResponse response) const;
	
public:
	TWeakObjectPtr<APIClimbableTree> Tree;
	
	explicit FPIClimbingState(APICharacterBase* character, const FPIClimbingStateData& stateData);

	virtual void BindInput(const TSharedRef<FPIInputDelegates>& inputDelegates) override;
	virtual void UnbindInput(const TSharedRef<FPIInputDelegates>& inputDelegates) override;
	
	virtual void Enter() override;
	virtual void Exit(FPIStateOnExitDelegate onExitDelegate) override;

	virtual void Tick(float DeltaSeconds) override;

#if !UE_BUILD_SHIPPING
private:
	inline static int _logKey = 0;

	FORCEINLINE static int GetLogKey() { return ++_logKey; }
	
	FAutoConsoleVariable _logsEnabled = FAutoConsoleVariable(
		TEXT("pi.climbing.screenLogs"),
		false,
		TEXT("Displays useful logs for debugging"));
#endif
};
