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

	void reset()
	{
		ChessPiece = nullptr;
		NewPosition = {0, 0};
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
	FName CurrentSide;

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
	void DisplayValidMoves(ABaseChessPiece* ChessPiece);
	void DeleteValidMoveSquares();
	void SwitchSides();
	void BeginNextTurn();
	void UpdateSelectedPieceLocation(FIntPoint NewIndex, ABaseChessPiece* ChessPiece);
	bool ShouldPromotePawn();

	bool HasEnemyLost();

	//Move Simulation
	void SimulateMove(ABaseChessPiece* ChessPiece, FIntPoint NewIndex);
	void UndoMove();
	int SimulateIndex{};
	TArray<ABaseChessPiece*> SimulatedMovedPieces;
	TArray<ABaseChessPiece*> SimulatedCapturedPieces;
	TArray<FIntPoint> OriginalLocations;
	TArray<FIntPoint> NewLocations;
	TArray<FName> OriginalCapturedPieceSide;

	TArray<FIntPoint> GetValidMoves(ABaseChessPiece* ChessPiece);

	void CheckSpecialMoves(FIntPoint index, ABaseChessPiece* ChessPiece);

	UFUNCTION(BlueprintCallable)
	void SetPawnPromotion(TSubclassOf<class ABaseChessPiece> ChosenPiece);

	UFUNCTION(BlueprintCallable)
	void SpawnPromotedPawn();

	

	class AKingChessPiece* WhiteKing;
	AKingChessPiece* BlackKing;
	AKingChessPiece* CurrentKing;
	AKingChessPiece* EnemyKing;

	//Move Generation
	UPROPERTY(EditAnywhere)
	int MinimaxDepth = 4;
	ChessMove GeneratedMove;
	int Minimax(int depth, bool MaximizingPlayer, int alpha, int beta, bool IsFirst);
	void RandomAIMove();
	void GenerateMove();
	
	
	void SynchronizeChessPieces() const;

	TArray<ChessMove> GetAllValidMoves(bool IsWhite);

	TArray<FIntPoint> MoveOriginalPositions;

	//AI vs AI
	UPROPERTY(EditAnywhere)
	bool AIvsAI;
	UPROPERTY(EditAnywhere)
	float AIRate;
	FTimerHandle AIvsAITimerHandle;
};