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
#include "RookChessPiece.h"
#include "ChessGameMode.h"
#include "QueenChessPiece.h"
#include "Kismet/KismetMathLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"

AChessPlayerController::AChessPlayerController()
{
    
}

void AChessPlayerController::BeginPlay()
{
    Super::BeginPlay();

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
            if(king->ActorHasTag("White"))
            {
                WhiteKing = Cast<AKingChessPiece>(king);
            }
            else if(king->ActorHasTag("Black"))
            {
                BlackKing = Cast<AKingChessPiece>(king);
            }
        }
    }

    //Initialize Current and Enemy Kings and Enemy Side
    if(CurrentSide == "White")
	{
        CurrentKing = WhiteKing;
        EnemyKing = BlackKing;
		EnemySide = "Black";
	}
	else if(CurrentSide == "Black")
	{
        CurrentKing = BlackKing;
        EnemyKing = WhiteKing;
		EnemySide = "White";
	}

    //AIvsAI
    if(AIvsAI)
    {
        GetWorldTimerManager().SetTimer(AIvsAITimerHandle, this, &AChessPlayerController::GenerateMove, AIRate, true);
    }
}

void AChessPlayerController::SelectPiece()
{
    DeleteValidMoveSquares();
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    //Check if piece selected is valid
    if(HitResult.GetActor()->ActorHasTag(CurrentSide))
    {
        SelectedPiece = Cast<ABaseChessPiece>(HitResult.GetActor());
        //UE_LOG(LogTemp, Display, TEXT("Actor Hit: %s"), *SelectedPiece->GetActorNameOrLabel());
        //Display Valid Moves
        DisplayValidMoves(SelectedPiece);
    }    
}

void AChessPlayerController::DisplayValidMoves(ABaseChessPiece* ChessPiece)
{
    TArray<FIntPoint> ValidMoves = GetValidMoves(ChessPiece);
    if(ValidMoves.Num() > 0)
    {
        for(const auto& location : ValidMoves)
        {
            if(ChessBoard)
            {
                FVector AdjustedLocation = ChessBoard->GetLocation(location);
                AdjustedLocation.Z = 0.f;
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
                }
                
            }
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

void AChessPlayerController::MoveSelectedPiece()
{
    if(SelectedPiece)
    {
        //Check if Player chose valid move location
        FHitResult HitResult;
        GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
        AActor* ChosenSquare = HitResult.GetActor();
        AActor* EnemyPiece = Cast<ABaseChessPiece>(ChosenSquare);
        if(EnemyPiece && EnemyPiece->ActorHasTag(EnemySide))
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
            if(ChessBoard)
            {
                FIntPoint index = ChessBoard->GetIndex(ChosenSquare->GetActorLocation());
                //UE_LOG(LogTemp, Display, TEXT("Index: %s"), *index.ToString());

                CheckSpecialMoves(index, SelectedPiece);

                if(index != FIntPoint(-1, -1))
                {
                    UpdateSelectedPieceLocation(index, SelectedPiece);
                }

                //Pawn Promotion Special Move
                //Check if Pawn has reached end of chessboard
                if(ShouldPromotePawn())
                {
                    //Spawn Widget
                    //User chooses which chess piece pawn will be promoted to
                    PromotePawn();
                    return;
                }   
            }
            // FTimerHandle DelayTimerHandle;
            // GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &AChessPlayerController::BeginNextTurn, 0.5f, false);
            BeginNextTurn();
        }
        else
        {
            return;
        }
    }
    
}

TArray<FIntPoint> AChessPlayerController::GetValidMoves(ABaseChessPiece* ChessPiece)
{
    TArray<FIntPoint> ValidMoves;
    AKingChessPiece* King = ChessPiece->GetSide() == "White"? WhiteKing : BlackKing;

    for(FIntPoint move : ChessPiece->GetPossibleMovePositions())
    {
        SimulateMove(ChessPiece, move);

        //if king is not in check
        if(!King->IsChecked())
        {
            ValidMoves.Emplace(move);
        }
        
        UndoMove();
    }
    //UE_LOG(LogTemp, Display, TEXT("Possible Moves: %d"), ValidMoves.Num());
    return ValidMoves;
}

void AChessPlayerController::CheckSpecialMoves(FIntPoint index, ABaseChessPiece* ChessPiece)
{
    //Castling Special Move
    //Check if ChessPiece is King and if it has CastledRook
    if(AKingChessPiece* KingPiece = Cast<AKingChessPiece>(ChessPiece))
    {
        if(KingPiece->IsCastling())
        {
            //Move CastledRook to new location depending selected location
            if(index.X == 2)
            {   
                if(ABaseChessPiece* RookPiece = Cast<ABaseChessPiece>(ChessBoard->GetChessPiece(FIntPoint(0, index.Y))))
                {
                    UpdateSelectedPieceLocation(FIntPoint(RookPiece->GetCurrentPosition().X + 3, RookPiece->GetCurrentPosition().Y), RookPiece);
                }
                
            }
            else if(index.X == 6)
            {
                if(ABaseChessPiece* RookPiece = Cast<ABaseChessPiece>(ChessBoard->GetChessPiece(FIntPoint(7, index.Y))))
                {
                    UpdateSelectedPieceLocation(FIntPoint(RookPiece->GetCurrentPosition().X - 2, RookPiece->GetCurrentPosition().Y), RookPiece);
                }
            } 
        }
    }

    //En Passant Special Move
    //if Pawn is performing En Passant
    if(APawnChessPiece* PawnPiece = Cast<APawnChessPiece>(ChessPiece))
    {        
        if(APawnChessPiece* EnPassantPawn = PawnPiece->GetEnPassantPawn())
        {
            UE_LOG(LogTemp, Display, TEXT("Has EnPassant"));
            //check if ChosenSquare is above EnPassantPawn
            if(index.X == EnPassantPawn->GetCurrentPosition().X)
            {
                EnPassantPawn->Destroy();
            }
            
        }
    }
}

void AChessPlayerController::SwitchSides()
{
    if(CurrentSide == "White")
	{
		CurrentSide = "Black";
        CurrentKing = BlackKing;
        EnemySide = "White";
        EnemyKing = WhiteKing;
	}
	else if(CurrentSide == "Black")
	{
		CurrentSide = "White";
        CurrentKing = WhiteKing;
        EnemySide = "Black";
        EnemyKing = BlackKing;
	}
}

void AChessPlayerController::BeginNextTurn()
{
    DeleteValidMoveSquares();  
    //Since no more piece selected, reset SelectedPiece to nullptr 
    SelectedPiece = nullptr;
    //Check if enemy side lost(whether it is AI or human enemy)
    if(HasEnemyLost())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s LOST!"), *EnemySide.ToString());
        return;
    }
    //Switch Sides if playing co-op
    SwitchSides();
    
    
    
    if(!CoOp)
    {
        //AI move
        GenerateMove();
        //Check if enemy side lost(whether it is AI or human enemy)
        if(HasEnemyLost())
        {
            UE_LOG(LogTemp, Warning, TEXT("%s LOST!"), *EnemySide.ToString());
        }
        SwitchSides();
        SynchronizeChessPieces();
    }
    //ChessBoard->PrintChessPieces();
}

