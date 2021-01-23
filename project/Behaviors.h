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
//#include "framework/EliteMath/EMath.h"
#include "EBehaviorTree.h"

#include "SteeringBehaviors.h"


bool EnoughBullets(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentBehaviors* pAgentBehavior = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior);
	if (!dataAvailable)
	{
		return false;
	}
	ItemInfo iI{};
	for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
	{
		//std::cout << "Enough Bullets\n";
		pInterface->Inventory_GetItem(i, iI);
		if (iI.Type == eItemType::PISTOL)//Check if you have a gun with more than 2 bullets, otherwise it might be not worth turning around
		{
			if (pInterface->Weapon_GetAmmo(iI) > 3)
			{
				//pAgentBehavior->SetToTurnAround(); // 20/01 15:19
				return true;
			}
		}

	}
	//pAgentBehavior->SetToWander();
	return false;
}
BehaviorState ChangeToWander(Elite::Blackboard* pBlackboard)
{

	AgentBehaviors* pAgentBehavior = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	Emotion* pEmotion = nullptr;
	float time{};
	auto dataAvailable = pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("emotion", pEmotion) && pBlackboard->GetData("Time", time)&& pBlackboard->GetData("pAgent", pAgentInfo);
	if (!dataAvailable)
	{
		return Failure;
	}	
	pEmotion->boredRemoved = false;
	if (pEmotion->boredomTime < (pEmotion->inattentiveDedicated + pEmotion->boredomTimer)) //Check how long the agent has been wandering
	{
		pEmotion->boredomTime += time;
	}
	else //If he has been wandering for more than the calculated time,
	{
		pEmotion->boredomTime = 0.f;
		pEmotion->boredomFactors.push_back(1); //he becomes bored
		pEmotion->memory -= 2.f;//and his memory decreases
		if (pEmotion->moodPoints < pEmotion->inattentiveDedicated * .1f)//if the agent is feelign worse than  bored, he gets even more bored
		{
			pEmotion->boredomFactors.push_back(1);
		}
		pEmotion->moodPoints += 0.1f;//Being bored is not fun
	}
	if (pEmotion->boredomFactors.size() > 1)
	{
		if (pEmotion->boredomFactors.size() >= 10)
		{
			if (pAgentInfo->Stamina > 0.f)
			{
				pAgentInfo->RunMode = true;
			}
			else
				pAgentInfo->RunMode = false;
		}
		else
		{
			if (pAgentInfo->Stamina > 10.f - float(pEmotion->boredomFactors.size())) //if his stamina is bigger than 10 - how bored he is, start running
			{
				pAgentInfo->RunMode = true;
			}
			else
				pAgentInfo->RunMode = false;
		}

	}
	//std::cout << 10.f - float(pEmotion->boredomFactors.size()) << " stamina to run at\n";
	pAgentBehavior->SetToWander();
	pBlackboard->ChangeData("AgentBehavior", pAgentBehavior);
	if (pEmotion->angerPoints >=  2.5f&& pEmotion->angerPoints <= 3.f) //&& EnoughBullets(pBlackboard))//If his anger points are between 2.5 and 3 AND you have enough bullets, start turning around to look for enemies
	{
		pAgentBehavior->SetToTurnAround();
	}
	else if (pEmotion->angerPoints > 3.f)//however, if you anger is higher than 3, just start turning around, regardless if you have a gun or not
	{
		pAgentBehavior->SetToTurnAround();
	}
	return Success;
}


BehaviorState ChangeToSeek(Elite::Blackboard* pBlackboard)
{
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	AgentBehaviors* pAgentBehavior = nullptr;
	Emotion* pEmotion = nullptr;
	Elite::Vector2 target;
	pBlackboard->GetData("Target", target);
	auto dataAvailable = pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("emotion", pEmotion);
	if (!dataAvailable)
	{
		return Failure;
	}
	if (!pEmotion->boredRemoved)
	{
		pEmotion->boredRemoved = true;
		if(pEmotion->boredomFactors.size() > 0)
			pEmotion->boredomFactors.pop_back();
		pEmotion->moodPoints -= 0.1f;//not being bored is fun
	}


	pAgentBehavior->SetToSeek();
	pAgentBehavior->SetTarget(target);
	if (pEmotion->angerPoints >= 2.5f && pEmotion->angerPoints <= 3.f && EnoughBullets(pBlackboard))
	{
		pAgentBehavior->SetToTurnAround();
	}
	else if (pEmotion->angerPoints > 3.f)
	{
		pAgentBehavior->SetToTurnAround();
	}

	return Success;
}

