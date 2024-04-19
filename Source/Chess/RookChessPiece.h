// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "RookChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API ARookChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	virtual TArray<FIntPoint> GetPossibleMovePositions() override;

	virtual int GetValue() const override;

private:
	int RookPieceSquareTable[8][8]
	{
		{  0,   0, 	 0,   0,   0,   0,   0,   0},
		{  5,  10, 	10,  10,  10,  10,  10,   5},
		{ -5, 	0, 	 0,   0,   0,   0,   0,  -5},
		{ -5, 	0, 	 0,   0,   0,   0,   0,  -5},
		{ -5, 	0, 	 0,   0,   0,   0,   0,  -5},
		{ -5, 	0, 	 0,   0,   0,   0,   0,  -5},
		{ -5, 	0, 	 0,   0,   0,   0,   0,  -5},
		{  0,   0,   0,   5,   5,   0,   0,   0}
	};
};
