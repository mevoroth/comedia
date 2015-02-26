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

	PathCharacter();
	~PathCharacter();

	void UpdateCharacter(float DeltaSeconds);
	void SetLastNode(PathNode* LastCrossedNode);

private:
	void _CrossNextNode();
};
