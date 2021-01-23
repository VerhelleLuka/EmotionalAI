#include "stdafx.h"
#include "AgentBehaviors.h"

AgentBehaviors::AgentBehaviors()
	:m_pSeek{new Seek()}
	,m_pWander{new Wander()}
	,m_pFace{new Face()}
	,m_pTurnAround{new TurnAround()}
	,m_pEvade{new Evade()}
{
	m_pCurrentSteering = m_pWander;
}

AgentBehaviors::~AgentBehaviors()
{
	//SAFE_DELETE(m_pCurrentSteering);
	SAFE_DELETE(m_pSeek);
	SAFE_DELETE(m_pWander);
	SAFE_DELETE(m_pFace);
	SAFE_DELETE(m_pTurnAround);
	SAFE_DELETE(m_pDecisions);
	SAFE_DELETE(m_pEvade);

}

void AgentBehaviors::SetToSeek()
{
	m_pCurrentSteering = m_pSeek;
}
void AgentBehaviors::SetToWander()
{
	m_pCurrentSteering = m_pWander;
}

void AgentBehaviors::SetToFace()
{
	m_pCurrentSteering = m_pFace;
}
void AgentBehaviors::SetToTurnAround()
{
	m_pCurrentSteering = m_pTurnAround;
}

void AgentBehaviors::SetToEvade()
{
	m_pCurrentSteering = m_pEvade;
}
void AgentBehaviors::SetDecisionMaking(IDecisionMaking* pDecision)
{
	m_pDecisions = pDecision;
}
void AgentBehaviors::SetTarget(const Elite::Vector2& target)
{
	m_pCurrentSteering->SetTarget(target);
}

//GETTERS
 ISteeringBehavior* AgentBehaviors::GetCurrentSteering()
{
	return m_pCurrentSteering;
}
const ISteeringBehavior* AgentBehaviors::GetTurningAround()
{
	return m_pTurnAround;
}
 IDecisionMaking* AgentBehaviors::GetDecisionMaking()
{
	return m_pDecisions;
}