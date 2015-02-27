// All rights reserved

#pragma once

#include "PathNode.h"

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
	float MovingSpeed = 1.0f;

	UWorld* World;

	PathCharacter();
	~PathCharacter();

	void UpdateCharacter(float DeltaSeconds);
	void SetCharacterNode(PathNode* LastCrossedNode);

private:
	void _CrossNextNode();
};