BehaviorState SeekHouse(Elite::Blackboard* pBlackboard)
{
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	AgentBehaviors* pAgentBehavior = nullptr;
	HouseInfo target;
	Emotion* pEmotion = nullptr;
	pBlackboard->GetData("house", target);
	auto dataAvailable = pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("emotion", pEmotion);
	if (!dataAvailable)
	{
		return Failure;
	}
	if (!pEmotion->boredRemoved)
	{
		pEmotion->boredRemoved = true;

		pEmotion->moodPoints -= .1f;
	}
	if (pEmotion->angerPoints >= 2.5f && pEmotion->angerPoints <= 3.f && EnoughBullets(pBlackboard))
	{
		pAgentBehavior->SetToTurnAround();
	}
	else if (pEmotion->angerPoints > 3.f)
	{
		pAgentBehavior->SetToTurnAround();
	}
	pAgentInfoEx->seekingHouse = true;
	pAgentBehavior->SetToSeek();
	pAgentBehavior->SetTarget(target.Center);

	return Success;
}
BehaviorState SeekItems(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	std::vector<EntityInfo>* pEntities = nullptr;
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	std::vector<EntityInfo>* pItems = nullptr;
	Elite::Vector2 target;
	Emotion* pEmotion = nullptr;
	Elite::Vector2 houseTarget{0.f,0.f};
	auto dataAvailable = pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pEntities", pEntities) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("emotion", pEmotion) && pBlackboard->GetData("pItems", pItems);
	if (!dataAvailable)
	{
		return Failure;
	}
	ItemInfo ei = {};
	if (!pEmotion->boredRemoved)
	{
		pEmotion->boredRemoved = true;
		if (pEmotion->boredomFactors.size() > 0)
			pEmotion->boredomFactors.pop_back();
		pEmotion->moodPoints -= .1f;

	}

	//Eerst een loop over inventory en nutteloze items verwijderen
	for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)
	{
		//std::cout << "Seek items\n";
		if (pInterface->Inventory_GetItem(i, ei))
		{
			switch (ei.Type)
			{
			case eItemType::PISTOL:
				if (pInterface->Weapon_GetAmmo(ei) <= 0)
				{
					pInterface->Inventory_RemoveItem(i);
				}
				break;
			case eItemType::MEDKIT:
				if (pInterface->Medkit_GetHealth(ei) <= 0)
				{
					pInterface->Inventory_RemoveItem(i);
				}
				break;
			case eItemType::FOOD:
				if (pInterface->Food_GetEnergy(ei) <= 0)
				{
					pInterface->Inventory_RemoveItem(i);
				}
				break;
			case eItemType::GARBAGE:
				pInterface->Inventory_RemoveItem(i);
				break;
				
			default:
				continue;
		

			}
		}
	}

	for (auto& e : (*pEntities))
	{
		if (e.Type == eEntityType::ITEM)
		{
			
			if (Distance(pInterface->Agent_GetInfo().Position,e.Location ) <= pInterface->Agent_GetInfo().GrabRange)
			{	
				//int freeSlot{ 5 };
				//ItemInfo checkIfThere;
				//for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
				//{
				//	if (!pInterface->Inventory_GetItem(i, checkIfThere))
				//	{
				//		freeSlot = i;
				//		break;
				//	}
				//}
				pInterface->Item_GetInfo(e, ei);
				
				if (pInterface->Item_Grab(e, ei))
				{
					//auto it = std::find_if(pAgentInfoEx->leftItems.begin(), pAgentInfoEx->leftItems.end(), [&ei](const ItemInfo& itemInfo)//If grabbed item was once left behind, erase from the vector
					//	{
					//		return (itemInfo.Location.x == ei.Location.x && itemInfo.Location.y == ei.Location.y);
					//	});
					//if (it != pAgentInfoEx->leftItems.end())
					//{
					//	pAgentInfoEx->leftItems.erase(std::remove(it));
					//}
				}
				for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
				{
					
					if (pInterface->Inventory_AddItem(i, ei))
					{
						pAgentInfoEx->goingForItem = false;
						if (ei.Type == eItemType::GARBAGE)
						{
							pInterface->Inventory_RemoveItem(i);
						}
						//for (auto item : pAgentInfoEx->leftItems)//If the picked up item was in the leftitems vector, remove it from the vector
						//{
						//	auto it = std::find(pAgentInfoEx->leftItems.begin(), pAgentInfoEx->leftItems.end(), [ei](const ItemInfo& itemInfo )
						//		{
						//			return ei.ItemHash == itemInfo.ItemHash;
						//		});
						//	if (it != pAgentInfoEx->leftItems.end())
						//	{
						//		pAgentInfoEx->leftItems.erase(std::remove(pAgentInfoEx->leftItems.begin(), pAgentInfoEx->leftItems.end(), (*it)));
						//	}
						//}
						break;
					}	

					//PAS OP
					else//If inventory is full and you need health or food, remove whatever necessary
					{
						//Check of inventory vol zit
						int freeSlots{5};
						ItemInfo checkIfThere;
						for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
						{
							//std::cout << "Seek items 2\n";
							if (pInterface->Inventory_GetItem(i, checkIfThere))
							{
								freeSlots--;
							}
						}
						if (freeSlots == 0)
						{
							if (pInterface->Agent_GetInfo().Health < 6.f && ei.Type == eItemType::MEDKIT)
							{
								std::vector<std::pair<ItemInfo, UINT>> gunsInInventory = {};
								std::vector<std::pair<ItemInfo, UINT>> foodInInventory = {};
								std::vector<ItemInfo> medkitInInventory = {};
								int gunCount{};
								int foodCount{};
								int medkitCount{};
								for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)//Loop over all items in inventory and throw them in a vector
								{
									//std::cout << "Seek items 3\n";
									pInterface->Inventory_GetItem(i, ei);
									if (ei.Type == eItemType::PISTOL)
									{
										gunsInInventory.push_back(std::pair<ItemInfo, UINT>(ei, i));
										gunCount++;
									}
									else if (ei.Type == eItemType::FOOD)
									{
										foodInInventory.push_back(std::pair<ItemInfo, UINT>(ei, i));
										foodCount++;
									}
									else if (ei.Type == eItemType::GARBAGE)
									{
										pInterface->Inventory_RemoveItem(i);
									}
								}
								if (gunCount > foodCount && pEmotion->angerPoints < 2.5f)//If you have more guns than food, throw out the gun with the least ammo otherwise use and throw out food with least energy
								{
									auto it = std::min_element(gunsInInventory.begin(), gunsInInventory.end(), [pInterface](std::pair<ItemInfo, UINT>& itemInfoa, std::pair<ItemInfo, UINT>& itemInfob)
										{
											return pInterface->Weapon_GetAmmo(itemInfoa.first) < pInterface->Weapon_GetAmmo(itemInfob.first);
										});
									if (it != gunsInInventory.end())
									{
										pInterface->Inventory_RemoveItem((*it).second);

										if (pInterface->Inventory_AddItem((*it).second, ei))
										{
											if (ei.Type == eItemType::GARBAGE)
											{
												pInterface->Inventory_RemoveItem((*it).second);
											}
											pAgentInfoEx->goingForItem = false;
											break;
										}
									}

								}
								else
								{
									auto it = std::min_element(foodInInventory.begin(), foodInInventory.end(), [pInterface](std::pair<ItemInfo, UINT>& itemInfoa, std::pair<ItemInfo, UINT>& itemInfob)
										{
											return pInterface->Weapon_GetAmmo(itemInfoa.first) < pInterface->Weapon_GetAmmo(itemInfob.first);
										});
									if (it != foodInInventory.end())
									{
										pInterface->Inventory_UseItem((*it).second);
										pInterface->Inventory_RemoveItem((*it).second);

										if (pInterface->Inventory_AddItem((*it).second, ei))
										{
											if (ei.Type == eItemType::GARBAGE)
											{
												pInterface->Inventory_RemoveItem((*it).second);
											}
											pAgentInfoEx->goingForItem = false;
											break;
										}
									}
								}
							}
							else if (pInterface->Agent_GetInfo().Energy < 6.f && ei.Type == eItemType::FOOD)
							{
								std::vector<std::pair<ItemInfo, UINT>> gunsInInventory = {};
								std::vector<std::pair<ItemInfo, UINT>> foodInInventory = {};
								std::vector<std::pair<ItemInfo, UINT>> medkitInInventory = {};
								int gunCount{};
								int foodCount{};
								int medkitCount{};
								for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)//Loop over all items that and throw them in a vector otherwise
								{
									//std::cout << "Seek items 4\n";
									pInterface->Inventory_GetItem(i, ei);
									if (ei.Type == eItemType::PISTOL)
									{
										gunsInInventory.push_back(std::pair<ItemInfo, UINT>(ei, i));
										gunCount++;
									}
									else if (ei.Type == eItemType::MEDKIT)
									{
										medkitInInventory.push_back(std::pair<ItemInfo, UINT>(ei, i));
										medkitCount++;
									}
									else if (ei.Type == eItemType::GARBAGE)
									{
										pInterface->Inventory_RemoveItem(i);
									}
								}
								if (gunCount > medkitCount && pEmotion->angerPoints < 2.5f)//If you have more guns than medkits, throw out the gun with the least ammo, otherwise use and throw out medkit with least health
								{
									auto it = std::min_element(gunsInInventory.begin(), gunsInInventory.end(), [pInterface](std::pair<ItemInfo, UINT>& itemInfoa, std::pair<ItemInfo, UINT>& itemInfob)
										{
											return pInterface->Weapon_GetAmmo(itemInfoa.first) < pInterface->Weapon_GetAmmo(itemInfob.first);
										});
									if (it != gunsInInventory.end())
									{

										pInterface->Inventory_RemoveItem((*it).second);

										if (pInterface->Inventory_AddItem((*it).second, ei))
										{
											if (ei.Type == eItemType::GARBAGE)
											{
												pInterface->Inventory_RemoveItem((*it).second);
											}
											pAgentInfoEx->goingForItem = false;
											break;
										}
									}

								}
								else
								{
									auto it = std::min_element(medkitInInventory.begin(), medkitInInventory.end(), [pInterface](std::pair<ItemInfo, UINT>& itemInfoa, std::pair<ItemInfo, UINT>& itemInfob)
										{
											return pInterface->Weapon_GetAmmo(itemInfoa.first) < pInterface->Weapon_GetAmmo(itemInfob.first);
										});
									if (it != medkitInInventory.end())
									{
										pInterface->Inventory_UseItem((*it).second);
										pInterface->Inventory_RemoveItem((*it).second);

										if (pInterface->Inventory_AddItem((*it).second, ei))
										{
											if (ei.Type == eItemType::GARBAGE)
											{
												pInterface->Inventory_RemoveItem((*it).second);
											}
											pAgentInfoEx->goingForItem = false;
											break;
										}
									}
								}
							}
							else//if you don't have any free slots AND don't need anything right now, save it in your items vector.
							{
								pAgentInfoEx->leftItems.push_back((&ei));
								break;
							}
						}

					}
					//PAS OP
				}
			}
			else
			{
			pAgentInfoEx->goingForItem = true;
				pBlackboard->ChangeData("Target", e.Location);
				pAgentBehavior->SetToSeek();
				pAgentBehavior->SetTarget(e.Location);
				return Success;
			}
		}
	}
	if (pAgentInfoEx->goingForItem)
	{
		if (pEmotion->angerPoints >= 2.5f && pEmotion->angerPoints <= 3.f && EnoughBullets(pBlackboard))
		{
			pAgentBehavior->SetToTurnAround();
		}
		else if (pEmotion->angerPoints > 3.f)
		{
			pAgentBehavior->SetToTurnAround();
		}
		return Success;
	}
	pAgentBehavior->SetToTurnAround();
	return Success;
}

