// Fill out your copyright notice in the Description page of Project Settings.

#include "ChessPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Actor.h"
#include "BaseChessPiece.h"
#include "ChessBoard.h"
#include "Kismet/GameplayStatics.h"
#include "PawnChessPiece.h"
#include "KingChessPiece.h"
#include "ChessGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraActor.h"
#include "GenericPlatform/GenericPlatformMath.h"

AChessPlayerController::AChessPlayerController()
{
    
}

void AChessPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetCamera();

    if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }

    //Initialize GameMode
    ChessGameMode = Cast<AChessGameMode>(UGameplayStatics::GetGameMode(this));

    //Initialize ChessBoard
    TArray<AActor*> ChessBoards;
    UGameplayStatics::GetAllActorsOfClass(this, AChessBoard::StaticClass(), ChessBoards);
    if(ChessBoards.Num() > 0)
    {
        ChessBoard = Cast<AChessBoard>(ChessBoards[0]);
    }

    //Initialize Kings
    TArray<AActor*> Kings;
    UGameplayStatics::GetAllActorsOfClass(this, AKingChessPiece::StaticClass(), Kings);

    if(Kings.Num() > 0)
    {
        for(auto king : Kings)
        {
            if(AKingChessPiece* KingPiece = Cast<AKingChessPiece>(king))
            {
                KingPiece->IsWhite() ? WhiteKing = KingPiece : BlackKing = KingPiece;
            }
        }
    }

    if(AIvsAI)
    {
        GetWorldTimerManager().SetTimer(AIvsAITimerHandle, this, &AChessPlayerController::GenerateMove, AIRate, true);
    }
    if(!CoOp && !PlayerIsWhite)
    {
        GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &AChessPlayerController::GenerateMove, AIRate, false);
        //GenerateMove();
    }
    
    if(CoOp)
    {
        PlayerIsWhite = true;
    }
}

void AChessPlayerController::SelectPiece()
{
    if(TestMoveGeneration)
    {
        double startSeconds = FPlatformTime::Seconds();
        UE_LOG(LogTemp, Warning, TEXT("Move Generation Test: %d"), MoveGenerationTest(MoveGenerationTestDepth, true));
        UE_LOG(LogTemp, Warning, TEXT("Time elapsed: %f seconds"), FPlatformTime::Seconds() - startSeconds);
    }
    DeleteValidMoveSquares();
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    //Check if piece selected is valid
    SelectedPiece = Cast<ABaseChessPiece>(HitResult.GetActor());
    if(SelectedPiece && SelectedPiece->IsWhite() == PlayerIsWhite)
    {
        //Display Valid Moves
        DisplayValidMoves(SelectedPiece);
        if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(MoveMappingContext, 1);
        }
    }    
}

void AChessPlayerController::DisplayValidMoves(ABaseChessPiece* ChessPiece)
{
    TArray<FIntPoint> ValidMoves; //= GetValidMoves(ChessPiece);
    TArray<ChessMove> Moves = GetAllValidMoves(ChessPiece->IsWhite());
    if(Moves.Num() > 0)
    {
        for(auto move : Moves)
        {
            if(move.ChessPiece == ChessPiece)
            {
                ValidMoves.Emplace(move.NewPosition);
            }
        }
    }

    FVector AdjustedPieceLocation = ChessBoard->GetLocation(ChessPiece->GetCurrentPosition());
    AdjustedPieceLocation.Z = 0.f;
    AActor* SelectedPieceSquare = GetWorld()->SpawnActor<AActor>(ValidSelectSquare, AdjustedPieceLocation, FRotator::ZeroRotator);
    
    if(SelectedPieceSquare)
    {
        ValidSquares.Emplace(SelectedPieceSquare);
    }

    if(ValidMoves.Num() > 0)
    {
        for(const auto& location : ValidMoves)
        {
            FVector AdjustedLocation = ChessBoard->GetLocation(location);
            AdjustedLocation.Z = 0.f;
            for(auto PastMoveSquare : PastMoveSquares)
            {
                if(PastMoveSquare->GetActorLocation() == AdjustedLocation)
                {
                    PastMoveSquare->SetActorLocation(PastMoveSquare->GetActorLocation() + FVector{0, 0, -1});
                }
            }
            AActor* ValidSquare;
            if(ChessBoard->GetChessPiece(location) == nullptr)
            {
                ValidSquare = GetWorld()->SpawnActor<AActor>(ValidMoveSquare, AdjustedLocation, FRotator::ZeroRotator); 
            }
            else
            {
                ValidSquare = GetWorld()->SpawnActor<AActor>(ValidCaptureSquare, AdjustedLocation, FRotator::ZeroRotator); 
            }
            if(ValidSquare)
            {
                ValidSquares.Emplace(ValidSquare);
                if(!ShowPossibleMoves)
                {
                    ValidSquare->SetActorHiddenInGame(true);
                }
            }
        }
    }
}

void AChessPlayerController::DisplayMove(FIntPoint OldIndex, FIntPoint NewIndex)
{
    TArray<FIntPoint> Indices = {OldIndex, NewIndex};
    for(auto index : Indices)
    {
        FVector AdjustedLocation = ChessBoard->GetLocation(index);
        AdjustedLocation.Z = 0.f;
        AActor* PastMoveSquare = GetWorld()->SpawnActor<AActor>(ValidSelectSquare, AdjustedLocation, FRotator::ZeroRotator);
        
        if(PastMoveSquare)
        {
            PastMoveSquares.Emplace(PastMoveSquare);
        }
    }
    
}

void AChessPlayerController::DeleteValidMoveSquares()
{
    if(ValidSquares.Num() > 0)
    {
        for(AActor* MoveSquare : ValidSquares)
        {
            MoveSquare->Destroy();
        }
        ValidSquares.Empty();
    }
}

void AChessPlayerController::DeletePastMoveSquares()
{
    if(PastMoveSquares.Num() > 0)
    {
        for(AActor* PastMoveSquare : PastMoveSquares)
        {
            PastMoveSquare->Destroy();
        }
        PastMoveSquares.Empty();
    }
}

