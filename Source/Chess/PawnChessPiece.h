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
		
protected:
	virtual void BeginPlay() override;

private:
	int Direction;
	APawnChessPiece* EnPassantPawn;
	void SetEnPassant(APawnChessPiece* PassedPawn);
};
