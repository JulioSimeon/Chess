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
	AQueenChessPiece();

	virtual TArray<FIntPoint> GetPossibleMovePositions() override;

	virtual int GetValue() const override;

	virtual TArray<FIntPoint> GetPossibleMovePositionsForEnemy() override;

private:
	int QueenPieceSquareTable[8][8]
	{
		{-20, -10, -10,  -5,  -5, -10, -10, -20},
		{-10, 	0, 	 0,   0,   0,   0,   0, -10},
		{-10, 	0, 	 5,   5,   5,   5,   0, -10},
		{ -5, 	0, 	 5,   5,   5,   5,   0,  -5},
		{  0, 	0, 	 5,   5,   5,   5,   0,  -5},
		{-10, 	5, 	 5,   5,   5,   5,   0, -10},
		{-10, 	0, 	 5,   0,   0,   0,   0, -10},
		{-20, -10, -10,  -5,  -5, -10, -10, -20}
	};
};
