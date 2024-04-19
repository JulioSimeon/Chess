// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseChessPiece.h"
#include "PawnChessPiece.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API APawnChessPiece : public ABaseChessPiece
{
	GENERATED_BODY()
	
public:
	virtual TArray<FIntPoint> GetPossibleMovePositions() override;

	virtual TArray<FIntPoint> GetPossibleMovePositionsForEnemy() override;

	virtual void SetCurrentPosition(FIntPoint NewPosition) override;

	APawnChessPiece* GetEnPassantPawn() const;

	virtual void MoveChessPiece(FIntPoint NewPosition) override;

	virtual int GetValue() const override;

protected:
	virtual void BeginPlay() override;

private:
	int Direction;
	APawnChessPiece* EnPassantPawn;
	void SetEnPassant(APawnChessPiece* PassedPawn);

	int PawnPieceSquareTable[8][8]
	{
		{  0,   0,   0,   0,   0,   0,   0,   0},
		{ 50,  50, 	50,  50,  50,  50,  50,  50},
		{ 10,  10, 	20,  30,  30,  20,  10,  10},
		{  5, 	5, 	10,  25,  25,  10,   5,   5},
		{  0, 	0, 	 0,  20,  20,   0,   0,   0},
		{  5,  -5, -10,   0,   0, -10,  -5,   5},
		{  5,  10, 	10, -20, -20,  10,  10,   5},
		{  0,   0,   0,   0,   0,   0,   0,   0}
	};	
};