BehaviorState LeaveHouse(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	IExamInterface* pInterface = nullptr;
	std::vector<HouseInfo>* houseInFOV = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	Elite::Vector2 target;
	Emotion* pEmotion = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("pHouses", houseInFOV)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&&pBlackboard->GetData("emotion", pEmotion);
	if (!dataAvailable)
	{
		return Failure;
	}
	//
	//if ( !pInterface->Agent_GetInfo().IsInHouse && (*houseInFOV).size() <= 0)
	//{
	//	return Failure;
	//}
	//else if ((*houseInFOV).size() > 0 && pInterface->Agent_GetInfo().IsInHouse)
	//{

	//	target.x = 0.f;
	//	target.y = 0.f;
	//	pBlackboard->ChangeData("HouseTarget", target);
	//	pAgentBehavior->SetToSeek();
	//	pAgentBehavior->SetTarget(target);
	//	return Success;

	//}
	//return Failure;
	if ( pInterface->Agent_GetInfo().IsInHouse)
	{
		target.x = 0.f;
		target.y = 0.f;
		pAgentBehavior->SetToSeek();
		pAgentBehavior->SetTarget(target);
		pAgentInfoEx->seekingHouse = false;

		return Success;
	}
	return Failure;

}

BehaviorState FaceEnemy(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	IExamInterface* pInterface = nullptr;
	std::vector<EntityInfo>* pEntities = nullptr;
	Elite::Vector2 target;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pEntities", pEntities)
		&& pBlackboard->GetData("enemyTarget", target);
	if(!dataAvailable)
	{
		return Failure;
	}
	//std::vector<EntityInfo> pOnlyEnemies;
	//for (const auto& e : (*pEntities))//get only the enemies from your field of view
	//{
	//	if (e.Type == eEntityType::ENEMY)
	//	{
	//		pOnlyEnemies.push_back(e);
	//	}
	//}
	//if (pOnlyEnemies.size() > 1)
	//{

	//	auto it = std::min_element(pOnlyEnemies.begin(), pOnlyEnemies.end(), [pInterface](EntityInfo& enemyA, EntityInfo& enemyB)
	//		{
	//			return Distance(pInterface->Agent_GetInfo().Position, enemyA.Location) < Distance(pInterface->Agent_GetInfo().Position, enemyB.Location);
	//		});
	//	if (it != pOnlyEnemies.end())
	//	{
	//		target = (*it).Location;
	//		pAgentBehavior->SetToFace();
	//		pAgentBehavior->SetTarget(target);
	//		pBlackboard->ChangeData("enemyTarget", target);
	//		return Success;

	//	}
	//}
	//else if(pOnlyEnemies.size() == 1)
	//{
	//	pAgentBehavior->SetToFace();
	//	pAgentBehavior->SetTarget(target);
	//	//pBlackboard->ChangeData("Target", target);
	//	return Success;
	//}
	pAgentBehavior->SetToFace();
	pAgentBehavior->SetTarget(target);
	return Success;
}

