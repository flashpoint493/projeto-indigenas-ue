﻿#include "Beings/Shared/PICharacterBase.h"

#include "WaterBodyActor.h"
#include "Beings/Player/States/PIClimbingState.h"
#include "Beings/Shared/States/PIMovementState.h"
#include "Beings/Shared/States/PIStateBase.h"

void APICharacterBase::SetCurrentState(const TSharedPtr<FPIStateBase>& state)
{
	auto setCurrentLambda = [this, state]
	{
		_currentState = state;

		if (!_currentState.IsValid()) return;

		if (_inputDelegates.IsValid())
		{
			_currentState->BindInput(_inputDelegates.ToSharedRef());
		}
			
		_currentState->Enter();
	};
	
	if (_currentState.IsValid())
	{
		if (_currentState->CanExit())
		{
			if (_inputDelegates.IsValid())
			{
				_currentState->UnbindInput(_inputDelegates.ToSharedRef());
			}

			_currentState->Exit(FPIStateOnExitDelegate::CreateLambda(setCurrentLambda));
		}

		return;
	}

	setCurrentLambda();
}

void APICharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void APICharacterBase::CreateMovementState(const float& capsuleRadius, const float& movementAcceleration)
{
	_movementState = MakeShared<FPIMovementState>(this,
		FPIMovementStateData(
			capsuleRadius,
			_capsuleRadiusAcceleration,
			_rotationAcceleration,
			movementAcceleration
		));
}

void APICharacterBase::CreateClimbingState(const float& capsuleRadius, const float& movementAcceleration)
{
	_climbingState = MakeShared<FPIClimbingState>(this,
		FPIClimbingStateData(
			capsuleRadius,
			_capsuleRadiusAcceleration,
			movementAcceleration,
			_rotationAcceleration
		));
}

void APICharacterBase::CreateSwimmingState(const float& capsuleRadius, const float& movementAcceleration)
{
	_swimmingState = MakeShared<FPISwimmingState>(this,
		FPISwimmingStateData(
			capsuleRadius,
			_capsuleRadiusAcceleration,
			_rotationAcceleration,
			movementAcceleration
		));
}

void APICharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!_currentState.IsValid()) return;

	_currentState->Tick(DeltaSeconds);

	// fixing Npc crash while it's missing implementation
	if (!_swimmingState.IsValid()) return;
	
	// TODO(anderson): should this really be here?
	AWaterBody* waterBody = _waterBodyActor.Get();
	
	if (_currentState != _swimmingState)
	{
		if (_swimmingState->CanStartSwimming(waterBody))
		{
			StartSwimming(waterBody);
		}
	}

	if (_currentState == _swimmingState)
	{
		if (_swimmingState->CanEndSwimming())
		{
			EndSwimming();
		}
	}
}

void APICharacterBase::SetInputDelegates(const TSharedPtr<FPIInputDelegates>& inputDelegates)
{
	if (_currentState.IsValid())
	{
		_currentState->UnbindInput(inputDelegates.ToSharedRef());
	}
	
	_inputDelegates = inputDelegates;

	if (_currentState.IsValid())
	{
		_currentState->BindInput(inputDelegates.ToSharedRef());
	}
}

void APICharacterBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// fixing Npc crash while it's missing implementation
	if (!_swimmingState.IsValid()) return;
	
	_waterBodyActor = Cast<AWaterBody>(OtherActor);
	if (_currentState == _swimmingState)
	{
		_swimmingState->WaterBody = _waterBodyActor;
	}
}

void APICharacterBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (_waterBodyActor == OtherActor)
	{
		_waterBodyActor = nullptr;
	}
}
