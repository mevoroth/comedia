// All rights reserved

#include "Comedia.h"
#include "PathGraph.h"

#include <limits>

PathGraph::PathGraph()
{
	StartNode = nullptr;
}

PathGraph::~PathGraph()
{
}

void PathGraph::InitNodes(UWorld* World)
{
	this->World = World;

	//Reset MapHeadNotes
	MapHeadNodes.Reset();
	ArrayNodes.Reset();

	//Get all posters
	TArray<AActor*> ArrayPosterActors;
	UGameplayStatics::GetAllActorsOfClass(World, APosterActor::StaticClass(), ArrayPosterActors);

	for (int i = 0; i < ArrayPosterActors.Num(); i++)
	{
		APosterActor* CurrentPosterActor = Cast<APosterActor>(ArrayPosterActors[i]);

		CurrentPosterActor->InitGripReferences();

		//Create head node
		PathNode* HeadNode = new PathNode();
		ArrayNodes.Add(HeadNode);
		HeadNode->NodePosition = 0.0f;
		HeadNode->PosterOwner = CurrentPosterActor;
		HeadNode->LeftNode = nullptr;
		HeadNode->NodeType = ENodeType::NT_SideNode;

		//Create key points nodes
		PathNode* LastKeyNode = HeadNode;
		for (int j = 0; j < CurrentPosterActor->KeyPoints.Num(); j++)
		{
			//Create new node with its data
			PathNode* KeypointNode = new PathNode();
			ArrayNodes.Add(KeypointNode);
			KeypointNode->NodePosition = CurrentPosterActor->KeyPoints[j];
			KeypointNode->PosterOwner = CurrentPosterActor;
			if (CurrentPosterActor->KeyNodeTypes[j])
			{
				KeypointNode->NodeType = CurrentPosterActor->KeyNodeTypes[j];

				//Store the start node
				if (CurrentPosterActor->KeyNodeTypes[j] == ENodeType::NT_StartNode)
				{
					if (StartNode == nullptr)
					{
						StartNode = KeypointNode;
					}
					else
					{
						UE_LOG(LogGPCode, Warning, TEXT("Warning! More than on start node!"));
					}
				}
			}
			
			//Link two nodes
			KeypointNode->LeftNode = LastKeyNode;
			LastKeyNode->RightNode = KeypointNode;

			//Store current node as last node
			LastKeyNode = KeypointNode;
		}

		//Create tail node
		PathNode* TailNode = new PathNode();
		ArrayNodes.Add(TailNode);
		TailNode->NodePosition = 1.0f;
		TailNode->PosterOwner = CurrentPosterActor;
		TailNode->RightNode = nullptr;
		TailNode->LeftNode = LastKeyNode;
		TailNode->NodeType = ENodeType::NT_SideNode;
		LastKeyNode->RightNode = TailNode;

		//Add head node to MapHeadNotes
		MapHeadNodes.Add(CurrentPosterActor, HeadNode);
	}

	//Connect posters
	for (int i = 0; i < ArrayPosterActors.Num(); i++)
	{
		UpdatePosterNodes(Cast<APosterActor>(ArrayPosterActors[i]));
	}
}