void AChessPlayerController::MoveSelectedPiece()
{
    if(SelectedPiece)
    {
        FIntPoint OriginalIndex = SelectedPiece->GetCurrentPosition();
        if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
        {
            Subsystem->RemoveMappingContext(MoveMappingContext);
        }
        //Check if Player chose valid move location
        FHitResult HitResult;
        GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
        AActor* ChosenSquare = HitResult.GetActor();
        ABaseChessPiece* EnemyPiece = Cast<ABaseChessPiece>(ChosenSquare);
        if(EnemyPiece && EnemyPiece->IsWhite() != SelectedPiece->IsWhite())
        {
            //Check if EnemyPiece location is same as any of the validsquares
            FVector AdjustedEnemyLocation = EnemyPiece->GetActorLocation();
            AdjustedEnemyLocation.Z = 0.f;
            for(AActor* Square : ValidSquares)
            {
                if(AdjustedEnemyLocation == Square->GetActorLocation())
                {
                    ChosenSquare = Square;
                }
            }    
        }
        if(ValidSquares.Find(ChosenSquare) != INDEX_NONE)
        {
            FIntPoint index = ChessBoard->GetIndex(ChosenSquare->GetActorLocation());
            UE_LOG(LogTemp, Display, TEXT("Index: %s"), *index.ToString());
            if(index != SelectedPiece->GetCurrentPosition())
            {
                if(index != FIntPoint(-1, -1))
                {
                    MakeMove(SelectedPiece, index, false);
                }

                if(!ShouldPromotePawn(SelectedPiece))
                {
                    BeginNextTurn();
                }
            }
        }
        else if(ABaseChessPiece* FriendlyPiece = Cast<ABaseChessPiece>(ChosenSquare))
        {
            if(FriendlyPiece->IsWhite() == PlayerIsWhite)
            {
                SelectPiece();
            }
        }
    }
}

TArray<FIntPoint> AChessPlayerController::GetValidMoves(ABaseChessPiece* ChessPiece)
{
    TArray<FIntPoint> ValidMoves;
    TArray<FIntPoint> EnemyMoves = GetPossibleMovesOfEnemyPieces(ChessPiece->IsWhite());
    for(FIntPoint move : ChessPiece->GetPossibleMovePositions())
    {
        MakeMove(ChessPiece, move, true);

        //if king is not in check add move to ValidMoves
        if(!IsKingInCheck(ChessPiece->IsWhite(), EnemyMoves))
        {
            ValidMoves.Emplace(move);
        }
        
        UndoMove(true);
    }
    return ValidMoves;
}

void AChessPlayerController::SwitchSides()
{
    PlayerIsWhite = !PlayerIsWhite;
}

void AChessPlayerController::BeginNextTurn()
{
    DeleteValidMoveSquares();
    //Since no more piece selected, reset SelectedPiece to nullptr 
    SelectedPiece = nullptr;
    //Check if enemy side lost(whether it is AI or human enemy)
    if(HasEnemyLost(!PlayerIsWhite))
    {
        //EndGame
        GameOver(!PlayerIsWhite);
        return;
    }
    if(GetAllValidMoves(!PlayerIsWhite).Num() == 0)
    {
        //draw
        DrawGame();
        return;
    }
    if(CoOp)
    {
        SwitchSides();
    }
    else // Player vs Computer
    {
        //AI move
        GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &AChessPlayerController::GenerateMove, AIRate, false);
        //GenerateMove();
        //Check if enemy side lost(human enemy)
        
    }
}

bool AChessPlayerController::ShouldPromotePawn(ABaseChessPiece* ChessPiece)
{
    if(ChessPiece->GetType() == Type::Pawn)
    {
        if(ChessPiece->GetCurrentPosition().Y == 0 || ChessPiece->GetCurrentPosition().Y == 7)
        {
            return true;
        }
    }
    return false;
}

bool AChessPlayerController::HasEnemyLost(bool IsWhite)
{
    TArray<FIntPoint> EnemyMoves = GetPossibleMovesOfEnemyPieces(IsWhite);
    if(IsKingInCheck(IsWhite, EnemyMoves))
    {
        if(GetAllValidMoves(IsWhite).Num() == 0)
        {
            return true;
        }
        else
        {
            for(auto move : GetAllValidMoves(IsWhite))
            {
                UE_LOG(LogTemp, Warning, TEXT("Possible Moves: %s to %s"), *move.ChessPiece->GetActorNameOrLabel(), *move.NewPosition.ToString());
            }
        }
    }
    return false;
}

