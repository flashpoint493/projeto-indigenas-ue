﻿#pragma once

#include "CoreMinimal.h"
#include "Beings/Player/Input/PIInputDelegates.h"

class APICharacterBase;
class UCapsuleComponent;

DECLARE_DELEGATE(FPIStateOnExitDelegate)

class PROJETOINDIGENAS_API FPIStateBase
{
	FPIStateOnExitDelegate _onExitDelegate;
	
protected:
	TWeakObjectPtr<APICharacterBase> _character;
	TWeakObjectPtr<UCapsuleComponent> _capsuleComponent;

	void InvokeOnExitDelegate();

public:
	explicit FPIStateBase(APICharacterBase* character);

	virtual ~FPIStateBase() = default;

	virtual void BindInput(const TSharedRef<FPIInputDelegates>& inputDelegates) { }
	virtual void UnbindInput(const TSharedRef<FPIInputDelegates>& inputDelegates) { }
	
	virtual void Enter() { }
	virtual void Exit(FPIStateOnExitDelegate onExitDelegate = nullptr);

	virtual bool CanEnter();
	virtual bool CanExit();

	virtual void Tick(float DeltaSeconds) { }
};

template <typename TAnimInstance>
class PROJETOINDIGENAS_API FPIAnimatedStateBase : public FPIStateBase
{
protected:
	TWeakObjectPtr<APICharacterBase> _character;

public:
	explicit FPIAnimatedStateBase(APICharacterBase* character);
	
	TAnimInstance* GetAnimInstance() const;
};

template <typename TStateData>
class PROJETOINDIGENAS_API FPIStateBaseWithData : public FPIStateBase
{
protected:
	const TStateData& _stateData;

public:
	FPIStateBaseWithData(APICharacterBase* character, const TStateData& stateData);
};

template <typename TAnimInstance, typename TStateData>
class PROJETOINDIGENAS_API FPIAnimatedStateBaseWithData : public FPIAnimatedStateBase<TAnimInstance>
{
protected:
	const TStateData _stateData;
	
public:
	FPIAnimatedStateBaseWithData(APICharacterBase* character, const TStateData& stateData);
};