BehaviorState ShootEnemy(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	IExamInterface* pInterface = nullptr;
	Elite::Vector2 target{0.f,0.f};
	std::vector<EntityInfo>* pEntities = nullptr;
	Emotion* pEmotion = nullptr;
	auto dataAvailable = pBlackboard->GetData("pAgentEx", pAgentInfoEx) && pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pEntities", pEntities)
		&& pBlackboard->GetData("emotion", pEmotion);
//	pBlackboard->ChangeData("HouseTarget", target);
	if (!dataAvailable)
	{
		return Failure;
	}
	ItemInfo iI;
	for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)
	{
		//std::cout << "ShootEnemy\n";
		pInterface->Inventory_GetItem(i, iI);
		if (iI.Type == eItemType::PISTOL)
		{
			if (pInterface->Weapon_GetAmmo(iI) > 0)
			{
				pInterface->Inventory_UseItem(i);
				if(pEmotion->fearFactors.size() > 0)
					pEmotion->fearFactors.pop_back();
				pEmotion->angerPoints -= pEmotion->calmHasty / 3.f;
				if (pEmotion->moodPoints < pEmotion->calmHasty)//If the character is feeling better than it gets angry, reduce the points by a little more
				{
					pEmotion->angerPoints -= pEmotion->calmHasty / 5.f;
				}

				pEmotion->moodPoints -= .1f;//Shooting is fun

				//pAgentBehavior->SetTarget(target);
				//if (pEntities->size() > 0)
				//{
				//	//pAgentBehavior->SetToWander();
				//	//pAgentInfoEx->hunting = false;
				//}
				if (pEmotion->angerPoints >= 2.5f && pEmotion->angerPoints <= 3.f && EnoughBullets(pBlackboard))
				{
					pAgentBehavior->SetToTurnAround();
				}
				else if (pEmotion->angerPoints > 3.f)
				{
					pAgentBehavior->SetToTurnAround();
				}
				return Success;

			}
			else
			{

				pInterface->Inventory_RemoveItem(i);
			}
		}
	}

	if (pEntities->size() == 0)
	{
		pAgentInfoEx->hunting = false;
		pBlackboard->ChangeData("enemyTarget", target);
	//	pAgentBehavior->SetToWander();
		pAgentBehavior->SetTarget(target);
		return Success;
	}

	return Failure;
}
BehaviorState UseMedkit(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	Emotion* pEmotion = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("emotion", pEmotion);

	if (!dataAvailable)
	{
		return Failure;
	}
	ItemInfo iI{};
	for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)
	{
		//std::cout << "Use medkit\n";
		pInterface->Inventory_GetItem(i, iI);
		if (iI.Type == eItemType::MEDKIT)
		{
				pInterface->Inventory_UseItem(i);
				pEmotion->moodPoints -= .1f;

				if (pInterface->Medkit_GetHealth(iI) == 0)
				{
					pInterface->Inventory_RemoveItem(i);
					continue;
				}
				pAgentBehavior->SetToWander();
				return Success;
		}
	}
	return Failure;

}
BehaviorState UseFood(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	Emotion* pEmotion = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("emotion", pEmotion);

	if (!dataAvailable)
	{
		return Failure;
	}
	float agentEnergyNeeded{ 10.f - pInterface->Agent_GetInfo().Energy };
	//The amound of energy the agent needs
	
	ItemInfo iI{};
	for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)
	{
		//std::cout << "Use food\n";
		pInterface->Inventory_GetItem(i, iI);
		if (iI.Type == eItemType::FOOD)
		{
			//std::cout << pInterface->Food_GetEnergy(iI) << "FOOD ENERGY\n";
			if (pInterface->Food_GetEnergy(iI) <= agentEnergyNeeded)
			{

				pInterface->Inventory_UseItem(i);
				pEmotion->moodPoints -= .1f;

				if (pInterface->Food_GetEnergy(iI) == 0)
				{
					pInterface->Inventory_RemoveItem(i);
					continue;
				}
				pAgentBehavior->SetToWander();
				if (pEmotion->angerPoints >= 2.5f && pEmotion->angerPoints <= 3.f && EnoughBullets(pBlackboard))
				{
					pAgentBehavior->SetToTurnAround();
				}
				else if (pEmotion->angerPoints > 3.f)
				{
					pAgentBehavior->SetToTurnAround();
				}
				return Success;

			}
		}
	}
	return Failure;
}


BehaviorState Sprint(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("pAgent", pAgentInfo);
	if (!dataAvailable)
	{
		return Failure;
	}
	if (pInterface->Agent_GetInfo().Stamina > 0)
	{
		//std::cout << "SPRIIIIIIIIIIIIIIIIIINT\n";
		pAgentInfo->RunMode = true;
		pBlackboard->ChangeData("pAgent", pAgentInfo);
		return Success;
	}
	return Failure;
}

BehaviorState ResetVisitedHouses(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx);
	if (!dataAvailable)
	{
		return Failure;
	}
	std::cout << "RESET VISITED HOUSES!\n";
	pAgentInfoEx->visitedHouseCenter.clear();
	pAgentInfoEx->canVisitAgain = 0.f;
	return Success;
}

BehaviorState ReturnToField(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	IExamInterface* pInterface = nullptr;
	Elite::Vector2 target{ 0.f,0.f };
	auto dataAvailable = pBlackboard->GetData("pAgentEx", pAgentInfoEx) && pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior);
	if (!dataAvailable)
	{
		return Failure;
	}
	pAgentBehavior->SetToSeek();
	pAgentBehavior->SetTarget(target);
	return Success;
}
BehaviorState Turn(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	IExamInterface* pInterface = nullptr;
	Elite::Vector2 target{ 0.f,0.f };
	auto dataAvailable = pBlackboard->GetData("pAgentEx", pAgentInfoEx) && pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior);
	if (!dataAvailable)
	{
		return Failure;
	}
	pAgentInfoEx->turning = true;
	pAgentBehavior->SetToTurnAround();
	return Success;

}


BehaviorState Evade(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	auto dataAvailable = pBlackboard->GetData("AgentBehavior", pAgentBehavior);
	if (!dataAvailable)
	{
		return Failure;
	}
	pAgentBehavior->SetToEvade();
	return Success;
}
//BEHAVIORCONDITIONAL
//***************