void AChessPlayerController::MakeMove(ABaseChessPiece* ChessPiece, FIntPoint NewIndex, bool IsSimulate)
{
    MovedPieces.EmplaceAt(MoveIndex, ChessPiece);
    NewLocations.EmplaceAt(MoveIndex, NewIndex);
    OriginalLocations.EmplaceAt(MoveIndex, ChessPiece->GetCurrentPosition());
    FirstMove.EmplaceAt(MoveIndex, ChessPiece->IsFirstMove());
    Castled.EmplaceAt(MoveIndex, false);
    PromotedPawns.EmplaceAt(MoveIndex, nullptr);

    //if move location has an enemy piece
    ABaseChessPiece* EnemyPiece = ChessBoard->GetChessPiece(NewIndex);
    TArray<APawnChessPiece*> PawnWithEnPassant = GetPawnsWithEnPassant(ChessPiece->IsWhite());
    TArray<APawnChessPiece*> EmptyArray;
    PawnWithEnPassant.Num() > 0 ? PawnsWithEnPassant.EmplaceAt(MoveIndex, PawnWithEnPassant) : PawnsWithEnPassant.EmplaceAt(MoveIndex, EmptyArray);
    PawnWithEnPassant.Num() > 0 && PawnWithEnPassant[0]->GetEnPassantPawn() ? EnPassantPawns.EmplaceAt(MoveIndex, PawnWithEnPassant[0]->GetEnPassantPawn()) : EnPassantPawns.EmplaceAt(MoveIndex, nullptr);
    if(EnemyPiece && EnemyPiece->IsWhite() != ChessPiece->IsWhite())
    {
        CapturedPieces.EmplaceAt(MoveIndex, EnemyPiece);
        ChessBoard->RemoveChessPiece(EnemyPiece);
    }
    //En Passant Check
    else if(PawnWithEnPassant.Contains(ChessPiece) && NewIndex.X == PawnWithEnPassant[0]->GetEnPassantPawn()->GetCurrentPosition().X)
    {   
        APawnChessPiece* EnPassantPawn = PawnWithEnPassant[0]->GetEnPassantPawn();
        CapturedPieces.EmplaceAt(MoveIndex, EnPassantPawn);
        ChessBoard->SetChessPiece(EnPassantPawn->GetCurrentPosition(), nullptr);  
        ChessBoard->RemoveChessPiece(EnPassantPawn);
    }
    else
    {
        CapturedPieces.EmplaceAt(MoveIndex, nullptr);
    }
    
    //Castling Check
    //Check if ChessPiece is King and if it has CastledRook
    if(AKingChessPiece* KingPiece = Cast<AKingChessPiece>(ChessPiece))
    {
        if(KingPiece->IsCastling())
        {
            //Move CastledRook to new location depending selected location
            if(NewIndex.X == 2)
            {   
                if(ABaseChessPiece* RookPiece = ChessBoard->GetChessPiece(FIntPoint(0, NewIndex.Y)))
                {
                    Castled[MoveIndex] = true;
                    FIntPoint NewRookIndex = FIntPoint(RookPiece->GetCurrentPosition().X + 3, RookPiece->GetCurrentPosition().Y);
                    //Set original index to nullptr
                    ChessBoard->SetChessPiece(RookPiece->GetCurrentPosition(), nullptr);
                    //Update location of ChessPiece to NewIndex
                    RookPiece->MoveChessPiece(NewRookIndex);
                    ChessBoard->SetChessPiece(NewRookIndex, RookPiece);
                    RookPiece->SetActorLocation(ChessBoard->GetLocation(NewRookIndex));
                    if(!IsSimulate && CastlingSound)
                    {
                        UGameplayStatics::PlaySound2D(this, CastlingSound);
                    }
                }
            }
            else if(NewIndex.X == 6)
            {
                if(ABaseChessPiece* RookPiece = ChessBoard->GetChessPiece(FIntPoint(7, NewIndex.Y)))
                {
                    FIntPoint NewRookIndex = FIntPoint(RookPiece->GetCurrentPosition().X - 2, RookPiece->GetCurrentPosition().Y);
                    Castled[MoveIndex] = true;
                    //Set original index to nullptr
                    ChessBoard->SetChessPiece(RookPiece->GetCurrentPosition(), nullptr);
                    //Update location of ChessPiece to NewIndex
                    RookPiece->MoveChessPiece(NewRookIndex);
                    ChessBoard->SetChessPiece(NewRookIndex, RookPiece);
                    RookPiece->SetActorLocation(ChessBoard->GetLocation(NewRookIndex));
                    if(!IsSimulate && CastlingSound)
                    {
                        UGameplayStatics::PlaySound2D(this, CastlingSound);
                    }
                }
            } 
        }
    }

    //If enemy piece captured and not simulated, move to new location
    if(CapturedPieces[MoveIndex])
    {
        FVector LocationOffset = {0, 0, 1000};
        CapturedPieces[MoveIndex]->SetActorLocation(CapturedPieces[MoveIndex]->GetActorLocation() + LocationOffset);
        if(CaptureSound && !IsSimulate)
        {
            UGameplayStatics::PlaySound2D(this, CaptureSound);
        }
    }
    //Move SelectedPiece to chosen location
    ChessPiece->SetActorLocation(ChessBoard->GetLocation(NewIndex));
    if(!IsSimulate && MoveSound)
    {
        DeletePastMoveSquares();
        DisplayMove(OriginalLocations[MoveIndex], NewLocations[MoveIndex]);
        UGameplayStatics::PlaySound2D(this, MoveSound);
    }

    //Set original index to nullptr
    ChessBoard->SetChessPiece(ChessPiece->GetCurrentPosition(), nullptr);
    //Update location of ChessPiece to NewIndex
    ChessPiece->MoveChessPiece(NewIndex);
    ChessBoard->SetChessPiece(NewIndex, ChessPiece);
    //Reset EnPassant Pawn if in case it did not move
    for(auto pawn : PawnWithEnPassant)
    {
        pawn->ResetEnPassant();
    }
    //ResetAllEnPassant(ChessPiece->IsWhite());
    //Pawn Promotion Special Move
    //Check if Pawn has reached end of chessboard
    if(ShouldPromotePawn(ChessPiece))
    {
        //Spawn Widget
        //User chooses which chess piece pawn will be promoted to
        PromotedPawns[MoveIndex] = ChessPiece;
        //if Player's turn promote pawn else computer's turn
        SelectedPiece ? PromotePawn() : SpawnPromotedPawn(ChessPiece);
    }   
    MoveIndex++;
}

