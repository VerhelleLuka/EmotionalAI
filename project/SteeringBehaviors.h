/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "SteeringHelpers.h"
#include "IExamInterface.h"
#include "Exam_HelperStructs.h"
using namespace Elite;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringPlugin_Output CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion) = 0;

	//Seek Functions
	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{ return static_cast<T*>(this); }

protected:
	TargetData m_Target;
};
#pragma endregion

///////////////////////////////////////
//SEEK
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringPlugin_Output CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion) override;
};


//////////////////////////
//WANDER
//******
class Wander : public Seek
{
public:
	Wander() = default;
	virtual ~Wander() = default;

	//Wander Behavior
	SteeringPlugin_Output CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion) override;

	void SetWanderOffset(float offset) { m_Offset = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_AngleChange = rad; }
protected:
	float m_Offset = 12.f; //Offset (agent direction) (distnce to circle center)
	float m_Radius = 4.f;//Wander radius
	float m_AngleChange = ToRadians(45);//Max WanderAngle change per frame
	float m_WanderAngle = 0.f; //Internal

private:
	//void SetTarget(const TargetData& pTarget) override {} //Hide SetTarget, no target
};

class Flee : public Seek
{
public:
	Flee() = default;
	virtual ~Flee() = default;

	SteeringPlugin_Output CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion) override;

private:
	float m_FleeRadius = 40.f;
};
//
//class Arrive : public Seek
//{
//public:
//	Arrive() = default;
//	virtual ~Arrive() = default;
//
//	SteeringOutput CalculateSteering(float deltaT, IExamInterface* pAgent) override;
//};
//
class Face : public Seek
{
public:
	Face() = default;
	virtual ~Face() = default;

	SteeringPlugin_Output CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion) override;
};
class TurnAround : public Wander
{
public:
	TurnAround() = default;
	virtual ~TurnAround() = default;

	SteeringPlugin_Output CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion) override;
protected:
	float m_Offset = 12.f; //Offset (agent direction) (distnce to circle center)
	float m_Radius = 4.f;//Wander radius
	float m_AngleChange = ToRadians(45);//Max WanderAngle change per frame
	float m_WanderAngle = 0.f; //Internal
};
//
//
//class Pursuit : public Seek
//{
//public:
//	Pursuit() = default;
//	virtual ~Pursuit() = default;
//
//	SteeringOutput CalculateSteering(float deltaT, IExamInterface* pAgent) override;
//};
class Evade: public Flee
{
public:
	Evade() = default;
	virtual ~Evade() = default;

	SteeringPlugin_Output CalculateSteering(float deltaT, IExamInterface* pInterface, Emotion* pEmotion) override;
protected:
	const float m_Radius{ 1.f };
};
#endif


