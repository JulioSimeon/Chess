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
	bool CurrentSide;
	//true = white / false = black

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
	TArray<bool> Castled;

	TArray<FIntPoint> GetValidMoves(ABaseChessPiece* ChessPiece);

	UFUNCTION(BlueprintCallable)
	void SetPawnPromotion(TSubclassOf<class ABaseChessPiece> ChosenPiece);

	UFUNCTION(BlueprintCallable)
	void SpawnPromotedPawn();

	void SpawnPromotedPawn(ABaseChessPiece* ChessPiece);

	

	class AKingChessPiece* WhiteKing;
	AKingChessPiece* BlackKing;

	//Move Generation
	UPROPERTY(EditAnywhere)
	int MinimaxDepth = 4;
	ChessMove MinimaxRoot(int depth, bool MaximizingPlayer);
	int Minimax(int depth, bool MaximizingPlayer, int alpha, int beta);
	void RandomAIMove();
	void GenerateMove();
	const int infinity = 10000;
	
	
	void SynchronizeChessPieces() const;

	TArray<ChessMove> GetAllValidMoves(bool IsWhite);

	TArray<FIntPoint> MoveOriginalPositions;

	//AI vs AI
	UPROPERTY(EditAnywhere)
	bool AIvsAI;
	UPROPERTY(EditAnywhere)
	float AIRate;
	FTimerHandle AIvsAITimerHandle;

	//GettingValidMoves
	TArray<FIntPoint> GetPossibleMovesOfEnemyPieces(bool IsWhite) const;
	bool IsKingInCheck(bool IsWhite, TArray<FIntPoint> EnemyMoves) const;
	//TArray<FIntPoint> EnemyMoves;
	
	ABaseChessPiece* KingChecker;

	//Temp
	int CursedMinimax(int depth, bool MaximizingPlayer, int alpha, int beta, bool IsFirst);
	ChessMove GeneratedMoveTemp;
	bool first = true;
	UPROPERTY(EditAnywhere)
	bool TestCursedMinimax;
	int MoveGenerationTest(int depth, bool White);
	UPROPERTY(EditAnywhere)
	int MoveGenerationTestDepth;
	UPROPERTY(EditAnywhere)
	bool TestMoveGeneration;
	void ResetAllEnPassant(bool IsWhite);

	class TArray<APawnChessPiece*> GetPawnsWithEnPassant(bool IsWhite);
};