bool IsInHouse(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	std::vector<HouseInfo>* houseInFOV;
	AgentInfo_Extended* pAgentInfoEx = nullptr;


	auto dataAvailable = pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pHouses", houseInFOV) && pBlackboard->GetData("pAgentEx", pAgentInfoEx);

	if (!dataAvailable || pAgentInfo == nullptr)
	{
		return false;
	}
	//Elite::Vector2 houseTarget{ 0,0 };
	//if (pInterface->Agent_GetInfo().IsInHouse)
	//{
	//	HouseInfo hi = {};
	//	for (int i = 0;; ++i)
	//	{
	//		if (pInterface->Fov_GetHouseByIndex(i, hi))
	//		{
	//			pAgentInfoEx->visitedHouseCenter.push_back(hi.Center);
	//			break;
	//		}
	//	}
	//	
	//	pBlackboard->ChangeData("Target", houseTarget);
	//	return true;
	//}
	//return false;



	if (pInterface->Agent_GetInfo().IsInHouse)
	{
		//pAgentInfoEx->seekingHouse = false;

		Elite::Vector2 emptyTarget{};
		pBlackboard->ChangeData("Target", emptyTarget);
		HouseInfo hi = {};
		for (int i = 0;; ++i)
		{
			if (pInterface->Fov_GetHouseByIndex(i, hi))
			{
				bool alreadyInList = false;
				for(auto& e: pAgentInfoEx->visitedHouseCenter)
				{
					if (e == hi.Center)
					{
						alreadyInList = true;
					}
				}
				if (!alreadyInList)
				{
					pAgentInfoEx->visitedHouseCenter.push_back(hi.Center);
					(*pAgentInfoEx).timeInHouse = 0.0f;
				}
				break;
			}
		}
		return true;
	}
	return false;
}
bool SearchingHouseEntrance(Elite::Blackboard* pBlackboard)
{
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	Elite::Vector2 target{};
	Elite::Vector2 houseTarget{};
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	auto dataAvailable = pBlackboard->GetData("pAgentEx", pAgentInfoEx) && pBlackboard->GetData("Target", target)
		&& pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("HouseTarget", houseTarget) && pBlackboard->GetData("pAgent", pAgentInfo);
	//std::cout << houseTarget.x << " " << houseTarget.y << "\n";
	if (!dataAvailable)
	{
		return false;
	}	
	if ( (abs(houseTarget.x) > 1.f &&  abs(houseTarget.y) > 1.f))
	{
		return false;
	}
	return true;
}
bool HasBeenInHouse(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	std::vector<HouseInfo>* houseInFOV = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
	 && pBlackboard->GetData("pAgentEx", pAgentInfoEx) && pBlackboard->GetData("pHouses", houseInFOV);
	if (!dataAvailable)
	{
		return false;
	}
	//HouseInfo houseCenter;
	//for (size_t i = 0; i < (*houseInFOV).size(); i++)
	//{
	//	houseCenter.Center = (*houseInFOV)[i].Center;
	//	auto it = std::find_if(pAgentInfoEx->visitedHouseCenter.begin(), pAgentInfoEx->visitedHouseCenter.end(), [houseCenter](const Elite::Vector2& houseCenterV)
	//		{
	//			return houseCenter.Center == houseCenterV;
	//		});
	//	if (it != pAgentInfoEx->visitedHouseCenter.end())//zit erin --> Huis is bezocht
	//	{
	//		pBlackboard->GetData("house", houseCenter);
	//		if (houseCenter.Center == (*it))
	//		{
	//			return false;
	//		}
	//		
	//	}

	//}
	//return true;
}
bool HouseInFOV(Elite::Blackboard* pBlackboard)
{
	std::vector<HouseInfo>* houseInFOV = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	IExamInterface* pInterface = nullptr;
	Emotion* pEmotion = nullptr;
	auto dataAvailable = pBlackboard->GetData("pHouses", houseInFOV) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("emotion", pEmotion);
	if (!dataAvailable)
	{
		return false;
	}
	//if ((*houseInFOV).size() > 0 ) {
	//	if (abs(houseTarget.x) <= abs(target.x) || abs(houseTarget.y) <= abs(target.y))
	//	{
	//		pBlackboard->ChangeData("Target", (*houseInFOV)[0].Center);
	//		pBlackboard->ChangeData("HouseTarget", (*houseInFOV)[0].Center);
	//	}
	//	if (pInterface->Agent_GetInfo().IsInHouse)
	//	{
	//		return false;
	//	}
	//}
	//else if ((*houseInFOV).size() <= 0 )// && pAgentInfoEx->searchingHouseEntrance)
	//{
	//	if ((abs(houseTarget.x) <= abs(target.x) || abs(houseTarget.y) = abs(target.y)))
	//	{
	//		target.x = 0;
	//		target.y = 0;
	//		//pBlackboard->ChangeData("HouseTarget", target);
	//		pBlackboard->ChangeData("Target", target);
	//	}
	//	//pBlackboard->ChangeData("HouseTarget", target);
	//	return false;
	//}
	//pBlackboard->GetData("HouseTarget", houseTarget);
	//if ((*houseInFOV).size() > 0)
	//{
	//	if (abs(houseTarget.x) <= abs(target.x) || abs(houseTarget.y) <= abs(target.y))
	//	{
	//		pBlackboard->ChangeData("Target", (*houseInFOV)[0].Center);
	//		pBlackboard->ChangeData("HouseTarget", (*houseInFOV)[0].Center);
	//	}
	//	if (pInterface->Agent_GetInfo().IsInHouse)
	//	{
	//		return false;
	//	}
	//}
	//else if (abs(houseTarget.x) <= abs(target.x) || abs(houseTarget.y) <= abs(target.y))
	//{
	//	//pBlackboard->ChangeData("HouseTarget", target);
	//	return false;
	//}
	//return true;
	//if (pAgentInfoEx->seekingHouse && (abs(houseTarget.x) > abs(target.x) || abs(houseTarget.y) > abs(target.y)))
	//{
	//	return true;
	//}
	//if ((*houseInFOV).size() > 0)
	//{
	//	for (size_t i = 0; i < (*houseInFOV).size(); i++)
	//	{
	//		if (abs(houseTarget.x) == abs((*houseInFOV)[i].Center.x) || abs(houseTarget.y) == abs((*houseInFOV)[i].Center.y))
	//		{
	//			pAgentInfoEx->seekingHouse = true;
	//			return true;
	//			
	//		}
	//	}
	//	if (abs(houseTarget.x) != abs((*houseInFOV)[0].Center.x) || abs(houseTarget.y) != abs((*houseInFOV)[0].Center.y))
	//	{
	//		pBlackboard->ChangeData("Target", (*houseInFOV)[0].Center);
	//		pAgentInfoEx->seekingHouse = true;

	//		return true;
	//	}
	//}

	//if (abs(houseTarget.x) > abs(target.x) || abs(houseTarget.y) > abs(target.y))
	//{
	//	pAgentInfoEx->seekingHouse = true;

	//	return true;
	//}

	//if ((*houseInFOV).size() > 0)
	//{
	//	//if (pInterface->Agent_GetInfo().IsInHouse)
	//	//{
	//	//	return false;
	//	//}
	//	pBlackboard->ChangeData("Target", (*houseInFOV)[0].Center);
	//	return true;
	//}
	//return false;

	if (pAgentInfo->Bitten)
	{
		pEmotion->angerPoints += pEmotion->calmHasty;
		if (pEmotion->moodPoints > pEmotion->calmHasty)
		{
			pEmotion->angerPoints += pEmotion->calmHasty * 0.5f;//if the character is feeling worse than it gets angry, add some more anger
		}
		if ((pEmotion->moodPoints*10.f) > pEmotion->timidBrave)//if the character is feeling worse than it gets scared, it gets more scared
		{
			pEmotion->fearFactors.push_back(1.f);
		}
		pEmotion->fearFactors.push_back(1.f);
		pEmotion->moodPoints += .2f; //Being bitten is not fun

		pAgentInfo->RunMode = true;
	}
	else if(pAgentInfo->Stamina < 3.f)
	{
		pAgentInfo->RunMode = false;
	}
	if (pAgentInfoEx->seekingHouse)
	{
		return true;
	}

	if (houseInFOV->size() > 0)
	{
		Elite::Vector2 houseCenter;
		for (size_t i = 0; i < (*houseInFOV).size(); i++)
		{
			houseCenter = (*houseInFOV)[i].Center;
			auto it = std::find_if(pAgentInfoEx->visitedHouseCenter.begin(), pAgentInfoEx->visitedHouseCenter.end(), [houseCenter](const Elite::Vector2& houseCenterV)
				{
					return houseCenter == houseCenterV;
				});
			if (it != pAgentInfoEx->visitedHouseCenter.end())//zit erin --> Huis is bezocht
			{
				continue;
			}
			pBlackboard->ChangeData("house", (*houseInFOV)[i]);
			return true;
		}
	}
	return false;
}
bool ShouldLeaveHouse(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	std::vector<EntityInfo>* pEntities = nullptr;
	Emotion* pEmotion = nullptr;
	float time{ 0.0f };
	Elite::Vector2 houseTarget{ 0.5f,0.5f };
	const float timeToLeave{ 7.5f };
	pBlackboard->GetData("Time", time);
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgentEx", pAgentInfoEx) && pBlackboard->GetData("pEntities", pEntities)
		&&pBlackboard->GetData("emotion", pEmotion);
	if (!dataAvailable)
	{
		(*pAgentInfoEx).timeInHouse = 0.0f;
		return false;
	}
	for (auto& e : (*pEntities))
	{
		if (e.Type == eEntityType::ITEM)
		{
			return false;
		}
	}
	if (pAgentInfoEx->timeInHouse < timeToLeave + (10.f - pEmotion->inattentiveDedicated) * 0.1f)
	{
		pAgentInfoEx->timeInHouse += time;
		//std::cout << pAgentInfoEx->timeInHouse << "TIME IN HOUSE\n";
	}
	if (pAgentInfoEx->timeInHouse >= timeToLeave && pInterface->Agent_GetInfo().IsInHouse)
	{
		//pBlackboard->ChangeData("HouseTarget", houseTarget);
		return true;
	}
	return false;
}
bool EnemyInSight(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	std::vector<EntityInfo>* pEntities = nullptr;
	AgentBehaviors* pAgentBehavior = nullptr;
	Elite::Vector2 emptyTarget{ 0.f,0.f };
	EnemyInfo emptyEnemy{};
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("pEntities", pEntities)
		&& pBlackboard->GetData("pAgentEx", pAgentInfoEx) && pBlackboard->GetData("AgentBehavior", pAgentBehavior);
	if (!dataAvailable)
	{
		return false;
	}
	//EnemyInfo ei = {};
	//for (auto& e : (*pEntities))
	//{
	//	if (e.Type == eEntityType::ENEMY)
	//	{
	//		ItemInfo iI{};
	//		for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
	//		{
	//			pInterface->Inventory_GetItem(i, iI);
	//			if (iI.Type == eItemType::PISTOL)//Check if you have a gun and if it has bullets
	//			{
	//				if (pInterface->Weapon_GetAmmo(iI) > 0)
	//				{
	//					pAgentBehavior->SetTarget(e.Location);
	//					pBlackboard->ChangeData("Target", e.Location);
	//					return true;
	//				}
	//				else
	//				{
	//					//pBlackboard->ChangeData("Target", emptyTarget);
	//					//pAgentBehavior->SetTarget(emptyTarget);
	//					pInterface->Inventory_RemoveItem(i);
	//				}
	//			}
	//			else if (iI.Type == eItemType::GARBAGE)//I put this in every "check for *ITEM*" conditional
	//			{
	//				pInterface->Inventory_RemoveItem(i);
	//			}
	//		}
	//	}
	//}
	//if (pEntities->size() == 0 && !HouseInFOV)
	//{
	//	pBlackboard->ChangeData("Target", emptyTarget);
	//	pAgentBehavior->SetTarget(emptyTarget);
	//}
	//return false;

	EnemyInfo ei = {};
	for (auto& e : (*pEntities))
	{
		if (e.Type == eEntityType::ENEMY)
		{
			ItemInfo iI{};
			for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
			{
				pInterface->Inventory_GetItem(i, iI);
				if (iI.Type == eItemType::PISTOL)//Check if you have a gun and if it has bullets
				{
					if (pInterface->Weapon_GetAmmo(iI) > 0)
					{
						pAgentBehavior->SetTarget(e.Location);//set target to enemy
						pBlackboard->ChangeData("enemyTarget", e.Location);
						return true;
					}
					else//if you don't have enough bullets in this gun, just remove it
					{
						//pBlackboard->ChangeData("Target", emptyTarget);
						//pAgentBehavior->SetTarget(emptyTarget);
						pInterface->Inventory_RemoveItem(i);
					}
				}
				else if (iI.Type == eItemType::GARBAGE)//I put this in every "check for *ITEM*" conditional
				{
					pInterface->Inventory_RemoveItem(i);
				}
			}
		}
	}
	//ENEMY TARGETS ZETTEN
	////
	////
	//If enemy is NOT looking for house AND there are no enemies in sight, put target to nothing
	
	if (pEntities->size() == 0 && !pAgentInfoEx->seekingHouse)
	{
		pBlackboard->ChangeData("Target", emptyTarget);
		pAgentBehavior->SetTarget(emptyTarget);
	}
	return false;

	//if (!pAgentInfoEx->hunting)
	//{
	//	if (pEntities->size() == 0)//If there's no enemies in sight, return false
	//	{
	//		pBlackboard->ChangeData("enemyTarget", emptyEnemy.Location);
	//		pAgentInfoEx->hunting = false;
	//		return false;
	//	}
	//}

	////first, check if you have enough bullets to even shoot an enemy
	//ItemInfo iI{};
	//bool hasGun{ false };
	//for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
	//{
	//	pInterface->Inventory_GetItem(i, iI);
	//	if (iI.Type == eItemType::PISTOL)//Check if you have a gun and if it has bullets
	//	{
	//		if (pInterface->Weapon_GetAmmo(iI) > 0)
	//		{
	//			hasGun = true;
	//			break;
	//		}
	//	}

	//}
	//if (!hasGun)
	//{
	//	pBlackboard->ChangeData("enemyTarget", emptyEnemy.Location);
	//	pAgentInfoEx->hunting = false;
	//	return false;
	//}
	//EnemyInfo eI{};
	//EnemyInfo mostImportantEnemy{};
	//bool thereAreEnemies{ false };
	//for (auto& e : *pEntities)
	//{
	//	if (e.Type == eEntityType::ENEMY)//If there's an enemy in sight
	//	{
	//		thereAreEnemies = true;
	//			switch (eI.Type)
	//			{
	//			case eEnemyType::ZOMBIE_HEAVY://make sure the most important enemy is the one you target

	//				mostImportantEnemy = eI;
	//				break;
	//			case eEnemyType::ZOMBIE_RUNNER:
	//				if (mostImportantEnemy.Type == eEnemyType::ZOMBIE_HEAVY)
	//				{
	//					break;
	//				}
	//				mostImportantEnemy = eI;
	//				break;
	//			case eEnemyType::ZOMBIE_NORMAL:
	//				if (mostImportantEnemy.Type == eEnemyType::ZOMBIE_HEAVY || mostImportantEnemy.Type == eEnemyType::ZOMBIE_RUNNER)
	//				{
	//					break;
	//				}
	//				mostImportantEnemy = eI;
	//				break;
	//			default:
	//				break;
	//				
	//			}

	//			
	//	}

	//}
	//if (thereAreEnemies)
	//{
	//	//	pBlackboard->ChangeData("enemyTarget", mostImportantEnemy.Location);
	//	//	pAgentBehavior->SetTarget(mostImportantEnemy.Location);
	//	//	pAgentInfoEx->hunting = true;
	//	//	return true;
	//	pAgentBehavior->SetTarget(mostImportantEnemy.Location);//set target to enemy
	//	pBlackboard->ChangeData("enemyTarget", mostImportantEnemy.Location);
	//	pAgentInfoEx->hunting = true;
	//	return true;
	//}
	//pBlackboard->ChangeData("enemyTarget", emptyEnemy.Location);
	//pAgentBehavior->SetTarget(emptyEnemy.Location);
	//pAgentInfoEx->hunting = false;
	//return false;
}
bool CanHitEnemy(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	std::vector<EntityInfo>* pEntities = nullptr;
	Elite::Vector2 target;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("enemyTarget", target)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pEntities", pEntities) &&
		pBlackboard->GetData("pAgentEx", pAgentInfoEx);
	if (!dataAvailable)
	{
		return false;
	}
	//if (abs(target.x) <= 1.f && abs(target.y) <= 1.f)
	//{
	//	pAgentInfoEx->hunting = false;
	//	return false;
	//}
	Elite::Vector2 directionVec{ target - pInterface->Agent_GetInfo().Position };
	directionVec = directionVec.GetNormalized();
	
	float angle = ToDegrees(atan2(directionVec.y, directionVec.x));//Hiermee vindt ge de hoek van uw directionVec (en zet naar graden om)
	float playerAngle = ToDegrees(pInterface->Agent_GetInfo().Orientation) - 90;
	float angleDifference = angle - playerAngle;
	while (angleDifference > 358.f)
		angleDifference -= 358.f;
	while (angleDifference < -358.f)
	{
		angleDifference += 358.f;
	}
	//std::cout << angleDifference << " ANGLEDIFFERENCE\n";
	if (abs(angleDifference ) <= 3.7f)
	{
	//	std::cout << "I CAN SHOOOOOOOOOOOOOT\n";
		return true;
	}
	return false;

}
bool HasAndShouldMedkit(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior);

	if (!dataAvailable)
	{
		return false;
	}
	if (pInterface->Agent_GetInfo().Health <= 8.f)
	{
		ItemInfo iI{};

		for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
		{
			//std::cout << "ShouldMedkitMaybe\n";

			pInterface->Inventory_GetItem(i, iI);
			if (iI.Type == eItemType::MEDKIT)//Check if you have a medkit and if it'll be used to it's full potential
			{
				//std::cout << "ShouldMedkitTrue\n";

				return true;	
			}
			 else if (iI.Type == eItemType::GARBAGE)//I put this in every "check for *ITEM*" conditional
			{
				pInterface->Inventory_RemoveItem(i);
			}
		}
	}
	//pAgentInfoEx->hasMedKit = 0;
	//std::cout << "ShouldMedkitFalse\n";
	//pAgentBehavior->SetToWander();//21/01
	return false;

}

