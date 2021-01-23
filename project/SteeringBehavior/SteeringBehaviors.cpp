//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "../stdafx.h"

//Includes
#include "SteeringBehaviors.h"

//SEEK
//****
//SteeringOutput Seek::CalculateSteering(float deltaT, IExamInterface* pAgent)
//{
//	SteeringOutput steering = {};
//	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition(); //Desired velocity
//	steering.LinearVelocity.Normalize();
//	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
//
//
//
//	return steering;
//}

//WANDER (base> SEEK)
//******
SteeringOutput Wander::CalculateSteering(float deltaT, IExamInterface* pAgent)
{
	pAgent->SetAutoOrient(true);
	//int randNr{ Elite::randomInt(3) - 1 };
	//if (randNr != 0)
	//{
	//	m_WanderAngle += Elite::randomFloat(float(randNr) * m_AngleChange);
	//}
	//Tom en mijn oplossing
	//m_WanderAngle += ((float(rand() % 9000) / 100.f) - 45);
	//ENZO's OPLOSSING
	//
	//m_WanderAngle += ((rand() % int(m_AngleChange * 200)) - int(m_AngleChange * 100)) / 100.f;
	//MIJN OPLOSSING
	//m_WanderAngle += float(rand() % 90-45);
	//float playerAngle = pAgent->GetRotation();
	//m_Target.Position.x = cos(playerAngle - ToRadians(90)) * m_Offset + pAgent->GetPosition().x;
	//m_Target.Position.y = sin(playerAngle - ToRadians(90)) * m_Offset + pAgent->GetPosition().y;
	//m_Target.Position.x += cos(m_WanderAngle) * m_Radius;
	//m_Target.Position.y += sin(m_WanderAngle) * m_Radius;
	//ENZO's OPLOSSING
	Elite::Vector2 circlePos{ cos(pAgent->GetRotation() - ToRadians(90)) * m_Offset + pAgent->GetPosition().x,
	sin(pAgent->GetRotation() - ToRadians(90)) * m_Offset + pAgent->GetPosition().y };
	
	m_WanderAngle += ((rand() % int(m_AngleChange * 200)) - int(m_AngleChange * 100)) / 100.f;

	Elite::Vector2 targetPos{ (float)cos(m_WanderAngle) * m_Radius, (float)sin(m_WanderAngle) * m_Radius };
	targetPos += circlePos;

	m_Target = targetPos;

	//NIEMAND's OPLOSSING
	SteeringOutput steering = {Seek::CalculateSteering(deltaT, pAgent)};
	

	return steering;
}
//SteeringOutput Flee::CalculateSteering(float deltaT, IExamInterface* pAgent)
//{
//	float distanceToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
//	
//	if (distanceToTarget > m_FleeRadius)
//	{
//		SteeringOutput steering;
//		steering.IsValid = false;
//		return steering;
//	}
//	
//
//	SteeringOutput steering = Seek::CalculateSteering(deltaT, pAgent);
//	steering.LinearVelocity = steering.LinearVelocity * -1;
//	return steering;
//
//	return steering;
//
//}
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
//SteeringOutput Face::CalculateSteering(float deltaT, IExamInterface* pAgent)
//{
//	SteeringOutput steering{};
//	std::cout << ToDegrees(pAgent->GetRotation()) << "\n";
//	pAgent->GetRotation();
//	pAgent->SetAutoOrient(false);
//	Elite::Vector2 directionVec{ m_Target.Position - pAgent->GetPosition() };
//	directionVec = directionVec.GetNormalized();
//
//	float angle = ToDegrees( atan2(directionVec.y, directionVec.x));//Hiermee vindt ge de hoek van uw directionVec (en zet naar graden om)
//	float playerAngle = ToDegrees(pAgent->GetRotation()) -90;//Haal -90 wweg en ge ziet wat er gebeurt
//	float angleDifference = angle - playerAngle;
//	steering.AngularVelocity =Clamp( angleDifference, -pAgent->GetMaxAngularSpeed(), pAgent->GetMaxAngularSpeed());
//
//
//	
//
//	//pAgent->SetAngularVelocity(5);//5 graden per seconden
//	
//
//	return steering;
//
//}
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

//SteeringOutput Evade::CalculateSteering(float deltaT, IExamInterface* pAgent)
//{
//	SteeringOutput steering{};
//	m_Target.Position += m_Target.LinearVelocity;
//	return  Flee::CalculateSteering(deltaT, pAgent);
//
//}