void AChessPlayerController::UndoMove(bool IsSimulate)
{
    MoveIndex--;
    
    //Undo pawn promotion if occured
    if(PromotedPawns[MoveIndex])
    {
        if(ABaseChessPiece* PromotedPawn = ChessBoard->GetChessPiece(NewLocations[MoveIndex]))
        {
            ChessBoard->DeleteChessPiece(PromotedPawn);
            PromotedPawn->Destroy();
            ChessBoard->SetChessPiece(NewLocations[MoveIndex], PromotedPawns[MoveIndex]);
            ChessBoard->ReviveChessPiece(PromotedPawns[MoveIndex]);
        }
    }
    //Reset castled rook if any
    if(Castled[MoveIndex] && MovedPieces[MoveIndex]->GetType() == Type::King)
    {
        if(MovedPieces[MoveIndex]->GetCurrentPosition().X == 2 && ChessBoard->GetChessPiece(FIntPoint(3, MovedPieces[MoveIndex]->GetCurrentPosition().Y))->GetType() == Type::Rook)
        {
            ABaseChessPiece* RookPiece = ChessBoard->GetChessPiece(FIntPoint(3, MovedPieces[MoveIndex]->GetCurrentPosition().Y));
            ChessBoard->SetChessPiece(RookPiece->GetCurrentPosition(), nullptr);
            RookPiece->SetCurrentPosition(FIntPoint(0, RookPiece->GetCurrentPosition().Y));
            ChessBoard->SetChessPiece(RookPiece->GetCurrentPosition(), RookPiece);
            RookPiece->SetIsFirstMove(true);
            RookPiece->SetActorLocation(ChessBoard->GetLocation(RookPiece->GetCurrentPosition()));
        }   
        else if(MovedPieces[MoveIndex]->GetCurrentPosition().X == 6 && ChessBoard->GetChessPiece(FIntPoint(5, MovedPieces[MoveIndex]->GetCurrentPosition().Y))->GetType() == Type::Rook)
        {
            ABaseChessPiece* RookPiece = ChessBoard->GetChessPiece(FIntPoint(5, MovedPieces[MoveIndex]->GetCurrentPosition().Y));
            ChessBoard->SetChessPiece(RookPiece->GetCurrentPosition(), nullptr);
            RookPiece->SetCurrentPosition(FIntPoint(7, RookPiece->GetCurrentPosition().Y));
            ChessBoard->SetChessPiece(RookPiece->GetCurrentPosition(), RookPiece);
            RookPiece->SetIsFirstMove(true);
            RookPiece->SetActorLocation(ChessBoard->GetLocation(RookPiece->GetCurrentPosition()));
        }
    }
    ChessBoard->SetChessPiece(NewLocations[MoveIndex], nullptr);
    //if EnemyPiece exists reset location to move index
    if(CapturedPieces[MoveIndex])
    {
        ChessBoard->ReviveChessPiece(CapturedPieces[MoveIndex]);
        ChessBoard->SetChessPiece(CapturedPieces[MoveIndex]->GetCurrentPosition(), CapturedPieces[MoveIndex]);
    }
    //reset ChessPiece location to original index
    ChessBoard->SetChessPiece(OriginalLocations[MoveIndex], MovedPieces[MoveIndex]);
    MovedPieces[MoveIndex]->SetCurrentPosition(OriginalLocations[MoveIndex]);
    
    //reset ChessPiece bIsFirstMove to original value
    if(FirstMove[MoveIndex])
    {
        MovedPieces[MoveIndex]->SetIsFirstMove(true);
    }

    ResetAllEnPassant(MovedPieces[MoveIndex]->IsWhite());
    //set back enpassant pawn if it existed
    
    if(PawnsWithEnPassant[MoveIndex].Num() > 0 && EnPassantPawns[MoveIndex])
    {
        for(auto pawn : PawnsWithEnPassant[MoveIndex])
        {
            pawn->SetEnPassant(EnPassantPawns[MoveIndex]);
        }
        
    }

    MovedPieces[MoveIndex]->SetActorLocation(ChessBoard->GetLocation(MovedPieces[MoveIndex]->GetCurrentPosition()));

    if(CapturedPieces[MoveIndex])
    {
        CapturedPieces[MoveIndex]->SetActorLocation(ChessBoard->GetLocation(CapturedPieces[MoveIndex]->GetCurrentPosition()));
    }

    MovedPieces.RemoveAt(MoveIndex);
    CapturedPieces.RemoveAt(MoveIndex);
    NewLocations.RemoveAt(MoveIndex);
    OriginalLocations.RemoveAt(MoveIndex);
    FirstMove.RemoveAt(MoveIndex);
    PawnsWithEnPassant.RemoveAt(MoveIndex);
    EnPassantPawns.RemoveAt(MoveIndex);
    Castled.RemoveAt(MoveIndex);
    PromotedPawns.RemoveAt(MoveIndex);
    //ChessBoard->PrintChessPieces();
}

void AChessPlayerController::SpawnPromotedPawn(int index)
{
    if(SelectedPiece) UE_LOG(LogTemp, Warning, TEXT("Selected Piece: %s"), *SelectedPiece->GetActorNameOrLabel());

    FIntPoint SpawnIndex = MovedPieces[MoveIndex-1]->GetCurrentPosition();
    //Remove Pawn
    ChessBoard->RemoveChessPiece(MovedPieces[MoveIndex-1]);
    
    FVector LocationOffset = {0, 0, 1000};
    MovedPieces[MoveIndex-1]->SetActorLocation(MovedPieces[MoveIndex-1]->GetActorLocation() + LocationOffset);
    
    index = MovedPieces[MoveIndex-1]->IsWhite() ? index : index + 1;
    //Spawn chosen chess piece and set selectedpiece to chosen piece
    if(ABaseChessPiece* PromotedPawn = Cast<ABaseChessPiece>(GetWorld()->SpawnActor<AActor>(PawnPromotionChessPieces[index], ChessBoard->GetLocation(SpawnIndex), FRotator::ZeroRotator)))
    {
        PromotedPawn->SetCurrentPosition(SpawnIndex);
        ChessBoard->AddChessPiece(PromotedPawn);
        ChessBoard->SetChessPiece(SpawnIndex, PromotedPawn);
    }
    BeginNextTurn();
}

void AChessPlayerController::SpawnPromotedPawn(ABaseChessPiece* ChessPiece)
{
    if(ChessPiece)
    {
        FIntPoint SpawnIndex = ChessPiece->GetCurrentPosition();
        //Remove Pawn
        ChessBoard->RemoveChessPiece(ChessPiece);

        FVector LocationOffset = {0, 0, 1000};
        ChessPiece->SetActorLocation(ChessPiece->GetActorLocation() + LocationOffset);
        
        //Spawn chosen chess piece and set selectedpiece to chosen piece
        int index = ChessPiece->IsWhite() ? 0 : 1;
        if(ABaseChessPiece* PromotedPawn = Cast<ABaseChessPiece>(GetWorld()->SpawnActor<AActor>(PawnPromotionChessPieces[index], ChessBoard->GetLocation(SpawnIndex), FRotator::ZeroRotator)))
        {
            PromotedPawn->SetCurrentPosition(SpawnIndex);
            ChessBoard->AddChessPiece(PromotedPawn);
            ChessBoard->SetChessPiece(SpawnIndex, PromotedPawn);
        }
    }
}

