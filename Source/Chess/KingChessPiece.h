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

private:
	TArray<FIntPoint> GetPossibleMovesOfEnemyPieces() const;

	bool bIsCastling = false;
};
