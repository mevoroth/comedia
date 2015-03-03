// All rights reserved

#include "Comedia.h"
#include "PathNode.h"

PathNode::PathNode()
{
}

PathNode::~PathNode()
{
}

FVector PathNode::GetNodeLocation() const
{
	FVector HeadPosition = PosterOwner->GripHeadComponent->GetComponentLocation();
	FVector TailPosition = PosterOwner->GripTailComponent->GetComponentLocation();

	return FMath::Lerp<FVector>(HeadPosition, TailPosition, NodePosition);
}
