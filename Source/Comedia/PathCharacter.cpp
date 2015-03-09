// All rights reserved

#include "Comedia.h"
#include "PathCharacter.h"
#include "LiyaCharacter.h"

PathCharacter::PathCharacter()
{
	LocalPosition = 0.0f;
	IndexCurrentTargetNode = 0;
	PathNodes = TArray<PathNode*>();
}

PathCharacter::~PathCharacter()
{
}

void PathCharacter::UpdateCharacter(float DeltaSeconds)
{
	if (PathNodes.Num() > 0 && IndexCurrentTargetNode < PathNodes.Num())
	{
		float PosterSize = (LastCrossedNode->PosterOwner->GripTailComponent->GetComponentLocation() - LastCrossedNode->PosterOwner->GripHeadComponent->GetComponentLocation()).Size();
		//Check if two nodes on different posters, if so, tp character to other poster
		if (LastCrossedNode->PosterOwner != PathNodes[IndexCurrentTargetNode]->PosterOwner)
		{
			_CrossNextNode();
		}
		else //Else, move character to next node
		{
			if (LastCrossedNode->NodePosition < PathNodes[IndexCurrentTargetNode]->NodePosition)
			{
				LocalPosition += MovingSpeed * DeltaSeconds / PosterSize;
				if (LocalPosition > PathNodes[IndexCurrentTargetNode]->NodePosition)
				{
					_CrossNextNode();
				}
			}
			else
			{
				LocalPosition -= MovingSpeed * DeltaSeconds / PosterSize;
				if (LocalPosition < PathNodes[IndexCurrentTargetNode]->NodePosition)
				{
					_CrossNextNode();
				}
			}
		}
	}

	//if (LastCrossedNode != nullptr && LastCrossedNode->PosterOwner != nullptr)
	//{
	//	FVector CharacWorldPosition = FMath::Lerp<FVector>(LastCrossedNode->PosterOwner->GripHeadComponent->GetComponentLocation(), LastCrossedNode->PosterOwner->GripTailComponent->GetComponentLocation(), LocalPosition);
	//	DrawDebugSphere(World, CharacWorldPosition, 32.0f, 32, FColor::Blue);
	//}
}

bool PathCharacter::MoveTo(const PathNode* TargetNode)
{
	bool bPathFound = false;
	PathNode* CurrentNode;
	PathNode* NextNode;

	//Reinit PathCharacter variables
	IndexCurrentTargetNode = 0;
	LocalPosition = LastCrossedNode->NodePosition;

	//DrawDebugSphere(World, GetNodeLocation(TargetNode), 64, 12, FColor::Red, false, 5.0f);
	//DrawDebugSphere(World, GetNodeLocation(PathMainCharacter.LastCrossedNode), 64, 12, FColor::Green, false, 5.0f);
	if (TargetNode != nullptr)
	{
		//Seek for target node on right
		PathNodes.Empty();
		CurrentNode = LastCrossedNode;
		NextNode = CurrentNode->RightNode;
		PathNodes.Add(CurrentNode);
		while (NextNode != nullptr && !bPathFound)
		{
			//Add next node
			PathNodes.Add(NextNode);
			bPathFound = (NextNode == TargetNode);

			//Go to next node
			if (NextNode->LeftNode == CurrentNode)
			{
				CurrentNode = NextNode;
				NextNode = CurrentNode->RightNode;
			}
			else
			{
				CurrentNode = NextNode;
				NextNode = CurrentNode->LeftNode;
			}
		}

		//Check target not found on right
		if (!bPathFound)
		{
			//Seek for target node on left
			PathNodes.Empty();
			CurrentNode = LastCrossedNode;
			NextNode = CurrentNode->LeftNode;
			PathNodes.Add(CurrentNode);

			while (NextNode != nullptr && !bPathFound)
			{
				//Add next node
				PathNodes.Add(NextNode);
				bPathFound = (NextNode == TargetNode);

				//Go to next node
				if (NextNode->RightNode == CurrentNode)
				{
					CurrentNode = NextNode;
					NextNode = CurrentNode->LeftNode;
				}
				else
				{
					CurrentNode = NextNode;
					NextNode = CurrentNode->RightNode;
				}
			}
		}
	}
	

	//Check target not found on left
	if (!bPathFound)
	{
		PathNodes.Empty();
		PathNodes.Add(LastCrossedNode);
	}

	//Check if character get out of hiding node
	if (bPathFound && LastCrossedNode->NodeType == ENodeType::NT_HiddingNode)
	{
		_LaunchAnimation(ENodeType::NT_HiddingNode, false);
	}

	//UE_LOG(LogGPCode, Log, TEXT("Path Found: %d"), (bPathFound) ? 1 : 0);

	//UE_LOG(LogGPCode, Log, TEXT("Target node position: %s"), *GetNodeLocation(TargetNode).ToString());
	//DrawDebugSphere(World, GetNodeLocation(TargetNode), 35.0f, 32, FColor::Magenta, false, 5.0f);

	return bPathFound;
}