ChessMove AChessPlayerController::MinimaxRoot(int depth, bool MaximizingPlayer)
{   
    int MaxEval = -infinity;
    int MinEval = infinity;
    ChessMove BestMove;
    for(ChessMove& move : GetAllValidMoves(MaximizingPlayer))
    {   
        MakeMove(move.ChessPiece, move.NewPosition, true);
        int eval = Minimax(depth - 1, !MaximizingPlayer, -infinity, infinity);
        if(MaximizingPlayer)
        {
            MaxEval = FGenericPlatformMath::Max(MaxEval, eval);
            UE_LOG(LogTemp, Warning, TEXT("Move: %s to %s Eval: %d"), *move.ChessPiece->GetActorNameOrLabel(), *move.NewPosition.ToString(), eval);
            if(eval == MaxEval)
            {
                BestMove = move;
            }
        }
        else
        {
            MinEval = FGenericPlatformMath::Min(MinEval, eval);
            UE_LOG(LogTemp, Warning, TEXT("Move: %s to %s Eval: %d"), *move.ChessPiece->GetActorNameOrLabel(), *move.NewPosition.ToString(), eval);
            if(eval == MinEval)
            {
                BestMove = move;
            }
        }
        UndoMove(true);
    }
    return BestMove;
}

int AChessPlayerController::Minimax(int depth, bool MaximizingPlayer, int alpha, int beta)
{
    if(depth == 0) // or game over
    {   
        return ChessBoard->Evaluate();
    }
    else if(GetAllValidMoves(true).Num() == 0)
    {
        return -infinity;
    }
    else if(GetAllValidMoves(false).Num() == 0)
    {
        return infinity;
    }
    else
    {
        if(MaximizingPlayer)
        {
            int MaxEval = -infinity;
            for(ChessMove& move : GetAllValidMoves(MaximizingPlayer))
            {
                MakeMove(move.ChessPiece, move.NewPosition, true);

                int eval = Minimax(depth - 1, false, alpha, beta);
                MaxEval = FGenericPlatformMath::Max(MaxEval, eval);
                alpha = FGenericPlatformMath::Max(alpha, eval);

                UndoMove(true);
            
                if(beta <= alpha)
                {
                    break;
                }
            }
            //UE_LOG(LogTemp, Display, TEXT("White Move: %s to %s Eval : %d"), *TempMove.ChessPiece->GetActorNameOrLabel(), *TempMove.NewPosition.ToString(), MaxEval);
            return MaxEval;
        }
        else
        {
            int MinEval = infinity;
            for(ChessMove& move : GetAllValidMoves(MaximizingPlayer))
            {
                MakeMove(move.ChessPiece, move.NewPosition, true);

                int eval = Minimax(depth - 1, true, alpha, beta);
                MinEval = FGenericPlatformMath::Min(MinEval, eval);
                beta = FGenericPlatformMath::Min(beta, eval);

                UndoMove(true);

                if(beta <= alpha)
                {
                    break;
                }
            }
            //UE_LOG(LogTemp, Display, TEXT("Black Move: %s to %s Eval : %d"), *TempMove.ChessPiece->GetActorNameOrLabel(), *TempMove.NewPosition.ToString(), MinEval);
            return MinEval;
        }
    }
    
}

void AChessPlayerController::GenerateMove()
{   
    ChessMove GeneratedMove;
    if(!PlayerIsWhite)
    {
        GeneratedMove = MinimaxRoot(MinimaxDepth, true);
    }
    else
    {
        GeneratedMove = MinimaxRoot(MinimaxDepth, false);
    }
    if(GeneratedMove.ChessPiece)
    {
        UE_LOG(LogTemp, Warning, TEXT("Chosen Actor %s Chosen Index %s"), *GeneratedMove.ChessPiece->GetActorNameOrLabel(), *GeneratedMove.NewPosition.ToString());
        MakeMove(GeneratedMove.ChessPiece, GeneratedMove.NewPosition, false);
    }

    if(AIvsAI)
    {
        SwitchSides();
    }

    if(HasEnemyLost(PlayerIsWhite))
    {
        //EndGame
        GameOver(PlayerIsWhite);
        return;
    }
    if(GetAllValidMoves(PlayerIsWhite).Num() == 0)
    {
        //draw
        DrawGame();
        return;
    }
}

