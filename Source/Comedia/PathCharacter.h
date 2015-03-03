// All rights reserved

#pragma once

#include "PathNode.h"
#include "PathGraph.h"

/**
 * 
 */
class COMEDIA_API PathCharacter
{
public:
	TArray<PathNode*> PathNodes;
	PathNode* LastCrossedNode;
	int32 IndexCurrentTargetNode;
	float LocalPosition;
	float MovingSpeed = 300.0f;

	PathGraph* CurrentPathGraph;
	UWorld* World;

	PathCharacter();
	~PathCharacter();

	bool MoveTo(const PathNode* TargetNode);
	void UpdateCharacter(float DeltaSeconds);
	void SetCharacterNode(PathNode* LastCrossedNode);
	float GetCharacterPosition(APosterActor* RelativePoster);

private:
	void _CrossNextNode();
	void _LaunchAnimation(TEnumAsByte<ENodeType::Type> CorrespondingNodeType, bool bStarting);
};
