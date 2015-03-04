// All rights reserved

#include "Comedia.h"
#include "PathCharacter.h"

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

void PathCharacter::_LaunchAnimation(TEnumAsByte<ENodeType::Type> CorrespondingNodeType, bool bStarting)
{
	if (CorrespondingNodeType == ENodeType::NT_HiddingNode)
	{
		if (bStarting)
		{
			UE_LOG(LogGPCode, Log, TEXT("Start Hidding Animation"));
		}
		else
		{
			UE_LOG(LogGPCode, Log, TEXT("Start exit hidding animation"));
		}
	}
}

void PathCharacter::_CrossNextNode()
{
	LocalPosition = PathNodes[IndexCurrentTargetNode]->NodePosition;
	LastCrossedNode = PathNodes[IndexCurrentTargetNode];
	IndexCurrentTargetNode++;

	//Check if character reach a hiding node
	if (PathNodes.Num() > 1 && IndexCurrentTargetNode == PathNodes.Num() && LastCrossedNode->NodeType == ENodeType::NT_HiddingNode)
	{
		_LaunchAnimation(ENodeType::NT_HiddingNode, true);
	}
}