TArray<ChessMove> AChessPlayerController::GetAllValidMoves(bool IsWhite)
{
    
    AKingChessPiece* King = IsWhite ? WhiteKing : BlackKing;
    TArray<ChessMove> ValidMoves;
    TArray<FIntPoint> EnemyMoves = GetPossibleMovesOfEnemyPieces(IsWhite);
    TArray<ABaseChessPiece*> EnemyChessPieces = ChessBoard->GetChessPieces(!IsWhite);
    TArray<ABaseChessPiece*> ChessPieces = ChessBoard->GetChessPieces(IsWhite);
    TArray<ABaseChessPiece*> PinnedChessPieces;

    //If King is in check
    if(IsKingInCheck(IsWhite, EnemyMoves)) //ABaseChessPiece* CheckPiece = IsKingInCheck(IsWhite)
    {
        ABaseChessPiece* CheckPiece = MovedPieces[MoveIndex - 1]; //to be changed find better method to find checkpiece
        TArray<FIntPoint> MovesToEndCheck;
        MovesToEndCheck.Add(CheckPiece->GetCurrentPosition());
        int CheckPieceX = CheckPiece->GetCurrentPosition().X;
        int CheckPieceY = CheckPiece->GetCurrentPosition().Y;
        int KingChessPieceX = King->GetCurrentPosition().X;
        int KingChessPieceY = King->GetCurrentPosition().Y;
        //if chess piece of last move is a sliding piece
        //getting orange squares
        if(CheckPiece->IsSlidingPiece())
        {
            if(CheckPieceX == KingChessPieceX)
            {
                int MaxY = FGenericPlatformMath::Max(CheckPieceY, KingChessPieceY);
                int MinY = FGenericPlatformMath::Min(CheckPieceY, KingChessPieceY);
                for(int i{MinY + 1}; i < MaxY; i++)
                {
                    MovesToEndCheck.Add(FIntPoint(CheckPieceX, i));
                }
            }
            else if(CheckPieceY == KingChessPieceY)
            {
                int MaxX = FGenericPlatformMath::Max(CheckPieceX, KingChessPieceX);
                int MinX = FGenericPlatformMath::Min(CheckPieceX, KingChessPieceX);
                for(int i{MinX + 1}; i < MaxX; i++)
                {
                    MovesToEndCheck.Add(FIntPoint(i, CheckPieceY));
                }
            }
            else
            {
                int YDirection = (CheckPieceY < KingChessPieceY) ? 1 : -1;
                int XDirection = (CheckPieceX < KingChessPieceX) ? 1 : -1;
                for(int i = 1; i < UKismetMathLibrary::Abs_Int(CheckPieceY - KingChessPieceY); i++)
                {
                    MovesToEndCheck.Add(FIntPoint(CheckPieceX + (XDirection * i), CheckPieceY + (YDirection * i)));
                }
                
            }
        }

        //Determine what pieces are pinned (if king is in check, they cannot move because they will cause another check(therefore they are pinned))
        for(auto EnemyChessPiece : EnemyChessPieces)
        {
            if(EnemyChessPiece->IsSlidingPiece())
            {
                ABaseChessPiece* FPinnedPiece = nullptr;
                int EnemyChessPieceX = EnemyChessPiece->GetCurrentPosition().X;
                int EnemyChessPieceY = EnemyChessPiece->GetCurrentPosition().Y;
                if((EnemyChessPiece->GetType() == Type::Rook || EnemyChessPiece->GetType() == Type::Queen) && (EnemyChessPieceX == KingChessPieceX || EnemyChessPieceY == KingChessPieceY))
                {
                    if(EnemyChessPieceX == KingChessPieceX)
                    {
                        int MaxY = FGenericPlatformMath::Max(EnemyChessPieceY, KingChessPieceY);
                        int MinY = FGenericPlatformMath::Min(EnemyChessPieceY, KingChessPieceY);
                        for(int i{MinY + 1}; i < MaxY; i++)
                        {
                            //if there is a chess piece
                            if(ABaseChessPiece* PinnedChessPiece = ChessBoard->GetChessPiece(FIntPoint(EnemyChessPieceX, i)))
                            {
                                //if there are the same color break because pinned piece no longer possible
                                if(PinnedChessPiece->IsWhite() == EnemyChessPiece->IsWhite())
                                {
                                    FPinnedPiece = nullptr;
                                    break;
                                }
                                else
                                {
                                     //If another PinnedChessPiece found then no pinned chess piece
                                    if(FPinnedPiece)
                                    {
                                        FPinnedPiece = nullptr;
                                        break;
                                    }
                                    FPinnedPiece = PinnedChessPiece;
                                }
                            }
                        }
                        if(FPinnedPiece)
                        {
                            PinnedChessPieces.Emplace(FPinnedPiece);
                        }
                    }
                    else if(EnemyChessPieceY == KingChessPieceY)
                    {
                        int MaxX = FGenericPlatformMath::Max(EnemyChessPieceX, KingChessPieceX);
                        int MinX = FGenericPlatformMath::Min(EnemyChessPieceX, KingChessPieceX);
                        for(int i{MinX + 1}; i < MaxX; i++)
                        {
                            if(ABaseChessPiece* PinnedChessPiece = ChessBoard->GetChessPiece(FIntPoint(i, EnemyChessPieceY)))
                            {
                                if(PinnedChessPiece->IsWhite() == EnemyChessPiece->IsWhite())
                                {
                                    FPinnedPiece = nullptr;
                                    break;
                                }
                                else
                                {
                                    if(FPinnedPiece)
                                    {
                                        FPinnedPiece = nullptr;
                                        break;
                                    }
                                    FPinnedPiece = PinnedChessPiece;
                                }
                            }
                        }
                        if(FPinnedPiece)
                        {
                            PinnedChessPieces.Emplace(FPinnedPiece);
                        }
                    }
                }
                else if(EnemyChessPiece->GetType() == Type::Bishop || EnemyChessPiece->GetType() == Type::Queen)
                {
                    if(UKismetMathLibrary::Abs_Int(EnemyChessPieceX - KingChessPieceX) == UKismetMathLibrary::Abs_Int(EnemyChessPieceY - KingChessPieceY))
                    {
                        int YDirection = (EnemyChessPieceY < KingChessPieceY) ? 1 : -1;
                        int XDirection = (EnemyChessPieceX < KingChessPieceX) ? 1 : -1;
                        for(int i = 1; i < UKismetMathLibrary::Abs_Int(EnemyChessPieceY - KingChessPieceY); i++)
                        {
                            if(ABaseChessPiece* PinnedChessPiece = ChessBoard->GetChessPiece(FIntPoint(EnemyChessPieceX + (XDirection * i), EnemyChessPieceY + (YDirection * i))))
                            {
                                if(PinnedChessPiece->IsWhite() == EnemyChessPiece->IsWhite())
                                {
                                    FPinnedPiece = nullptr;
                                    break;
                                }
                                else
                                {
                                    if(FPinnedPiece)
                                    {
                                        FPinnedPiece = nullptr;
                                        break;
                                    }
                                    FPinnedPiece = PinnedChessPiece;
                                }
                            }                            
                        }
                        if(FPinnedPiece)
                        {
                            PinnedChessPieces.Emplace(FPinnedPiece);
                        }
                    }
                }
            }
        }
        //Check which chess pieces can block or capture enemy pieces that are putting the king in check if Piece is not pinned / will result in another check
        for(auto ChessPiece : ChessPieces)
        {
            if(!PinnedChessPieces.Contains(ChessPiece) ) //&& ChessPiece->GetType() != Type::King
            {
                //check if piece possible move positions include blocking or capturing threating enemy piece - will need array of moves that will satisfy this to compare against
                if(ChessPiece->GetType() != Type::King)
                {
                    for(auto move : ChessPiece->GetPossibleMovePositions())
                    {
                        if(MovesToEndCheck.Contains(move) )
                        {
                            ValidMoves.Emplace(ChessPiece, move);
                        }
                    }    
                }
            }
        }

        for(auto move : King->GetPossibleMovePositions())
        {
            //check that new location will result in another check
            if(!EnemyMoves.Contains(move))
            {
                ValidMoves.Emplace(King, move);
            }
        }
    }
    else
    {
        //To determine what pieces are pinned (they can only move along determined axis otherwise they will cause a check(therefore they are pinned))
        for(auto EnemyChessPiece : EnemyChessPieces)
        {
           
            if(EnemyChessPiece->IsSlidingPiece())
            {
                ABaseChessPiece* FPinnedPiece = nullptr;
                int EnemyChessPieceX = EnemyChessPiece->GetCurrentPosition().X;
                int EnemyChessPieceY = EnemyChessPiece->GetCurrentPosition().Y;
                int KingChessPieceX = King->GetCurrentPosition().X;
                int KingChessPieceY = King->GetCurrentPosition().Y;
                if((EnemyChessPiece->GetType() == Type::Rook || EnemyChessPiece->GetType() == Type::Queen) && (EnemyChessPieceX == KingChessPieceX || EnemyChessPieceY == KingChessPieceY))
                {
                    if(EnemyChessPieceX == KingChessPieceX)
                    {
                        int MaxY = FGenericPlatformMath::Max(EnemyChessPieceY, KingChessPieceY);
                        int MinY = FGenericPlatformMath::Min(EnemyChessPieceY, KingChessPieceY);
                        for(int i{MinY + 1}; i < MaxY; i++)
                        {
                           
                            if(ABaseChessPiece* PinnedChessPiece = ChessBoard->GetChessPiece(FIntPoint(EnemyChessPieceX, i)))
                            {
                                if(PinnedChessPiece->IsWhite() == EnemyChessPiece->IsWhite())
                                {
                                    FPinnedPiece = nullptr;
                                    break;
                                }
                                else
                                {
                                    if(FPinnedPiece)
                                    {
                                        FPinnedPiece = nullptr;
                                        break;
                                    }
                                    FPinnedPiece = PinnedChessPiece;
                                }
                            }
                        }
                        if(FPinnedPiece)
                        {

                            TArray<FIntPoint> PossibleMoves = FPinnedPiece->GetPossibleMovePositions();
                            for(int i{MinY}; i <= MaxY; i++)
                            {
                                if(PossibleMoves.Contains(FIntPoint(EnemyChessPieceX, i)))
                                {
                                    ValidMoves.Emplace(FPinnedPiece, FIntPoint(EnemyChessPieceX, i));
                                }
                            }
                            PinnedChessPieces.Emplace(FPinnedPiece);
                        }
                        
                    }
                    else if(EnemyChessPieceY == KingChessPieceY)
                    {
                        int MaxX = FGenericPlatformMath::Max(EnemyChessPieceX, KingChessPieceX);
                        int MinX = FGenericPlatformMath::Min(EnemyChessPieceX, KingChessPieceX);
                        APawnChessPiece* PinnedPawn = nullptr;
                        for(int i{MinX + 1}; i < MaxX; i++)
                        {
                            if(ABaseChessPiece* PinnedChessPiece = ChessBoard->GetChessPiece(FIntPoint(i, EnemyChessPieceY)))
                            {
                                if(PinnedChessPiece->IsWhite() == EnemyChessPiece->IsWhite())
                                {
                                    if(PinnedChessPiece->GetType() == Type::Pawn)
                                    {
                                        if(PinnedPawn && PinnedPawn->GetEnPassantPawn() == PinnedChessPiece)
                                        {
                                            continue;
                                        }
                                        else if(APawnChessPiece* TPinnedPawn = Cast<APawnChessPiece>(ChessBoard->GetChessPiece(FIntPoint(i + 1, EnemyChessPieceY))))
                                        {
                                            if(TPinnedPawn->GetEnPassantPawn() == PinnedChessPiece)
                                            {
                                                continue;
                                            }
                                        }
                                    }
                                    FPinnedPiece = nullptr;
                                    PinnedPawn = nullptr;
                                    break;
                                }
                                else
                                {
                                    if(PinnedChessPiece->GetType() == Type::Pawn && !PinnedPawn && !FPinnedPiece)
                                    {
                                        APawnChessPiece* PawnWithEnPassant = Cast<APawnChessPiece>(PinnedChessPiece);
                                        if(PawnWithEnPassant && PawnWithEnPassant->GetEnPassantPawn())
                                        {
                                            PinnedPawn = PawnWithEnPassant;
                                            continue;
                                        }
                                    }
                                    if(FPinnedPiece || PinnedPawn)
                                    {
                                        FPinnedPiece = nullptr;
                                        PinnedPawn = nullptr;
                                        break;
                                    }
                                    else
                                    {
                                        FPinnedPiece = PinnedChessPiece;
                                    }                                         
                                }
                            }
                        }
                        if(FPinnedPiece)
                        {
                            TArray<FIntPoint> PossibleMoves = FPinnedPiece->GetPossibleMovePositions();
                            for(int i{MinX}; i <= MaxX; i++)
                            {
                                if(PossibleMoves.Contains(FIntPoint(i, EnemyChessPieceY)))
                                {
                                    ValidMoves.Emplace(FPinnedPiece, FIntPoint(i, EnemyChessPieceY));
                                }
                            }
                            PinnedChessPieces.Emplace(FPinnedPiece);
                        }
                        if(PinnedPawn)
                        {
                           PinnedPawn->ResetEnPassant();
                           UE_LOG(LogTemp, Warning, TEXT("Pinned Pawn found!")); 
                        }
                    }
                }
                else if(EnemyChessPiece->GetType() == Type::Bishop || EnemyChessPiece->GetType() == Type::Queen)
                {
                    if(UKismetMathLibrary::Abs_Int(EnemyChessPieceX - KingChessPieceX) == UKismetMathLibrary::Abs_Int(EnemyChessPieceY - KingChessPieceY))
                    {
                        int YDirection = (EnemyChessPieceY < KingChessPieceY) ? 1 : -1;
                        int XDirection = (EnemyChessPieceX < KingChessPieceX) ? 1 : -1;
                        for(int i = 1; i < UKismetMathLibrary::Abs_Int(EnemyChessPieceY - KingChessPieceY); i++)
                        {
                            if(ABaseChessPiece* PinnedChessPiece = ChessBoard->GetChessPiece(FIntPoint(EnemyChessPieceX + (XDirection * i), EnemyChessPieceY + (YDirection * i))))
                            {
                                if(PinnedChessPiece->IsWhite() == EnemyChessPiece->IsWhite())
                                {
                                    FPinnedPiece = nullptr;
                                    break;
                                }
                                else
                                {
                                    if(FPinnedPiece)
                                    {
                                        FPinnedPiece = nullptr;
                                        break;
                                    }
                                    FPinnedPiece = PinnedChessPiece;
                                }
                            }
                        }
                        if(FPinnedPiece)
                        {
                            TArray<FIntPoint> PossibleMoves = FPinnedPiece->GetPossibleMovePositions();
                            for(int i = 0; i < UKismetMathLibrary::Abs_Int(EnemyChessPieceY - KingChessPieceY); i++)
                            {
                                if(PossibleMoves.Contains(FIntPoint(EnemyChessPieceX + (XDirection * i), EnemyChessPieceY + (YDirection * i))))
                                {
                                    ValidMoves.Emplace(FPinnedPiece, FIntPoint(EnemyChessPieceX + (XDirection * i), EnemyChessPieceY + (YDirection * i)));
                                }
                            }
                            PinnedChessPieces.Emplace(FPinnedPiece);
                        }
                    }
                }
            }
        }
        for(auto ChessPiece : ChessPieces)
        {
            if(!PinnedChessPieces.Contains(ChessPiece) ) //&& ChessPiece->GetType() != Type::King
            {
                for(auto move : ChessPiece->GetPossibleMovePositions())
                {
                    if(ChessPiece->GetType() != Type::King)
                    {
                        ValidMoves.Emplace(ChessPiece, move);
                    }
                    else
                    {
                        //check that new location will not result in check
                        if(!EnemyMoves.Contains(move))
                        {
                            ValidMoves.Emplace(ChessPiece, move);
                        }
                    }
                }
            }
        }
    }
    return ValidMoves;
}
    