bool HasAndShouldFood(Elite::Blackboard* pBlackboard)
{
	AgentBehaviors* pAgentBehavior = nullptr;
	IExamInterface* pInterface = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior);

	if (!dataAvailable)
	{
		return false;
	}
	float agentEnergyLeft{ 10.f - pInterface->Agent_GetInfo().Energy };
	float agentEnergyNeeded{ 10.f - agentEnergyLeft };//The amound of energy the agent needs
	//std::cout << agentEnergyLeft << "ENERGY NEEDED\n";
	if (pInterface->Agent_GetInfo().Energy <= 8.f)
	{
		ItemInfo iI{};
		for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
		{
			//std::cout << "ShouldFoodMaybe\n";

			pInterface->Inventory_GetItem(i, iI);
			if (iI.Type == eItemType::FOOD)//Check if you have food and if it'll be used to it's full potential
			{
				//std::cout << "ShouldFoodTrue\n";
				return true;
			}
			else if (iI.Type == eItemType::GARBAGE)//I put this in every "check for *ITEM*" conditional
			{
					pInterface->Inventory_RemoveItem(i);
			}
		}
	}
	//std::cout << "ShouldFoodFalse\n";

	return false;

}
bool IsBit(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	AgentBehaviors* pAgentBehavior = nullptr;
	Emotion* pEmotion = nullptr;
	const float turnTime{ 2.f };
	float time{};
	auto dataAvailable = pBlackboard->GetData("Info", pInterface) 
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&&pBlackboard->GetData("Time", time) && pBlackboard->GetData("AgentBehavior", pAgentBehavior)
		&& pBlackboard->GetData("emotion", pEmotion);
	if (!dataAvailable)
	{
		return false;
	}
	if (pAgentInfo->Bitten)//If my agent is bitten
	{
		pEmotion->memory += .5f; //Increase his memory a tiny bit, becoming angry makes him remember he is feeling something for longer
		pEmotion->fearFactors.push_back(1.f); //Make the agent more scared
		pEmotion->angerPoints += pEmotion->calmHasty;//The amount of anger points added is his calmHasty rating, meaning a calm agent's angerPoints will go up slower
		if (pEmotion->moodPoints > pEmotion->calmHasty)//If my agent has high moodPoints (meaning he is already in a bad mood), he wil get even more angry.
		{
			pEmotion->angerPoints += pEmotion->calmHasty * 0.5;
		}
		pEmotion->moodPoints += .2f;//being bitten is not fun, this makes him a little more moody

	}
	if ((pAgentInfo->Bitten || pAgentInfoEx->turning) && pAgentInfoEx->turnTimer < turnTime)
	{
		
		pAgentInfoEx->turnTimer += time;
		return true;
	}
	else
	{
		pAgentInfoEx->turning = false;
		pAgentInfoEx->turnTimer = 0.f;
	}
	if (pAgentInfo->Stamina <= 0.1f)
	{
		pAgentInfo->RunMode= false;
	}
	pAgentBehavior->SetToWander();
	return false;
}

