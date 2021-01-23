/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "../Shared/Agario/AgarioAgent.h"
#include "../Shared/Agario/AgarioFood.h"
#include "../App_Steering/SteeringBehaviors.h"
#include "framework\EliteAI\EliteGraphs\EInfluenceMap.h" 
#include "framework\EliteAI\EliteGraphs\EGraph2D.h" 
#include "framework\EliteAI\EliteGraphs\EGridGraph.h" 
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphEditor.h"
#include "framework\EliteAI\EliteGraphs\EliteGraphUtilities\EGraphRenderer.h"
#include<algorithm>
//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
bool IsCloseToFood(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioFood*>* foodVec = nullptr;

	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("FoodVec", foodVec);

	if (!pAgent || !foodVec)
		return false;

	//TODO: Check for food closeby and set target accordingly
	const float closeToFoodRange{ 20.0f };
	auto foodIt = std::find_if((*foodVec).begin(), (*foodVec).end(), [&pAgent, &closeToFoodRange](AgarioFood* f ) 
		{
			return DistanceSquared(pAgent->GetPosition(), f->GetPosition()) < (closeToFoodRange * closeToFoodRange);
		});
	if (foodIt != (*foodVec).end())
	{
		pBlackboard->ChangeData("Target", (*foodIt)->GetPosition());
		return true;
	}

	return false;
}

bool BigEnemyClose(Elite::Blackboard* pB)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioAgent*>* pEnemies = nullptr;
	pB->GetData("Agent", pAgent);
	bool dataAvailable = pB->GetData("AgentsVec", pEnemies);
	if (!dataAvailable)
		return false;
	std::vector<AgarioAgent*>::iterator it = std::find_if(pEnemies->begin(), pEnemies->end(), [pAgent](AgarioAgent* pA)
		{
			return Distance(pA->GetPosition(), pAgent->GetPosition()) < pAgent->GetRadius() + 20.0f + pA->GetRadius() && pAgent->GetRadius() < pA->GetRadius() - 1;
		});
	if (it == pEnemies->end() || (*it)->CanBeDestroyed())
	{
		return false;
	}
	pB->ChangeData("Target", (*it)->GetPosition());
	return true;

}
bool SmallEnemyClose(Elite::Blackboard* pB)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<AgarioAgent*>* pEnemies = nullptr;
	pB->GetData("Agent", pAgent);
	bool dataAvailable = pB->GetData("AgentsVec", pEnemies);
	if (!dataAvailable)
		return false;
	std::vector<AgarioAgent*>::iterator it = std::find_if(pEnemies->begin(), pEnemies->end(), [pAgent](AgarioAgent* pA)
		{
			return Distance(pA->GetPosition(), pAgent->GetPosition()) < pAgent->GetRadius() + 20.0f + pA->GetRadius() && pAgent->GetRadius() > pA->GetRadius() + 1;
		});
	if (it == pEnemies->end() || (*it)->CanBeDestroyed())
	{
		return false;
	}
	pB->ChangeData("Target", (*it)->GetPosition());
	return true;

}



BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent);

	if (!pAgent)
	{
		return Failure;
	}
	pAgent->SetToWander();

	return Success;
}

BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Elite::Vector2 seekTarget{};
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("Target", seekTarget);
	if (!dataAvailable)
	{
		return Failure;
	}
	if (!pAgent)
		return Failure;
	
	//TODO: Implement Change to seek (Target)
	pAgent->SetToSeek(seekTarget);

	return Success;
}
BehaviorState ChangeToFlee(Elite::Blackboard* pBlackboard)
{
	AgarioAgent* pAgent = nullptr;
	Elite::Vector2 seekTarget{};
	auto dataAvailable = pBlackboard->GetData("Agent", pAgent) &&
		pBlackboard->GetData("Target", seekTarget);
	if (!dataAvailable)
	{
		return Failure;
	}
	if (!pAgent)
		return Failure;

	//TODO: Implement Change to seek (Target)
	pAgent->SetToFlee(seekTarget);

	return Success;
}

bool GoToBestInfluence(Elite::Blackboard* pB)
{
	AgarioAgent* pAgent = nullptr;
	std::vector<InfluenceNode*> influenceNodes{};
	Elite::InfluenceMap<Elite::GridGraph<Elite::InfluenceNode, Elite::GraphConnection>>* iMap = nullptr;

	
	auto dataAvailable = pB->GetData("Agent", pAgent) &&
		pB->GetData("InfluenceMap", iMap);
	if (!dataAvailable)
	{
		return false;
	}
	if (!pAgent)
		return false;
	auto nodeAtPlayer = iMap->GetNodeIdxAtWorldPos(pAgent->GetPosition());
	auto nodesAroundPlayer = iMap->GetConnections(nodeAtPlayer);
	float highestInfluence = -100;
	Elite::Vector2 pos{ pAgent->GetPosition() };
	
	for (auto& nodes : nodesAroundPlayer)
	{
		
		auto nodeToCheck = iMap->GetNode(nodes->GetTo());
		if (nodeToCheck->GetInfluence() > highestInfluence)
		{
			highestInfluence = nodeToCheck->GetInfluence();
			pos = iMap->GetNodeWorldPos(nodes->GetTo());
			
		}
	}
	std::cout << "done" << "\n";
	pB->ChangeData("Target", pos);
	return true;

}

#endif