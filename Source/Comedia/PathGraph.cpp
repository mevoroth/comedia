// All rights reserved

#include "Comedia.h"
#include "PathGraph.h"

PathGraph::PathGraph()
{
	UE_LOG(LogGPCode, Log, TEXT("PathGraph Constructor"));
}

PathGraph::~PathGraph()
{
	MapHeadNodes = TMap<APosterActor*, PathNode*>();
}

void PathGraph::InitNodes(UWorld* World)
{
	//Reset MapHeadNotes
	MapHeadNodes.Reset();

	//Get all posters
	TArray<AActor*> ArrayPosterActors;
	UGameplayStatics::GetAllActorsOfClass(World, APosterActor::StaticClass(), ArrayPosterActors);

	for (int i = 0; i < ArrayPosterActors.Num(); i++)
	{
		APosterActor* CurrentPosterActor = Cast<APosterActor>(ArrayPosterActors[i]);

		//Create head node
		PathNode* HeadNode = new PathNode();
		HeadNode->NodePosition = 0.0f;
		HeadNode->PosterOwner = CurrentPosterActor;
		HeadNode->LeftNode = nullptr;

		//Create key points nodes
		PathNode* LastKeyNode = HeadNode;
		for (int j = 0; j < CurrentPosterActor->KeyPoints.Num(); j++)
		{
			//Create new node with its data
			PathNode* KeypointNode = new PathNode();
			KeypointNode->NodePosition = CurrentPosterActor->KeyPoints[i];
			KeypointNode->PosterOwner = CurrentPosterActor;
			
			//Link two nodes
			KeypointNode->LeftNode = LastKeyNode;
			LastKeyNode->RightNode = KeypointNode;

			//Store current node as last node
			LastKeyNode = KeypointNode;
		}

		//Create tail node
		PathNode* TailNode = new PathNode();
		TailNode->NodePosition = 1.0f;
		TailNode->PosterOwner = CurrentPosterActor;
		TailNode->RightNode = nullptr;
		TailNode->LeftNode = LastKeyNode;

		//Add head node to MapHeadNotes
		MapHeadNodes.Add(CurrentPosterActor, HeadNode);
	}
}

void PathGraph::UpdatePosterNodes(APosterActor* Poster)
{

}

float PathGraph::GetCharacterPosition(APosterActor* Poster)
{
	return 0.5f;
}

bool PathGraph::MoveCharacterTo(PathNode* TargetNode)
{
	return false;
}

PathNode* PathGraph::_GetLastNode(APosterActor* Poster)
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