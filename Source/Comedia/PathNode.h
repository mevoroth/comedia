// All rights reserved

#pragma once

#include "PosterActor.h"

/**
 * 
 */
class COMEDIA_API PathNode
{
public:

	PathNode* LeftNode;
	PathNode* RightNode;
	APosterActor* PosterOwner;
	float NodePosition;
	TEnumAsByte<ENodeType::Type> NodeType;

	PathNode();
	~PathNode();
};
