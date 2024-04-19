// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "KnightChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AKnightChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
		
public:
	virtual TArray<FIntPoint> GetPossibleMovePositions() override;

	virtual int GetValue() const override;

private:
	int KnightPieceSquareTable[8][8]
	{
		{-50, -40, -30, -30, -30, -30, -40, -50},
		{-40, -20, 	 0,   0,   0,   0, -20, -40},
		{-30, 	0, 	10,  15,  15,  10,   0, -30},
		{-30, 	5, 	15,  20,  20,  15,   5, -30},
		{-30, 	0, 	15,  20,  20,  15,   0, -30},
		{-30, 	5, 	10,  15,  15,  10,   5, -30},
		{-40, -20, 	 0,   5,   5,   0, -20, -40},
		{-50, -40, -30, -30, -30, -30, -40, -50}
	};
};
