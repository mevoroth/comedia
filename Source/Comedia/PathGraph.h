// All rights reserved

#pragma once

#include "PosterActor.h"
#include "PathNode.h"

/**
 * 
 */
class COMEDIA_API PathGraph
{
public:

	TMap<APosterActor*, PathNode*> MapHeadNodes;

	PathGraph();
	~PathGraph();

	void  InitNodes(UWorld* World);
	void  UpdatePosterNodes(APosterActor* Poster);
	float GetCharacterPosition(APosterActor* Poster);
	bool  MoveCharacterTo(PathNode* TargetNode);
	void  DrawNodes();
	void  DrawPath(PathNode* NodeOfPath);
	FVector GetNodeLocation(PathNode* PosterNode);
	PathNode* GetRandomNode();

private:
	UWorld* World;

	PathNode* _GetLastNode(APosterActor* Poster);
};