void AChessPlayerController::UpdateSelectedPieceLocation(FIntPoint NewIndex, ABaseChessPiece* ChessPiece)
{
    //If selected square has enemy chess piece, destroy enemy chess piece
    if(ChessBoard->GetChessPiece(NewIndex))
    {
        ChessBoard->GetChessPiece(NewIndex)->Tags[0] = "";
        ChessGameMode->PieceCaptured(ChessBoard->GetChessPiece(NewIndex));
    }
    //Set ChessBoard to chess piece of vacated position to nullptr
    ChessBoard->SetChessPiece(ChessPiece->GetCurrentPosition(), nullptr);
    //Move SelectedPiece to chosen location
    ChessPiece->SetActorLocation(ChessBoard->GetLocation(NewIndex));
    //Update ChessBoard to set chess piece of new location
    ChessBoard->SetChessPiece(NewIndex, ChessPiece);
    //Update CurrentPosition of SelectedPiece
    ChessPiece->MoveChessPiece(NewIndex);
    UE_LOG(LogTemp, Display, TEXT("ChessBoard Eval: %d"), ChessBoard->Evaluate());
    
}

bool AChessPlayerController::ShouldPromotePawn()
{
    if(APawnChessPiece* PawnPiece = Cast<APawnChessPiece>(SelectedPiece))
    {
        if(PawnPiece->GetCurrentPosition().Y == 0 || PawnPiece->GetCurrentPosition().Y == 7)
        {
            return true;
        }
    }
    return false;
}

bool AChessPlayerController::HasEnemyLost()
{
    if(EnemyKing->IsChecked())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s King is in CHECK!"), *EnemySide.ToString());
        if(GetAllValidMoves(false).Num() == 0)
        {
            return true;
        }
    }
    return false;
}

