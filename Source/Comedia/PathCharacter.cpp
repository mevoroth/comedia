// All rights reserved

#include "Comedia.h"
#include "PathCharacter.h"

PathCharacter::PathCharacter()
{
	LocalPosition = 0.0f;
	IndexCurrentTargetNode = 0;
	PathNodes = TArray<PathNode*>();
}

PathCharacter::~PathCharacter()
{
}

void PathCharacter::UpdateCharacter(float DeltaSeconds)
{
	if (PathNodes.Num() > 0 && IndexCurrentTargetNode < PathNodes.Num() - 1)
	{
		//Check if two nodes on different posters, if so, tp character to other poster
		if (LastCrossedNode->PosterOwner != PathNodes[IndexCurrentTargetNode]->PosterOwner)
		{
			_CrossNextNode();
		}
		else //Else, move character to next node
		{
			if (LastCrossedNode->NodePosition < PathNodes[IndexCurrentTargetNode]->NodePosition)
			{
				LocalPosition += MovingSpeed * DeltaSeconds;
				if (LocalPosition > PathNodes[IndexCurrentTargetNode]->NodePosition)
				{
					_CrossNextNode();
				}
			}
			else
			{
				LocalPosition -= MovingSpeed * DeltaSeconds;
				if (LocalPosition < PathNodes[IndexCurrentTargetNode]->NodePosition)
				{
					_CrossNextNode();
				}
			}
		}
	}
}

void PathCharacter::SetCharacterNode(PathNode* LastCrossedNode)
{
	PathNodes.Empty();
	PathNodes.Add(LastCrossedNode);
	this->LastCrossedNode = LastCrossedNode;
	LocalPosition = LastCrossedNode->NodePosition;
	IndexCurrentTargetNode = 0;
}

void PathCharacter::_CrossNextNode()
{
	LocalPosition = PathNodes[IndexCurrentTargetNode]->NodePosition;
	LastCrossedNode = PathNodes[IndexCurrentTargetNode];
	IndexCurrentTargetNode++;
}