void PathGraph::UpdatePosterNodes(APosterActor* Poster)
{
	//Remove previous link with other posters
	PathNode** PtrHeadPosterNode = MapHeadNodes.Find(Poster);
	if (PtrHeadPosterNode != nullptr)
	{
		(*PtrHeadPosterNode)->LeftNode = nullptr;
		GetLastNode(Poster)->RightNode = nullptr;
		////Break left link with other poster
		//if ((*PtrHeadPosterNode)->LeftNode != nullptr)
		//{
		//	if ((*PtrHeadPosterNode)->LeftNode->RightNode == (*PtrHeadPosterNode))
		//	{
		//		(*PtrHeadPosterNode)->LeftNode->RightNode = nullptr;
		//	}
		//	else
		//	{
		//		(*PtrHeadPosterNode)->LeftNode->LeftNode = nullptr;
		//	}
		//	(*PtrHeadPosterNode)->LeftNode = nullptr;
		//}

		////Break right link with other poster
		//PathNode* LastNodePoster = GetLastNode(Poster);
		//if (LastNodePoster != nullptr && LastNodePoster->RightNode != nullptr)
		//{
		//	if (LastNodePoster->RightNode->LeftNode == LastNodePoster)
		//	{
		//		LastNodePoster->RightNode->LeftNode = nullptr;
		//	}
		//	else
		//	{
		//		LastNodePoster->RightNode->RightNode = nullptr;
		//	}
		//	LastNodePoster->RightNode = nullptr;
		//}

		//Update Head
		bool bLinkOtherPosterHead = false;
		TArray<UPrimitiveComponent*> OverlappingComponentsHead = TArray<UPrimitiveComponent*>();
		Poster->GripHeadComponent->GetOverlappingComponents(OverlappingComponentsHead);
		for (int i = 0; i < OverlappingComponentsHead.Num() && !bLinkOtherPosterHead; i++)
		{
			//Check if overlapping compo is a sphere compo
			USphereComponent* GripComponent = Cast<USphereComponent>(OverlappingComponentsHead[i]);
			if (GripComponent)
			{
				//Check if other poster is different than current poster
				APosterActor* OtherPoster = Cast<APosterActor>(GripComponent->GetOwner());
				if (OtherPoster != nullptr && OtherPoster != Poster)
				{
					if (OtherPoster->GripHeadComponent == GripComponent)
					{
						//Attach to other head
						PathNode** PtrHeadOtherPosterNode = MapHeadNodes.Find(OtherPoster);
						if (PtrHeadOtherPosterNode != nullptr && *PtrHeadOtherPosterNode != nullptr)
						{
							(*PtrHeadOtherPosterNode)->LeftNode = *PtrHeadPosterNode;
							(*PtrHeadPosterNode)->LeftNode = *PtrHeadOtherPosterNode;
						}
						bLinkOtherPosterHead = true;
					}
					else if (OtherPoster->GripTailComponent == GripComponent)
					{
						//Attach to other tail
						PathNode* TailOtherPosterNode = GetLastNode(OtherPoster);
						if (TailOtherPosterNode != nullptr)
						{
							TailOtherPosterNode->RightNode = (*PtrHeadPosterNode);
							(*PtrHeadPosterNode)->LeftNode = TailOtherPosterNode;
						}
						bLinkOtherPosterHead = true;
					}
				}
			}
		}

		//Update Tail
		PathNode* TailPosterNode = GetLastNode(Poster);
		if (TailPosterNode != nullptr)
		{
			bool bLinkOtherPosterTail = false;
			TArray<UPrimitiveComponent*> OverlappingComponentsTail;
			Poster->GripTailComponent->GetOverlappingComponents(OverlappingComponentsTail);
			for (int i = 0; i < OverlappingComponentsTail.Num() && !bLinkOtherPosterTail; i++)
			{
				//Check if overlapping compo is a sphere compo
				USphereComponent* GripComponent = Cast<USphereComponent>(OverlappingComponentsTail[i]);
				APosterActor* PosterOwner = (GripComponent) ? Cast<APosterActor>(GripComponent->GetOwner()) : nullptr;
				if (GripComponent && PosterOwner)
				{
					//Check if other poster is different than current poster
					APosterActor* OtherPoster = Cast<APosterActor>(GripComponent->GetOwner());
					if (OtherPoster != Poster)
					{
						if (OtherPoster->GripHeadComponent == GripComponent)
						{
							//Attach to other head
							PathNode** PtrHeadOtherPosterNode = MapHeadNodes.Find(OtherPoster);
							if (PtrHeadOtherPosterNode != nullptr && *PtrHeadOtherPosterNode != nullptr)
							{
								(*PtrHeadOtherPosterNode)->LeftNode = TailPosterNode;
								TailPosterNode->RightNode = *PtrHeadOtherPosterNode;
							}
							bLinkOtherPosterHead = true;
						}
						else if (OtherPoster->GripTailComponent == GripComponent)
						{
							//Attach to other tail
							PathNode* TailOtherPosterNode = GetLastNode(OtherPoster);
							if (TailOtherPosterNode != nullptr)
							{
								TailOtherPosterNode->RightNode = TailPosterNode;
								TailPosterNode->RightNode = TailOtherPosterNode;
							}
							bLinkOtherPosterHead = true;
						}
					}
				}
			}
		}
	}
}