bool NewLevel(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	float resetHouses{45.f};
	float time{};
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("Time", time);
	if (!dataAvailable)
	{
		return false;
	}
	if (pAgentInfoEx->canVisitAgain < resetHouses)
	{
		pAgentInfoEx->canVisitAgain += time;
		return false;

	}
	else
	{
		pAgentInfoEx->canVisitAgain = 0.f;
		return true;
	}
	return false;

}
bool OutOfBounds(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	Elite::Vector2 lowestBound;
	Elite::Vector2 highestBound;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx);
	lowestBound.x = pInterface->World_GetInfo().Center.x - pInterface->World_GetInfo().Dimensions.x / 2;
	lowestBound.y = pInterface->World_GetInfo().Center.y - pInterface->World_GetInfo().Dimensions.y / 2;
	highestBound.x = pInterface->World_GetInfo().Center.x + pInterface->World_GetInfo().Dimensions.x / 2;
	highestBound.y = pInterface->World_GetInfo().Center.y + pInterface->World_GetInfo().Dimensions.y / 2;
	if (!dataAvailable)
	{
		return false;
	}
	//If the agent goes out of bounds ANYWHERE, return true
	if (pAgentInfo->Position.x < lowestBound.x || pAgentInfo->Position.y < lowestBound.y || pAgentInfo->Position.x > highestBound.x
		|| pAgentInfo->Position.y > highestBound.y)
	{
		return true;
	}
	return false;
}
bool IsInPurgeZone(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	std::vector<EntityInfo>* pEntities = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&&pBlackboard->GetData("pEntities", pEntities) ;
	if (!dataAvailable)
	{
		return false;
	}
	EntityInfo ei{};
	Elite::Vector2 target{};
	for (auto& e : (*pEntities))
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			pBlackboard->ChangeData("Target", target);
			pAgentInfo->RunMode = true;
			return true;
		}
	}
	return false;
}

