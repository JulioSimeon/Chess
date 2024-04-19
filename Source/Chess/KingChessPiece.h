// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "KingChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AKingChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	virtual TArray<FIntPoint> GetPossibleMovePositions() override;

	virtual TArray<FIntPoint> GetPossibleMovePositionsForEnemy() override;

	virtual void MoveChessPiece(FIntPoint NewPosition) override;

	bool IsCastling() const;

	bool IsChecked() const;

	virtual int GetValue() const override;

private:
	TArray<FIntPoint> GetPossibleMovesOfEnemyPieces() const;

	bool bIsCastling = false;

	int KingPieceSquareTable[8][8]
	{
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-30, -40, -40, -50, -50, -40, -40, -30},
		{-20, -30, -30, -40, -40, -30, -30, -20},
		{-10, -20, -20, -20, -20, -20, -20, -10},
		{ 20,  20, 	 0,   0,   0,   0,  20,  20},
		{ 20,  30,  10,   0,   0,  10,  30,  20}
	};	
};
