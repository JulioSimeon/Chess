// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChessPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_API AChessPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AChessPlayerController();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputAction* SelectPieceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputAction* MoveSelectedPieceAction;
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PromotePawn();


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FName PlayerSide;

	FName EnemySide;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ValidCaptureSquare;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ValidMoveSquare;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseChessPiece> PawnPromotion;

	class AChessGameMode* ChessGameMode;

	UPROPERTY(EditDefaultsOnly)
	bool CoOp;

	class ABaseChessPiece* SelectedPiece;

	TArray<AActor*> ValidSquares;

	class AChessBoard* ChessBoard;

	void SelectPiece();
	void MoveSelectedPiece();
	void DisplayValidMoves();
	void DeleteValidMoveSquares();
	void SwitchSides();
	void BeginNextTurn();
	void UpdateSelectedPieceLocation(FIntPoint NewIndex, ABaseChessPiece* ChessPiece);
	bool ShouldPromotePawn();
	TArray<FIntPoint> GetValidMoves();

	UFUNCTION(BlueprintCallable)
	void SetPawnPromotion(TSubclassOf<class ABaseChessPiece> ChosenPiece);

	UFUNCTION(BlueprintCallable)
	void SpawnPromotedPawn();

	class AKingChessPiece* WhiteKing;
	AKingChessPiece* BlackKing;
	AKingChessPiece* PlayerKing;
};