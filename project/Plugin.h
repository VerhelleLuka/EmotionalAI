#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "EBlackboard.h"
#include "EBehaviorTree.h"
#include "AgentBehaviors.h"
class ISteeringBehavior;
class IBaseInterface;
class IExamInterface;

class Plugin :public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	vector<HouseInfo> GetHousesInFOV() const;
	vector<EntityInfo> GetEntitiesInFOV() const;

	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose
	//Blackboard
	Elite::Blackboard* CreateBlackboard();
	Elite::Blackboard* m_pBlackboard = nullptr;


	AgentBehaviors* m_pAgentBehavior = nullptr;
	std::vector<HouseInfo> m_HouseInfo;
	std::vector<EntityInfo> m_EntitiesInFOV;

	AgentInfo m_AgentInfo;
	AgentInfo_Extended m_AgentInfoEx;
	Emotion m_EmotionInfo;

	SteeringPlugin_Output* m_pSteering = nullptr;
	std::vector<EntityInfo>* m_pItems = nullptr;
	float m_EmotionTimer{};
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}