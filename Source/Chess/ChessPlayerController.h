// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChessPlayerController.generated.h"

/**
 * 
 */

struct ChessMove
{
	class ABaseChessPiece* ChessPiece = nullptr;
	FIntPoint NewPosition;

	ChessMove()
	{

	}
	ChessMove(ABaseChessPiece* piece, FIntPoint index)
	{
		ChessPiece = piece;
		NewPosition = index;
	}
};

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

	UFUNCTION(BlueprintImplementableEvent)
	void SetAIPawnPromotion();


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
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

	bool AITurn = false;

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

	void CheckSpecialMoves(FIntPoint index);

	UFUNCTION(BlueprintCallable)
	void SetPawnPromotion(TSubclassOf<class ABaseChessPiece> ChosenPiece);

	UFUNCTION(BlueprintCallable)
	void SpawnPromotedPawn();

	void RandomAIMove();

	class AKingChessPiece* WhiteKing;
	AKingChessPiece* BlackKing;
	AKingChessPiece* PlayerKing;

	FTimerHandle AIvsAITimerHandle;

	UPROPERTY(EditAnywhere)
	bool AIvsAI;

	UPROPERTY(EditAnywhere)
	float AIRate;

	int Minimax(int depth, bool MaximizingPlayer, bool IsFirst);

	int Minimax(int depth, bool MaximizingPlayer, int alpha, int beta, bool IsFirst);

	ChessMove GeneratedMove;

	void GenerateMove();

	TArray<ChessMove> GetAllValidMoves(bool WhiteSide);

	TArray<AActor*> MoveChessPieces;
	TArray<FIntPoint> MoveOriginalPositions;
};