void PathGraph::DrawNodes()
{
	FColor RandomColor;
	TArray<APosterActor*> Posters;
	MapHeadNodes.GetKeys(Posters);

	for (int i = 0; i < Posters.Num(); i++)
	{
		//Draw Nodes
		RandomColor = FColor::MakeRandomColor();
		PathNode** PtrHeadPosterNode = MapHeadNodes.Find(Posters[i]);
		PathNode* TailPosterNode = GetLastNode(Posters[i]);

		if (PtrHeadPosterNode != nullptr && *PtrHeadPosterNode != nullptr && TailPosterNode != nullptr)
		{
			PathNode* CurrentNode = *PtrHeadPosterNode;
			FVector HeadPosition = (Posters[i])->GripHeadComponent->ComponentToWorld.GetLocation();
			FVector TailPosition = (Posters[i])->GripTailComponent->ComponentToWorld.GetLocation();
			DrawDebugSphere(World, HeadPosition, 25.0f, 32, RandomColor);

			while (CurrentNode != TailPosterNode)
			{
				CurrentNode = CurrentNode->RightNode;
				FVector NodePosition = FMath::Lerp<FVector>(HeadPosition, TailPosition, CurrentNode->NodePosition);
				DrawDebugSphere(World, NodePosition, 25.0f, 32, RandomColor);
			}
		}
	}
}

void PathGraph::DrawPath(PathNode* NodeOfPath)
{
	PathNode* CurrentNode;
	PathNode* NextNode;

	//Draw right path
	CurrentNode = NodeOfPath;
	NextNode = NodeOfPath->RightNode;
	while (NextNode != nullptr)
	{
		//Draw path segment
		DrawDebugLine(World, GetNodeLocation(CurrentNode), GetNodeLocation(NextNode), FColor::Blue);

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

	//Draw left path
	CurrentNode = NodeOfPath;
	NextNode = NodeOfPath->LeftNode;
	while (NextNode != nullptr)
	{
		//Draw path segment
		DrawDebugLine(World, GetNodeLocation(CurrentNode), GetNodeLocation(NextNode), FColor::Blue);

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

FVector PathGraph::GetNodeLocation(const PathNode* PosterNode) const
{
	return PosterNode->GetNodeLocation();
}

PathNode* PathGraph::GetRandomNode()
{
	TArray<PathNode*> ArrayNodes;
	MapHeadNodes.GenerateValueArray(ArrayNodes);

	return ArrayNodes[FMath::RandRange(0, ArrayNodes.Num() - 1)];
}

const PathNode* PathGraph::GetNode(const FVector& Location, const APosterActor* Poster) const
{
	const PathNode* Current = *MapHeadNodes.Find(Poster);
	const PathNode* ClosestNode = nullptr;
	float Dist = FVector::DistSquared(Location, GetNodeLocation(Current));
	float TmpDist = 0.f;
	while (Current->RightNode
		&& Current->RightNode->PosterOwner == Poster)
	{
		if (Current->NodeType != ENodeType::NT_SideNode)
		{
			ClosestNode = Current;
		}

		TmpDist = FVector::DistSquared(Location, GetNodeLocation(Current->RightNode));
		if (TmpDist >= Dist && ClosestNode != nullptr)
		{
			break;
		}
		Dist = TmpDist;
		Current = Current->RightNode;
	}
	return ClosestNode;
}

PathNode* PathGraph::GetDoorNode(const APosterActor* Poster) const
{
	PathNode* Current = *MapHeadNodes.Find(Poster);
	PathNode* DoorNode = nullptr;

	while (Current->RightNode != nullptr && Current->RightNode->PosterOwner == Poster && DoorNode == nullptr)
	{
		if (Current->NodeType == ENodeType::NT_DoorNode)
		{
			DoorNode = Current;
		}

		Current = Current->RightNode;
	}

	return DoorNode;
}

PathNode* PathGraph::GetRespawnNode(const APosterActor* Poster) const
{
	PathNode* Current = *MapHeadNodes.Find(Poster);
	PathNode* RespawnNode = nullptr;

	while (Current->RightNode != nullptr && Current->RightNode->PosterOwner == Poster && RespawnNode == nullptr)
	{
		if (Current->NodeType == ENodeType::NT_RespawnNode || Current->NodeType == ENodeType::NT_StartNode)
		{
			RespawnNode = Current;
		}

		Current = Current->RightNode;
	}

	return RespawnNode;
}

void PathGraph::Tick(float DeltaSeconds)
{
}

PathNode* PathGraph::GetLastNode(APosterActor* Poster)
{
	PathNode* LastPosterNode = nullptr;

	//Get pointer to head node corresponding to this poster in MapHeadNodes
	PathNode** PtrHeadPosterNode = MapHeadNodes.Find(Poster);
	if (PtrHeadPosterNode != nullptr)
	{
		LastPosterNode = *PtrHeadPosterNode;

		//Go to last node parented by this poster
		while (LastPosterNode->RightNode != nullptr && LastPosterNode->RightNode->PosterOwner == Poster)
		{
			LastPosterNode = LastPosterNode->RightNode;
		}
	}

	return LastPosterNode;
}