TArray<FIntPoint> AChessPlayerController::GetPossibleMovesOfEnemyPieces(bool IsWhite) const
{
    TArray<FIntPoint> EnemyMoves;
    
    for(auto EnemyPiece : ChessBoard->GetChessPieces(!(IsWhite)))
    {
        EnemyMoves.Append(EnemyPiece->GetPossibleMovePositionsForEnemy());
    }
    return EnemyMoves;
}

bool AChessPlayerController::IsKingInCheck(bool IsWhite, TArray<FIntPoint> EnemyMoves) const
{
    // TArray<ABaseChessPiece*> EnemyChessPieces = ChessBoard->GetChessPieces(!IsWhite);
    // for(auto enemy : EnemyChessPieces)
    // {
    for(auto move : EnemyMoves)
    {
        if(move == (IsWhite ? WhiteKing->GetCurrentPosition() : BlackKing->GetCurrentPosition()))
        {
            return true;
        }
    }
    // }
    
    return false;
}

int AChessPlayerController::MoveGenerationTest(int depth, bool IsWhite)
{
    if(depth == 1)
    {
        return GetAllValidMoves(IsWhite).Num();
        //return 1;
    }
    int numPositions{};
    for(ChessMove& move : GetAllValidMoves(IsWhite))
    {
        MakeMove(move.ChessPiece, move.NewPosition, true);
        numPositions += MoveGenerationTest(depth - 1, !IsWhite);
        UndoMove(true);
    }

    return numPositions;
}

