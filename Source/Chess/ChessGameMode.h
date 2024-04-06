// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ChessGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AChessGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	void PieceCaptured(AActor* CapturedPiece);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
};
