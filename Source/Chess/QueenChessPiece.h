// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "QueenChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AQueenChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	virtual TArray<FIntPoint> GetPossibleMovePositions() override;
};