void AChessPlayerController::ResetAllEnPassant(bool IsWhite)
{
    TArray<ABaseChessPiece*> ChessPieces = ChessBoard->GetChessPieces(IsWhite);
    for(auto ChessPiece : ChessPieces)
    {
        if(ChessPiece->GetType() == Type::Pawn)
        {
            APawnChessPiece* PawnPiece = Cast<APawnChessPiece>(ChessPiece);
            if(PawnPiece->GetEnPassantPawn())
            {
                PawnPiece->ResetEnPassant();
            }
        }
    }
}

void AChessPlayerController::UndoActualMove()
{
    if(MoveIndex > 0)
    {
        DeleteValidMoveSquares();
        UndoMove(false);
        if(CoOp)
        {
            SwitchSides();
        }
        else
        {
            if(MoveIndex > 0)
            {
                UndoMove(false);
            }
        }
    }
    
}

void AChessPlayerController::SetCamera()
{
    TArray<AActor*> Cameras;
    UGameplayStatics::GetAllActorsOfClass(this, ACameraActor::StaticClass(), Cameras);
    if(Cameras.Num() > 0 && Cameras[0])
    {
        SetViewTarget(Cameras[0]);
    }
}

TArray<APawnChessPiece*> AChessPlayerController::GetPawnsWithEnPassant(bool IsWhite)
{
    TArray<ABaseChessPiece*> ChessPieces = ChessBoard->GetChessPieces(IsWhite);
    TArray<APawnChessPiece*> PawnsWithEnpassant;

    for(auto ChessPiece : ChessPieces)
    {
        if(ChessPiece->GetType() == Type::Pawn)
        {
            APawnChessPiece* PawnPiece = Cast<APawnChessPiece>(ChessPiece);
            if(PawnPiece->GetEnPassantPawn())
            {
                PawnsWithEnpassant.Emplace(PawnPiece);
            }
        }
    }
    return PawnsWithEnpassant;
}

void AChessPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(SelectPieceAction, ETriggerEvent::Started, this, &AChessPlayerController::SelectPiece);
        EnhancedInputComponent->BindAction(MoveSelectedPieceAction, ETriggerEvent::Started, this, &AChessPlayerController::MoveSelectedPiece);
        EnhancedInputComponent->BindAction(UndoMoveAction, ETriggerEvent::Started, this, &AChessPlayerController::UndoActualMove);
        EnhancedInputComponent->BindAction(PauseGame, ETriggerEvent::Started, this, &AChessPlayerController::PauseChessGame);
    }
}