void AChessPlayerController::SimulateMove(ABaseChessPiece* ChessPiece, FIntPoint NewIndex)
{
    FName EnemyTag = ChessPiece->GetSide() == "White"? "Black" : "White";
    SimulatedMovedPieces.EmplaceAt(SimulateIndex, ChessPiece);
    NewLocations.EmplaceAt(SimulateIndex, NewIndex);
    OriginalLocations.EmplaceAt(SimulateIndex, ChessPiece->GetCurrentPosition());

    //if move location has an enemy piece
    ABaseChessPiece* EnemyPiece = Cast<ABaseChessPiece>(ChessBoard->GetChessPiece(NewIndex));
    if(EnemyPiece && EnemyPiece->ActorHasTag(EnemyTag))
    {
        SimulatedCapturedPieces.EmplaceAt(SimulateIndex, EnemyPiece);
        OriginalCapturedPieceSide.EmplaceAt(SimulateIndex, EnemyTag);
        //Set EnemyPiece as not a threat
        EnemyPiece->Tags[0] = "";
    }
    else
    {
        SimulatedCapturedPieces.EmplaceAt(SimulateIndex, nullptr);
        OriginalCapturedPieceSide.EmplaceAt(SimulateIndex, "");
    }
    //Update location of ChessPiece to NewIndex
    ChessBoard->SetChessPiece(NewIndex, ChessPiece);
    //Set original index to nullptr
    ChessBoard->SetChessPiece(ChessPiece->GetCurrentPosition(), nullptr);
    //Set current position of ChessPiece to NewIndex
    ChessPiece->SetCurrentPosition(NewIndex);
    SimulateIndex++;
}

void AChessPlayerController::UndoMove()
{
    SimulateIndex--;
    //if EnemyPiece exists reset location to move index
    if(SimulatedCapturedPieces[SimulateIndex])
    {
        SimulatedCapturedPieces[SimulateIndex]->Tags[0] = OriginalCapturedPieceSide[SimulateIndex];
        ChessBoard->SetChessPiece(NewLocations[SimulateIndex], SimulatedCapturedPieces[SimulateIndex]);
    }
    //else reset to nullptr
    else
    {
        ChessBoard->SetChessPiece(NewLocations[SimulateIndex], nullptr);
    }
    //reset ChessPiece location to original index
    ChessBoard->SetChessPiece(OriginalLocations[SimulateIndex], SimulatedMovedPieces[SimulateIndex]);
    SimulatedMovedPieces[SimulateIndex]->SetCurrentPosition(OriginalLocations[SimulateIndex]);
    
    SimulatedMovedPieces.RemoveAt(SimulateIndex);
    SimulatedCapturedPieces.RemoveAt(SimulateIndex);
    OriginalCapturedPieceSide.RemoveAt(SimulateIndex);
    NewLocations.RemoveAt(SimulateIndex);
    OriginalLocations.RemoveAt(SimulateIndex);

    
}

void AChessPlayerController::SpawnPromotedPawn()
{
    if(SelectedPiece)
    {
        FIntPoint SpawnIndex = SelectedPiece->GetCurrentPosition();
        //Destroy Pawn
        SelectedPiece->Destroy();
        //Spawn chosen chess piece and set selectedpiece to chosen piece
        SelectedPiece = Cast<ABaseChessPiece>(GetWorld()->SpawnActor<AActor>(PawnPromotion, ChessBoard->GetLocation(SpawnIndex), FRotator::ZeroRotator));
        if(!AITurn)
        {
            BeginNextTurn();
        }
        
    }
    
}

