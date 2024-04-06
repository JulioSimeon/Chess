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

AChessPlayerController::AChessPlayerController()
{
    
}

void AChessPlayerController::SetPawnPromotion(TSubclassOf<ABaseChessPiece> ChosenPiece)
{
    PawnPromotion = ChosenPiece;
}

void AChessPlayerController::SelectPiece()
{
    DeleteValidMoveSquares();
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
    //Check if piece selected is valid
    if(HitResult.GetActor()->ActorHasTag(PlayerSide))
    {
        SelectedPiece = Cast<ABaseChessPiece>(HitResult.GetActor());
        UE_LOG(LogTemp, Display, TEXT("Actor Hit: %s"), *SelectedPiece->GetActorNameOrLabel());
        //Display Valid Moves
            //Call SelectedPiece.GetPossibleMovePositions() return array of possible moves, if array empty, no moves, Array of FIntPoint
        DisplayValidMoves();
    }    
}

void AChessPlayerController::MoveSelectedPiece()
{
    if(SelectedPiece)
    {
        //Check if Player chose valid move location
        UE_LOG(LogTemp, Display, TEXT("Moving"));
        FHitResult HitResult;
        GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
        AActor* ChosenSquare = HitResult.GetActor();
        AActor* EnemyPiece = Cast<ABaseChessPiece>(ChosenSquare);
        if(EnemyPiece && EnemyPiece->ActorHasTag(EnemySide))
        {
            UE_LOG(LogTemp, Display, TEXT("EnemyPiece was selected"));
            //Check if EnemyPiece location is same as any of the validsquares
            FVector AdjustedEnemyLocation = EnemyPiece->GetActorLocation();
            AdjustedEnemyLocation.Z = 0.f;
            for(AActor* Square : ValidSquares)
            {
                if(AdjustedEnemyLocation == Square->GetActorLocation())
                {
                    //If true then enemy is valid capture and enemy is eaten
                    UE_LOG(LogTemp, Display, TEXT("Matching square found"));
                    ChosenSquare = Square;
                    EnemyPiece->Destroy();
                }
            }    
        }
        if(ValidSquares.Find(ChosenSquare) != INDEX_NONE)
        {
            if(ChessBoard)
            {
                FIntPoint index = ChessBoard->GetIndex(ChosenSquare->GetActorLocation());
                UE_LOG(LogTemp, Display, TEXT("Index: %s"), *index.ToString());

                //Castling Special Move
                //Check if SelectedPiece is King and if it has CastledRook
                if(AKingChessPiece* KingPiece = Cast<AKingChessPiece>(SelectedPiece))
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

                if(APawnChessPiece* PawnPiece = Cast<APawnChessPiece>(SelectedPiece))
                {
                    //En Passant Special Move
                    //if Pawn is performing En Passant
                    if(APawnChessPiece* EnPassantPawn = PawnPiece->GetEnPassantPawn())
                    {
                        //check if ChosenSquare is above EnPassantPawn
                        if(index.X == EnPassantPawn->GetCurrentPosition().X)
                        {
                            EnPassantPawn->Destroy();
                        }
                        
                    }
                }

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
            BeginNextTurn();
        }
        else
        {
            return;
        }
    }
    
}

void AChessPlayerController::DisplayValidMoves()
{
    TArray<FIntPoint> ValidMoves;
    FIntPoint OriginalIndex = SelectedPiece->GetCurrentPosition();
    if(PlayerKing->IsChecked())
    {
        TArray<FIntPoint> PossibleMoves = SelectedPiece->GetPossibleMovePositions();
        if(PossibleMoves.Num() > 0)
        {
            for(FIntPoint move : PossibleMoves)
            {
                //simulate moving selectedpiece to each possible move location
                
                //if move location has an enemy piece
                if(AActor* TempActor = ChessBoard->GetChessPiece(move))
                {
                    if(TempActor && TempActor->ActorHasTag(EnemySide))
                    {  
                        SelectedPiece->SetCurrentPosition(move);
                        //Set TempActor as not a threat
                        TempActor->Tags[0] = PlayerSide;
                    }
                    //if king is no longer checked
                    if(!PlayerKing->IsChecked())
                    {
                        ValidMoves.Emplace(move);
                    }
                    ChessBoard->GetChessPiece(move)->Tags[0] = EnemySide;
                    SelectedPiece->SetCurrentPosition(OriginalIndex);
                }
                else
                {
                    ChessBoard->SetChessPiece(move, SelectedPiece);
                    ChessBoard->SetChessPiece(OriginalIndex, nullptr);
                    SelectedPiece->SetCurrentPosition(move);
                    //if king is no longer checked
                    if(!PlayerKing->IsChecked())
                    {
                        ValidMoves.Emplace(move);
                    }
                    //reset selectedpiece location to original index
                    ChessBoard->SetChessPiece(OriginalIndex, SelectedPiece);
                    ChessBoard->SetChessPiece(move, nullptr);
                    SelectedPiece->SetCurrentPosition(OriginalIndex);
                }
            }
        }
        
    }
    else
    {
        for(FIntPoint move : SelectedPiece->GetPossibleMovePositions())
        {
            if(AActor* TempActor = ChessBoard->GetChessPiece(move))
            {
                if(TempActor && TempActor->ActorHasTag(EnemySide))
                {  
                    SelectedPiece->SetCurrentPosition(move);
                    //Set TempActor as not a threat
                    TempActor->Tags[0] = PlayerSide;

                }
                //if king will not be in check
                if(!PlayerKing->IsChecked())
                {
                    ValidMoves.Emplace(move);
                }
                ChessBoard->GetChessPiece(move)->Tags[0] = EnemySide;
                SelectedPiece->SetCurrentPosition(OriginalIndex);
            }
            else
            {
                ChessBoard->SetChessPiece(move, SelectedPiece);
                ChessBoard->SetChessPiece(OriginalIndex, nullptr);
                SelectedPiece->SetCurrentPosition(move);
                //if king will not be in check
                if(!PlayerKing->IsChecked())
                {
                    ValidMoves.Emplace(move);
                }
                //reset selectedpiece location to original index
                ChessBoard->SetChessPiece(OriginalIndex, SelectedPiece);
                ChessBoard->SetChessPiece(move, nullptr);
                SelectedPiece->SetCurrentPosition(OriginalIndex);
            }
            
        }
        
        
    }
    UE_LOG(LogTemp, Display, TEXT("Possible Moves: %d"), ValidMoves.Num());
    if(ValidMoves.Num() > 0)
    {
        for(const auto& location : ValidMoves)
        {
            if(ChessBoard)
            {
                UE_LOG(LogTemp, Display, TEXT("ChessBoard is valid in Controller"));
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

void AChessPlayerController::SwitchSides()
{
    if(PlayerSide == "White")
	{
		PlayerSide = "Black";
        EnemySide = "White";
        PlayerKing = BlackKing;
	}
	else if(PlayerSide == "Black")
	{
		PlayerSide = "White";
        EnemySide = "Black";
        PlayerKing = WhiteKing;
	}
}

void AChessPlayerController::BeginNextTurn()
{
    DeleteValidMoveSquares();  
    //Since no more piece selected, reset SelectedPiece to nullptr 
    SelectedPiece = nullptr;
    //Switch Sides if playing co-op
    if(CoOp)
    {
        SwitchSides();
    }
    else
    {
        //AI move
    }
}

void AChessPlayerController::UpdateSelectedPieceLocation(FIntPoint NewIndex, ABaseChessPiece* ChessPiece)
{
    //If selected square has enemy chess piece, destroy enemy chess piece
    if(ChessBoard->GetChessPiece(NewIndex))
    {
        ChessGameMode->PieceCaptured(ChessBoard->GetChessPiece(NewIndex));
    }
    //Set ChessBoard to chess piece of vacated position to nullptr
    ChessBoard->SetChessPiece(ChessPiece->GetCurrentPosition(), nullptr);
    //Move SelectedPiece to chosen location
    ChessPiece->SetActorLocation(ChessBoard->GetLocation(NewIndex));
    //Update ChessBoard to set chess piece of new location
    ChessBoard->SetChessPiece(NewIndex, ChessPiece);
    //Update CurrentPosition of SelectedPiece
    ChessPiece->SetCurrentPosition(NewIndex);
    UE_LOG(LogTemp, Display, TEXT("Index Valid and Chessboard and SelectedPiece are valid"));
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

void AChessPlayerController::SpawnPromotedPawn()
{
    if(SelectedPiece)
    {
        FIntPoint SpawnIndex = SelectedPiece->GetCurrentPosition();
        //Destroy Pawn
        SelectedPiece->Destroy();
        //Spawn chosen chess piece and set selectedpiece to chosen piece
        SelectedPiece = Cast<ABaseChessPiece>(GetWorld()->SpawnActor<AActor>(PawnPromotion, ChessBoard->GetLocation(SpawnIndex), FRotator::ZeroRotator));
        BeginNextTurn();
    }
    
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

    //Initialize EnemySide
    if(PlayerSide == "White")
	{
		EnemySide = "Black";
	}
	else if(PlayerSide == "Black")
	{
		EnemySide = "White";
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
    PlayerKing = WhiteKing;
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