bool NeedAnything(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx);
	if (!dataAvailable)
	{
		return false;
	}

	ItemInfo iI{};
	if (pAgentInfoEx->leftItems.size() > 0)
	{
		bool hasMedkit = false;
		bool hasGun = false;
		bool hasFood = false;
		if (pInterface->Agent_GetInfo().Health < 5.0f) //Need medkit
		{
			for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)
			{
				pInterface->Inventory_GetItem(i, iI);
				if (iI.Type == eItemType::MEDKIT)
				{
					hasMedkit = true;

				}
			}
		}
		else if (pInterface->Agent_GetInfo().Energy < 5.0f)//Need food
		{
			for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)
			{
				pInterface->Inventory_GetItem(i, iI);
				if (iI.Type == eItemType::FOOD)
				{
					hasFood = true;
				}
			}
		}
		else//Kijk of je een wapen hebt met genoeg ammo
		{
			for (UINT i = 0; i < pInterface->Inventory_GetCapacity(); i++)
			{
				pInterface->Inventory_GetItem(i, iI);
				if (iI.Type == eItemType::PISTOL && pInterface->Weapon_GetAmmo(iI) >= 5)
				{
					hasGun = true;
				}
			}
		}
		if (!hasMedkit)
		{
			pAgentInfoEx->needMedkit = true;
			return true;
		}
		else if (!hasGun)
		{
			pAgentInfoEx->needGun = true;
			return true;
		}
		else if (!hasFood)
		{
			pAgentInfoEx->needFood = true;
			return true;
		}
	}


	pAgentInfoEx->needGun = false;
	pAgentInfoEx->needFood = false;
	pAgentInfoEx->needMedkit = false;
	return false;
}


bool GetClosestNeed(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx);
	if (!dataAvailable)
	{
		return false;
	}
	if (pAgentInfoEx->leftItems.size() > 0)
	{

		if (pAgentInfoEx->needFood)
		{
			auto it = std::find_if(pAgentInfoEx->leftItems.begin(), pAgentInfoEx->leftItems.end(), [](const ItemInfo* itemInfo)
				{
					if (itemInfo->Type == eItemType::FOOD)
						return true;
				});
			pBlackboard->ChangeData("Target", (*it)->Location);
			return true;
		}
		if (pAgentInfoEx->needMedkit)
		{
			auto it = std::find_if(pAgentInfoEx->leftItems.begin(), pAgentInfoEx->leftItems.end(), [](const ItemInfo* itemInfo)
				{
					if (itemInfo->Type == eItemType::MEDKIT)
						return true;
				});
			pBlackboard->ChangeData("Target", (*it)->Location);
			return true;
		}
		if (pAgentInfoEx->needGun)
		{
			auto it = std::find_if(pAgentInfoEx->leftItems.begin(), pAgentInfoEx->leftItems.end(), [](const ItemInfo* itemInfo)
				{
					if (itemInfo->Type == eItemType::PISTOL)
						return true;
				});
			pBlackboard->ChangeData("Target", (*it)->Location);
			return true;
		}
	}
	return false;
}

bool HasNoGun(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface = nullptr;
	AgentInfo* pAgentInfo = nullptr;
	AgentBehaviors* pAgentBehavior = nullptr;
	AgentInfo_Extended* pAgentInfoEx = nullptr;
	std::vector<EntityInfo>* pEntities = nullptr;
	auto dataAvailable = pBlackboard->GetData("Info", pInterface)
		&& pBlackboard->GetData("pAgent", pAgentInfo) && pBlackboard->GetData("pAgentEx", pAgentInfoEx)
		&& pBlackboard->GetData("AgentBehavior", pAgentBehavior) && pBlackboard->GetData("pEntities", pEntities);
	if (!dataAvailable)
	{
		return false;
	}
	EnemyInfo ei = {};
	bool hasGunWithAmmo{ false };
	bool enemyInSight{ false };
	if (pEntities->size() > 0)
	{
		enemyInSight = true;
	}
	for (auto& e : (*pEntities))
	{
		if (e.Type == eEntityType::ENEMY)
		{
			ItemInfo iI{};
			for (size_t i = 0; i < pInterface->Inventory_GetCapacity(); i++)
			{
				//std::cout << "HasNoGun\n";
				pInterface->Inventory_GetItem(i, iI);
				if (iI.Type == eItemType::PISTOL)//Check if you have a gun and if it has bullets
				{
					if (pInterface->Weapon_GetAmmo(iI) > 0)
					{
						pAgentBehavior->SetTarget(e.Location);
						pBlackboard->ChangeData("Target", e.Location);
						hasGunWithAmmo = true;
					}
					else
					{
						pInterface->Inventory_RemoveItem(i);
					}
				}
				else if (iI.Type == eItemType::GARBAGE)//I put this in every "check for *ITEM*" conditional
				{
					pInterface->Inventory_RemoveItem(i);
				}
			}
		}
	}
	if (hasGunWithAmmo || !enemyInSight)
		return false;
	pAgentBehavior->SetTarget((*pEntities)[0].Location);
	return true;

}
#endif