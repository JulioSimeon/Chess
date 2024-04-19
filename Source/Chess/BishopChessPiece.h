// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "BishopChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API ABishopChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	virtual TArray<FIntPoint> GetPossibleMovePositions() override;

	virtual int GetValue() const override;

private:
	int BishopPieceSquareTable[8][8]
	{
		{-20, -10, -10, -10, -10, -10, -10, -20},
		{-10,   0, 	 0,   0,   0,   0,   0, -10},
		{-10, 	0, 	 5,  10,  10,   5,   0, -10},
		{-10, 	5, 	 5,  10,  10,   5,   5, -10},
		{-10, 	0, 	10,  10,  10,  10,   0, -10},
		{-10,  10, 	10,  10,  10,  10,  10, -10},
		{-10,   5, 	 0,   0,   0,   0,   5, -10},
		{-20, -10, -10, -10, -10, -10, -10, -20}
	};	
};
