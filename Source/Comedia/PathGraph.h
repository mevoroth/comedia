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
	TArray<PathNode*> ArrayNodes;
	TMap<APosterActor*, PathNode*> MapHeadNodes;
	PathNode* StartNode;

	PathGraph();
	~PathGraph();

	void  InitNodes(UWorld* World);
	void  UpdatePosterNodes(APosterActor* Poster);
	void  DrawNodes();
	void  DrawPath(PathNode* NodeOfPath);
	FVector GetNodeLocation(const PathNode* PosterNode) const;
	PathNode* GetRandomNode();
	void Tick(float DeltaSeconds);
	const PathNode* GetNode(const FVector& Location, const APosterActor* Poster) const;
	PathNode* GetDoorNode(const APosterActor* Poster) const;
	PathNode* GetLastNode(APosterActor* Poster);

	PathNode* GetLastNode(APosterActor* Poster);

private:
	UWorld* World;
};
