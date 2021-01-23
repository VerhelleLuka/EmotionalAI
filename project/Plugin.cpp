#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "SteeringBehaviors.h"
#include "Behaviors.h"
//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "BotNameTEST";
	info.Student_FirstName = "Luka";
	info.Student_LastName = "Verhelle";
	info.Student_Class = "2DAE08";

	m_pAgentBehavior = new AgentBehaviors();
	m_pSteering =  new SteeringPlugin_Output();
	m_pAgentBehavior->SetTarget({ 100,100 });
	m_pBlackboard = CreateBlackboard();
	m_pBlackboard->AddData("Time", 0.f);
	IDecisionMaking* pDecisionMaking =  new BehaviorTree(m_pBlackboard,
		new BehaviorSelector(
			{	
				new BehaviorSequence(
					{
						new BehaviorSelector(//Manage inventory selector
						{
							new BehaviorSequence(
							{
								new BehaviorConditional(HasAndShouldMedkit),
								new BehaviorAction(UseMedkit)
							}),
							new BehaviorSequence(
							{
								new BehaviorConditional(HasAndShouldFood),
								new BehaviorAction(UseFood)
							}),
						}),
						
							
					}),
				new BehaviorSequence(
					{
						new BehaviorConditional(IsInPurgeZone),
						new BehaviorAction(ChangeToSeek)
					}),
				new BehaviorSequence(
					{
						new BehaviorConditional(EnemyInSight),
						new BehaviorSelector(
							{
								new BehaviorSequence(
									{
										new BehaviorConditional(CanHitEnemy),
										new BehaviorAction(ShootEnemy)
									}),
								new BehaviorAction(FaceEnemy)
							}),
					}),
				new BehaviorSequence(
				{
					new BehaviorConditional(IsBit),
					new BehaviorConditional(EnoughBullets),
					new BehaviorAction(Turn)
				}),
				//new BehaviorSequence(
				//	{
				//		new BehaviorConditional(NeedAnything),
				//		new BehaviorConditional(GetClosestNeed),
				//		new BehaviorAction(ChangeToSeek)
				//	}),
				new BehaviorSequence(
				{
					new BehaviorConditional(IsInHouse),
					new BehaviorSelector(
					{
						new BehaviorSequence(
						{
							new BehaviorConditional(ShouldLeaveHouse),
							new BehaviorAction(LeaveHouse)
						}),
						new BehaviorAction(SeekItems),
					}),
				}),

				new BehaviorSequence(
				{
					new BehaviorConditional(NewLevel),
					new BehaviorAction(ResetVisitedHouses)
				}),
				new BehaviorSequence(
				{
					new BehaviorConditional(HouseInFOV),
					//new BehaviorConditional(HasBeenInHouse),
					new BehaviorAction(SeekHouse)
				}),
				//new BehaviorSequence(
				//{
				//	new BehaviorConditional(HasNoGun),
				//	new BehaviorAction(Evade)
				//}),
				new BehaviorSequence(
				{
					new BehaviorConditional(IsBit),
					new BehaviorAction(Sprint),
				}),
				new BehaviorSequence(
				{
					new BehaviorConditional(OutOfBounds),
					new BehaviorAction(ReturnToField)
				}),
				new BehaviorAction(ChangeToWander)
			})
	);
	m_pAgentBehavior->SetDecisionMaking(pDecisionMaking);
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded
	SAFE_DELETE(m_pAgentBehavior);
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be usefull to inspect certain behaviours (Default = false)
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.SpawnDebugPistol = true;
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	//std::cout << "EMOTION\n";
	m_pBlackboard->ChangeData("Time", dt);

	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	m_AgentInfo = m_pInterface->Agent_GetInfo();
	auto nextTargetPos = m_Target; //To start you can use the mouse position as guidance

	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	m_HouseInfo = vHousesInFOV;
	auto vEntitiesInFOV = GetEntitiesInFOV(); //uses m_pInterface->Fov_GetEntityByIndex(...)
	m_EntitiesInFOV = vEntitiesInFOV;
	

	m_pAgentBehavior->GetDecisionMaking()->Update(dt);
	Emotion* pEmotion = nullptr;
	bool dataAvailable = m_pBlackboard->GetData("emotion", pEmotion);
	if (dataAvailable)
	{
		*m_pSteering = m_pAgentBehavior->GetCurrentSteering()->CalculateSteering(dt, m_pInterface, pEmotion);
	}

	
	m_pSteering->RunMode = m_AgentInfo.RunMode; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	if (m_EmotionTimer < pEmotion->memory)//If the memory timer is not done counting, keep counting
	{
		m_EmotionTimer += dt;
	}
	else
	{
		
		if (pEmotion->boredomFactors.size() == 0)
		{
			std::cout << "I am amused\n";
		}
		else if (pEmotion->boredomFactors.size() >= 1 && pEmotion->boredomFactors.size() <= 3)
		{
			std::cout << "I am a little bored\n";
		}
		else
		{
			std::cout << "I am soooo bored\n";
		}
		if (pEmotion->fearFactors.size() == 0)
		{
			std::cout << "I am not scared\n";
		}
		else if (pEmotion->fearFactors.size() >= 1 && pEmotion->fearFactors.size() <= 3)
		{
			std::cout << "I am a little scared\n";
		}
		else if(pEmotion->fearFactors.size() > 3  && pEmotion->fearFactors.size() <= 5)
		{
			std::cout << "I am scared\n";
		}
		else
		{
			std::cout << "I am petrified\n";
		}
		if (pEmotion->angerPoints <= 0.1f)
		{
			std::cout << "I am not angry\n";
		}
		else if (pEmotion->angerPoints >= .1f && pEmotion->angerPoints <= 1.f)
		{
			std::cout << "I am a little angry\n";
		}
		else if (pEmotion->angerPoints > 1.f && pEmotion->angerPoints <= 2.5f)
		{
			std::cout << "I am angry\n";
		}
		else
		{
			std::cout << "I am RAGING\n";
		}

		m_EmotionTimer = 0.f;//Make it so the timer can start counting again
		if (pEmotion->fearFactors.size() > 0)//if the agent isn't already free of any fear
		{
			pEmotion->fearFactors.pop_back();//remove some fear
		}
		if (pEmotion->boredomFactors.size() > 0)//if the agent isn't already free of any boredom
		{
			pEmotion->boredomFactors.pop_back();//remove some boredom
		}
		if (pEmotion->angerPoints > 0.f) //if the agent has anger in him
		{
			pEmotion->angerPoints -= (1.0 - pEmotion->calmHasty);//decrease his anger
			if (pEmotion->calmHasty >= 0.99f)//if his calmHasty personality is higher than 0.99, meaning he get's extemely angry really quick
			{
				pEmotion->angerPoints -= 0.1f;//Only remove a little bit of anger
			}
		}
		
	}
	if (pEmotion->angerPoints > 5.f)
	{
		pEmotion->angerPoints = 5.f;
	}
	if (pEmotion->moodPoints > 1.f)
	{
		pEmotion->moodPoints = 1.f;
	}
	else if (pEmotion->moodPoints < 0.f)
	{
		pEmotion->moodPoints = 0.f;
	}
	//std::cout << pEmotion->angerPoints<<"ANGER \n";
	//std::cout << pEmotion->fearFactors.size() << "FEAR \n";
	//std::cout << pEmotion->boredomFactors.size() << "BOREDOM\n";
	return *m_pSteering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}

vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}

Blackboard* Plugin::CreateBlackboard()
{
	Elite::Blackboard* pBlackboard = new Elite::Blackboard();
	pBlackboard->AddData("pAgent", (&m_AgentInfo));
	pBlackboard->AddData("Info", m_pInterface);
	pBlackboard->AddData("AgentBehavior", m_pAgentBehavior);
	pBlackboard->AddData("pEntities", (&m_EntitiesInFOV));
	pBlackboard->AddData("Target", Elite::Vector2{0.f,0.f});
	pBlackboard->AddData("pHouses", (&m_HouseInfo));
	pBlackboard->AddData("pAgentEx", (&m_AgentInfoEx));
	pBlackboard->AddData("enemyTarget", Elite::Vector2{});
	pBlackboard->AddData("pSteering", m_pSteering);
	pBlackboard->AddData("pItems", m_pItems);
	pBlackboard->AddData("house", HouseInfo{});
	pBlackboard->AddData("emotion", (&m_EmotionInfo));
	return pBlackboard;
}
