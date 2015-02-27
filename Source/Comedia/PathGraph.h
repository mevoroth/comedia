// All rights reserved

#pragma once

#include "PosterActor.h"
#include "PathCharacter.h"
#include "PathNode.h"

/**
 * 
 */
class COMEDIA_API PathGraph
{
public:

	TArray<PathNode*> ArrayNodes;
	TMap<APosterActor*, PathNode*> MapHeadNodes;
	PathCharacter PathMainCharacter;

	PathGraph();
	~PathGraph();

	void  InitNodes(UWorld* World);
	void  UpdatePosterNodes(APosterActor* Poster);
	float GetCharacterPosition(APosterActor* Poster);
	bool  MoveCharacterTo(const PathNode* TargetNode);
	void  DrawNodes();
	void  DrawPath(PathNode* NodeOfPath);
	FVector GetNodeLocation(const PathNode* PosterNode) const;
	PathNode* GetRandomNode();
	void Tick(float DeltaSeconds);
	const PathNode* GetNode(const FVector& Location, const APosterActor* Poster) const;

private:
	UWorld* World;

	PathNode* _GetLastNode(APosterActor* Poster);
};