void AChessPlayerController::RandomAIMove()
{
    AITurn = true;
    TArray<AActor*> EnemyPieces;
    TArray<AActor*> EnemyPiecesWithValidMoves;
    UGameplayStatics::GetAllActorsWithTag(this, CurrentSide, EnemyPieces);
    for(AActor* enemy : EnemyPieces)
    {
        if(ABaseChessPiece* ChessPiece = Cast<ABaseChessPiece>(enemy))
        {
            if(GetValidMoves(ChessPiece).Num() > 0)
            {
                EnemyPiecesWithValidMoves.Add(enemy);
            }
        }
    }
    if(EnemyPiecesWithValidMoves.Num() > 0)
    {
        int target = UKismetMathLibrary::RandomInteger64(EnemyPiecesWithValidMoves.Num());
        if(ABaseChessPiece* ChessPiece = Cast<ABaseChessPiece>(EnemyPiecesWithValidMoves[target]))
        {
            target = UKismetMathLibrary::RandomInteger64(GetValidMoves(ChessPiece).Num());
            FIntPoint NewIndex = GetValidMoves(ChessPiece)[target];
            CheckSpecialMoves(NewIndex, ChessPiece);
            UpdateSelectedPieceLocation(NewIndex, ChessPiece);

            if(ShouldPromotePawn())
            {
                SetAIPawnPromotion();
                SpawnPromotedPawn();
            }   
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s LOST!"), *CurrentSide.ToString());
    }
    AITurn = false;

    if(AIvsAI)
    {
        SwitchSides();
    }
}

int AChessPlayerController::Minimax(int depth, bool MaximizingPlayer, int alpha, int beta, bool IsFirst)
{
    ChessMove TempMove;

    if(depth == 0) // or game over when in position
    {
        SynchronizeChessPieces();
        return ChessBoard->Evaluate();
    }
    else if(GetAllValidMoves(true).Num() == 0)
    {
        return -10000;
    }
    else if(GetAllValidMoves(false).Num() == 0)
    {
        return 10000;
    }

    if(MaximizingPlayer)
    {
        int MaxEval = -10000;
        for(ChessMove& move : GetAllValidMoves(true))
        {
            SimulateMove(move.ChessPiece, move.NewPosition);

            int eval = Minimax(depth - 1, false, alpha, beta, false);
            MaxEval = FGenericPlatformMath::Max(MaxEval, eval);
            alpha = FGenericPlatformMath::Max(alpha, eval);
            
            if(eval == MaxEval)
            {
                TempMove.ChessPiece = move.ChessPiece;
                TempMove.NewPosition = move.NewPosition;
            }

            UndoMove();
        
            if(beta <= alpha)
            {
                break;
            }

        }
        if(IsFirst)
        {
            GeneratedMove = TempMove;
        }
        return MaxEval;
    }
    else
    {
        int MinEval = 10000;
        for(ChessMove& move : GetAllValidMoves(false))
        {
            SimulateMove(move.ChessPiece, move.NewPosition);

            int eval = Minimax(depth - 1, true, alpha, beta, false);
            MinEval = FGenericPlatformMath::Min(MinEval, eval);
            beta = FGenericPlatformMath::Min(beta, eval);

            if(eval == MinEval)
            {
                TempMove.ChessPiece = move.ChessPiece;
                TempMove.NewPosition = move.NewPosition;
            }

            UndoMove();

            if(beta <= alpha)
            {
                break;
            }
        }
        if(IsFirst)
        {
            GeneratedMove = TempMove;
        }
        return MinEval;
    }
}

void AChessPlayerController::GenerateMove()
{   
    const int infinity = 10000;
    if(CurrentSide == "White")
    {
        Minimax(MinimaxDepth, true, -infinity, infinity, true);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Black Possible Moves"));
        for(auto move : GetAllValidMoves(false))
        {
            UE_LOG(LogTemp, Warning, TEXT("%s : %s"), *move.ChessPiece->GetActorNameOrLabel(), *move.NewPosition.ToString());
        }
        Minimax(MinimaxDepth, false, -infinity, infinity, true);
    }
    if(GeneratedMove.ChessPiece)
    {
        UE_LOG(LogTemp, Warning, TEXT("Chosen Actor %s Chosen Index %s"), *GeneratedMove.ChessPiece->GetActorNameOrLabel(), *GeneratedMove.NewPosition.ToString());
        CheckSpecialMoves(GeneratedMove.NewPosition, GeneratedMove.ChessPiece);
        UpdateSelectedPieceLocation(GeneratedMove.NewPosition, GeneratedMove.ChessPiece);
    }

    GeneratedMove.reset();

    if(AIvsAI)
    {
        SwitchSides();
    }
    
}

void AChessPlayerController::SynchronizeChessPieces() const
{
    TArray<AActor*> chesspieces;
    UGameplayStatics::GetAllActorsOfClass(this, ABaseChessPiece::StaticClass(), chesspieces);
    for(AActor* piece : chesspieces)
    {
        if(ABaseChessPiece* chesspiece = Cast<ABaseChessPiece>(piece))
        {
            chesspiece->SynchronizePosition();
        }
    }
}

TArray<ChessMove> AChessPlayerController::GetAllValidMoves(bool IsWhite)
{
    TArray<ChessMove> ValidMoves;
    TArray<FIntPoint> PlayerMoves;
    TArray<AActor*> ChessPieces;
    IsWhite ? UGameplayStatics::GetAllActorsWithTag(this, "White", ChessPieces) : UGameplayStatics::GetAllActorsWithTag(this, "Black", ChessPieces);
    
    for(AActor* PlayerPiece : ChessPieces)
    {
        if(ABaseChessPiece* ChessPiece = Cast<ABaseChessPiece>(PlayerPiece))
        {
            for(FIntPoint ValidMove : GetValidMoves(ChessPiece))
            {
                ValidMoves.Emplace(ChessMove(ChessPiece, ValidMove));
            }
        }
    }
    return ValidMoves;
}

void AChessPlayerController::SetPawnPromotion(TSubclassOf<ABaseChessPiece> ChosenPiece)
{
    PawnPromotion = ChosenPiece;
}

void AChessPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(SelectPieceAction, ETriggerEvent::Started, this, &AChessPlayerController::SelectPiece);
        EnhancedInputComponent->BindAction(MoveSelectedPieceAction, ETriggerEvent::Started, this, &AChessPlayerController::MoveSelectedPiece);
    }
}
