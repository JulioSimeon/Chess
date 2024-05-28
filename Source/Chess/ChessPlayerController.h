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
	class UInputMappingContext* MoveMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputAction* SelectPieceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputAction* MoveSelectedPieceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputAction* UndoMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputAction* PauseGame;
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintImplementableEvent)
	void PromotePawn();

	UFUNCTION(BlueprintImplementableEvent)
	void PauseChessGame();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool LoserIsWhite);

	UFUNCTION(BlueprintImplementableEvent)
	void DrawGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool CoOp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool ShowPossibleMoves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PlayerIsWhite;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int MinimaxDepth = 4;

private:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ABaseChessPiece>> PawnPromotionChessPieces;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ValidCaptureSquare;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ValidMoveSquare;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ValidSelectSquare;

	class AChessGameMode* ChessGameMode;

	class ABaseChessPiece* SelectedPiece;

	TArray<AActor*> ValidSquares;

	TArray<AActor*> PastMoveSquares;

	class AChessBoard* ChessBoard;

	void SelectPiece();
	void MoveSelectedPiece();
	void DisplayValidMoves(ABaseChessPiece* ChessPiece);
	void DisplayMove(FIntPoint OldIndex, FIntPoint NewIndex);
	void DeleteValidMoveSquares();
	void DeletePastMoveSquares();
	void SwitchSides();
	void BeginNextTurn();
	bool ShouldPromotePawn(ABaseChessPiece* ChessPiece);

	bool HasEnemyLost(bool IsWhite);

	//Move Simulation
	void MakeMove(ABaseChessPiece* ChessPiece, FIntPoint NewIndex, bool IsSimulate);
	void UndoMove(bool IsSimulate);
	UPROPERTY(VisibleAnywhere)
	int MoveIndex{};
	UPROPERTY(VisibleAnywhere)
	TArray<ABaseChessPiece*> MovedPieces;
	UPROPERTY(VisibleAnywhere)
	TArray<ABaseChessPiece*> CapturedPieces;
	UPROPERTY(VisibleAnywhere)
	TArray<FIntPoint> OriginalLocations;
	UPROPERTY(VisibleAnywhere)
	TArray<FIntPoint> NewLocations;
	UPROPERTY(VisibleAnywhere)
	TArray<bool> FirstMove;
	TArray<TArray<class APawnChessPiece*>> PawnsWithEnPassant;
	UPROPERTY(VisibleAnywhere)
	TArray<class APawnChessPiece*> EnPassantPawns;
	UPROPERTY(VisibleAnywhere)
	TArray<ABaseChessPiece*> PromotedPawns;
	UPROPERTY(VisibleAnywhere)
	TArray<bool> Castled;

	TArray<FIntPoint> GetValidMoves(ABaseChessPiece* ChessPiece);

	UFUNCTION(BlueprintCallable)
	void SpawnPromotedPawn(int index);

	void SpawnPromotedPawn(ABaseChessPiece* ChessPiece);

	

	class AKingChessPiece* WhiteKing;
	AKingChessPiece* BlackKing;

	//Move Generation

	ChessMove MinimaxRoot(int depth, bool MaximizingPlayer);
	int Minimax(int depth, bool MaximizingPlayer, int alpha, int beta);
	void GenerateMove();
	const int infinity = 10000;

	TArray<ChessMove> GetAllValidMoves(bool IsWhite);

	TArray<FIntPoint> MoveOriginalPositions;

	//AI vs AI
	UPROPERTY(EditAnywhere)
	bool AIvsAI;
	UPROPERTY(EditAnywhere)
	float AIRate;
	FTimerHandle DelayTimerHandle;
	FTimerHandle AIvsAITimerHandle;

	//GettingValidMoves
	TArray<FIntPoint> GetPossibleMovesOfEnemyPieces(bool IsWhite) const;
	bool IsKingInCheck(bool IsWhite, TArray<FIntPoint> EnemyMoves) const;
	//TArray<FIntPoint> EnemyMoves;
	
	ABaseChessPiece* KingChecker;

	//Temp
	int MoveGenerationTest(int depth, bool White);
	UPROPERTY(EditAnywhere)
	int MoveGenerationTestDepth;
	UPROPERTY(EditAnywhere)
	bool TestMoveGeneration;
	void ResetAllEnPassant(bool IsWhite);

	class TArray<APawnChessPiece*> GetPawnsWithEnPassant(bool IsWhite);

	void UndoActualMove();

	//Audio
	UPROPERTY(EditAnywhere)
	class USoundBase* MoveSound;

	UPROPERTY(EditAnywhere)
	USoundBase* CaptureSound;

	UPROPERTY(EditAnywhere)
	USoundBase* CheckSound;

	UPROPERTY(EditAnywhere)
	USoundBase* CastlingSound;

	void SetCamera();
};