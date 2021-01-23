//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "EBlackboard.h"
#include "SteeringBehaviors.h"

//SEEK
//****
SteeringPlugin_Output Seek::CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion)
{
	SteeringPlugin_Output steering = {};
	Elite::Vector2 target{ m_Target.Position.x, m_Target.Position.y };
	steering.LinearVelocity = pInterface->NavMesh_GetClosestPathPoint(target) - pInterface->Agent_GetInfo().Position; //Desired velocity
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pInterface->Agent_GetInfo().MaxLinearSpeed;



	return steering;
}


//WANDER (base> SEEK)
//******
SteeringPlugin_Output Wander::CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion)
{
	SteeringPlugin_Output steering;
	steering.AutoOrient = true;
	if(pEmotion->fearFactors.size() <= 10)
		m_Offset = 22.f - (10.f + (pEmotion->timidBrave * 0.1f * (pEmotion->fearFactors.size() +4)));
	else
	{
		m_Offset = 22.f - (10.f + (pEmotion->timidBrave * 0.1f * 10.f));
	}
	//std::cout << m_Offset<<"FEAR OFFSET\n";

	Elite::Vector2 circlePos{ cos(pInterface->Agent_GetInfo().Orientation - ToRadians(90)) * m_Offset + pInterface->Agent_GetInfo().Position.x,
	sin(pInterface->Agent_GetInfo().Orientation - ToRadians(90)) * m_Offset + pInterface->Agent_GetInfo().Position.y };
	


	
	m_WanderAngle += ((rand() % int(m_AngleChange * 200)) - int(m_AngleChange * 100)) / 100.f;

	Elite::Vector2 targetPos{ (float)cos(m_WanderAngle) * m_Radius, (float)sin(m_WanderAngle) * m_Radius };
	targetPos += circlePos;

	m_Target = targetPos;
	//steering.AngularVelocity += pInterface->Agent_GetInfo().MaxAngularSpeed;

	steering = { Seek::CalculateSteering(deltaT, pInterface, pEmotion) };



	return steering;
}
SteeringPlugin_Output Flee::CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion)
{
	float distanceToTarget = Distance(pInterface->Agent_GetInfo().Position, m_Target.Position);
	
	if (distanceToTarget > m_FleeRadius)
	{
		SteeringPlugin_Output steering;
		return steering;
	}

	SteeringPlugin_Output steering = Seek::CalculateSteering(deltaT, pInterface, pEmotion);
	steering.LinearVelocity = steering.LinearVelocity * -1;
	return steering;

	return steering;

}
//SteeringOutput Arrive::CalculateSteering(float deltaT, IExamInterface* pAgent)
//{
//	const float slowRadius = 1.f;
//	const float arrivedRadius = 2.f;
//	SteeringOutput steering{};
//	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
//	
//	if (steering.LinearVelocity.Magnitude() < slowRadius && steering.LinearVelocity.Magnitude() > arrivedRadius)
//	{
//		steering.LinearVelocity.Normalize();
//		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * (steering.LinearVelocity.Magnitude() / slowRadius);
//	}
//	else if(steering.LinearVelocity.Magnitude() < arrivedRadius)
//	{
//		steering.LinearVelocity.Normalize();
//		steering.LinearVelocity = {0, 0};
//	}
//	else
//	{
//		steering.LinearVelocity.Normalize();
//		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
//	}
//
//	return steering;
//	
//}
//
SteeringPlugin_Output Face::CalculateSteering(float deltaT, IExamInterface* pAgent, Emotion* pEmotion)
{
	SteeringPlugin_Output steering{};
	steering.AutoOrient = false;
	Elite::Vector2 directionVec{ m_Target.Position - pAgent->Agent_GetInfo().Position };
	directionVec = directionVec.GetNormalized();

	float angle = ToDegrees(atan2(directionVec.y, directionVec.x));//Hiermee vindt ge de hoek van uw directionVec (en zet naar graden om)
	float playerAngle = ToDegrees(pAgent->Agent_GetInfo().Orientation) - 90;//Haal -90 wweg en ge ziet wat er gebeurt
	float angleDifference = angle - playerAngle;
	while (angleDifference > 180.f)
	{
		angleDifference -= 360.f;
	}
	while (angleDifference < -180.f)
	{
		angleDifference += 360.f;
	}
	//std::cout << m_Target.Position.x <<" "<<m_Target.Position.y <<"FACE\n";
	steering.AngularVelocity =Clamp( angleDifference, -pAgent->Agent_GetInfo().MaxAngularSpeed, pAgent->Agent_GetInfo().MaxAngularSpeed);
	//steering.AngularVelocity = angleDifference > 0 ? pAgent->Agent_GetInfo().MaxAngularSpeed : -pAgent->Agent_GetInfo().MaxAngularSpeed;
		return steering;
}
SteeringPlugin_Output TurnAround::CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion)
{
	SteeringPlugin_Output steering{};
	steering.AutoOrient = false;
	steering.AngularVelocity += pInterface->Agent_GetInfo().MaxAngularSpeed;
	steering.LinearVelocity = pInterface->Agent_GetInfo().LinearVelocity; 
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pInterface->Agent_GetInfo().MaxLinearSpeed;
	//pInterface->Agent_GetInfo().LinearVelocity = steering.LinearVelocity;
	return steering;
}
//
//SteeringOutput Pursuit::CalculateSteering(float deltaT, IExamInterface* pAgent)
//{
//	SteeringOutput steering{};
//
//	//Mijn oplossing
//	//steering = Seek::CalculateSteering(deltaT, pAgent);
//	//steering.LinearVelocity += m_Target.LinearVelocity;
//
//	//Tachtigk zijn oplossing
//	m_Target.Position += m_Target.LinearVelocity;
//	return steering = Seek::CalculateSteering(deltaT, pAgent);
//	
//	
//}

SteeringPlugin_Output Evade::CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion)
{
	SteeringPlugin_Output steering{};
	m_Target.Position += m_Target.LinearVelocity;
	return  Flee::CalculateSteering(deltaT, pInterface, pEmotion);

}

