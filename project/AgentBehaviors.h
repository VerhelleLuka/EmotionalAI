#pragma once
#include "SteeringBehaviors.h"
#include "EBlackboard.h"
#include "EDecisionMaking.h"
class AgentBehaviors
{
public:
	AgentBehaviors();
	~AgentBehaviors();

	void SetToSeek();
	void SetToWander();
	void SetToFace();
	void SetToTurnAround();
	void SetToEvade();
	void SetDecisionMaking(IDecisionMaking* pDecision);
	void SetTarget(const Elite::Vector2& target);

	 ISteeringBehavior* GetCurrentSteering();
	const ISteeringBehavior* GetTurningAround();
	 IDecisionMaking* GetDecisionMaking();

private:
	IDecisionMaking* m_pDecisions = nullptr;
	//Behaviors
	ISteeringBehavior* m_pCurrentSteering = nullptr;
	ISteeringBehavior* m_pWander = nullptr;
	ISteeringBehavior* m_pSeek = nullptr;
	ISteeringBehavior* m_pFace = nullptr;
	ISteeringBehavior* m_pTurnAround = nullptr;
	ISteeringBehavior* m_pEvade = nullptr;

};