void PathCharacter::SetCharacterNode(PathNode* LastCrossedNode)
{
	PathNodes.Empty();
	PathNodes.Add(LastCrossedNode);
	this->LastCrossedNode = LastCrossedNode;
	LocalPosition = LastCrossedNode->NodePosition;
	IndexCurrentTargetNode = 0;
}

float PathCharacter::GetCharacterPosition(APosterActor* RelativePoster)
{
	float CharacterPosition;
	PathNode* LastPosterNode = CurrentPathGraph->GetLastNode(RelativePoster);
	PathNode* HeadPosterNode = *CurrentPathGraph->MapHeadNodes.Find(RelativePoster);

	//Check if character is in selected poster
	if (LastCrossedNode->PosterOwner == RelativePoster)
	{
		CharacterPosition = LocalPosition;
	}
	//If on right poster
	else if (LastPosterNode->RightNode != nullptr && LastPosterNode->RightNode->PosterOwner == LastCrossedNode->PosterOwner)
	{
		if (LastPosterNode->RightNode->LeftNode == LastPosterNode)
		{
			CharacterPosition = 1.0f + LocalPosition;
		}
		else
		{
			CharacterPosition = 1.0f + (1.0f - LocalPosition);
		}
	}
	//If on left poster
	else if (HeadPosterNode->LeftNode != nullptr && HeadPosterNode->LeftNode->PosterOwner == LastCrossedNode->PosterOwner)
	{
		if (HeadPosterNode->LeftNode->RightNode == HeadPosterNode)
		{
			CharacterPosition = - 1.0f + LocalPosition;
		}
		else
		{
			CharacterPosition = -LocalPosition;
		}
	}
	else
	{
		CharacterPosition = -1.0f;
	}

	return CharacterPosition;
}

void PathCharacter::_LaunchAnimation(TEnumAsByte<ENodeType::Type> CorrespondingNodeType, bool bStarting)
{
	if (CorrespondingNodeType == ENodeType::NT_HiddingNode)
	{
		if (bStarting)
		{
			UE_LOG(LogGPCode, Log, TEXT("Start Hidding Animation"));
			bIsHidden = true;
		}
		else
		{
			UE_LOG(LogGPCode, Log, TEXT("Start exit hidding animation"));
			bIsHidden = false;
		}

		ALiyaCharacter* Liya = Cast<ALiyaCharacter>(World->GetFirstPlayerController()->GetCharacter());
		if (Liya)
		{
			Liya->UpdateHidding(bIsHidden);
		}
	}
}

void PathCharacter::_CrossNextNode()
{
	LocalPosition = PathNodes[IndexCurrentTargetNode]->NodePosition;
	LastCrossedNode = PathNodes[IndexCurrentTargetNode];
	IndexCurrentTargetNode++;

	if (LastCrossedNode->NodeType == ENodeType::NT_CheckpointNode)
	{
		ALiyaCharacter* Liya = Cast<ALiyaCharacter>(World->GetFirstPlayerController()->GetCharacter());
		if (Liya)
		{
			UE_LOG(LogGPCode, Warning, TEXT("PRINCE IS IN RESPAWN NODE"));
			Liya->CurrentRespawnZone = LastCrossedNode->PosterOwner->RespawnZone;
			LastCrossedNode->NodeType = ENodeType::NT_BasicNode;
		}
	}

	if (PathNodes.Num() > 1 && IndexCurrentTargetNode == PathNodes.Num())
	{
		//Check if character reach a hiding node
		if (LastCrossedNode->NodeType == ENodeType::NT_HiddingNode)
		{
			_LaunchAnimation(ENodeType::NT_HiddingNode, true);
		}
		//Check if character reach a door node
		else if (LastCrossedNode->NodeType == ENodeType::NT_DoorNode)
		{
			if (LastCrossedNode->PosterOwner->DoorLinkedPoster != nullptr)
			{
				PathNode* LinkedNode = CurrentPathGraph->GetDoorNode(LastCrossedNode->PosterOwner->DoorLinkedPoster);
				if (LinkedNode != nullptr)
				{
					LinkedNode->NodeType = LastCrossedNode->NodeType = ENodeType::NT_BasicNode;
					//Set poster grabbable when character go through a poster
					LastCrossedNode->PosterOwner->bIsGrabbable = true;

					SetCharacterNode(LinkedNode);
					if (LinkedNode->RightNode != nullptr && LinkedNode->RightNode->NodeType != ENodeType::NT_SideNode)
					{
						MoveTo(LinkedNode->RightNode);
					}
					else if (LinkedNode->LeftNode != nullptr && LinkedNode->LeftNode->NodeType != ENodeType::NT_SideNode)
					{
						MoveTo(LinkedNode->LeftNode);
					}
				}
			}
		}
	}
}
