#include "GasManager.h"
#include <BWAPI.h>
#include <BWTA.h>
#include <set>
#include <vector>

using namespace BWAPI;
using namespace std;

GasManager::GasManager() 
{
	refineryCount = 0;
	refineryConstructingCount = 0;
	worksersConstructing = 0;
	newRefineries = 0;
}

void GasManager::update()
{

	// TODO List
	// get calc gas income rate
	// record when geysers become depleted
	// worry about gas steal?


	Broodwar->drawTextScreen(2, 30, "\x10 GM : (SCV=%d)", numAgents(UnitTypes::Terran_SCV));

	// vector
	vector<Agent*> refineries;
	vector<Agent*>::iterator refinery;

	// set
	set<Agent*>::iterator agent;

	// how many refineries do we own
	int count = numAgents(UnitTypes::Terran_Refinery);

	newRefineries = count - refineryCount;
	refineryCount = count;

	// workers done building refineries
	if (newRefineries) {
		refineryConstructingCount -= newRefineries;
		worksersConstructing -= newRefineries;
	}

	// check for refinery
	// if not put one scv into build state
	if (refineryCount == 0 && numAgents(UnitTypes::Terran_SCV) && 
		refineryConstructingCount == 0) {
			//Broodwar->sendText("Gas manager, search for agent out of %d\n", (int)agents.size());
			// keep track
			refineryConstructingCount++;
			worksersConstructing++;
			// find a woker to do the work
			// for now just grab first, don't care where, just can't be a refinery
			for (agent = agents.begin(); agent != agents.end(); agent++) {
				const Unit& unit = (*agent)->getUnit();
				if (unit.getType().isWorker()) {
					Broodwar->sendText("Gas manager, set agent to build");
					(*agent)->setState(BuildState);
					(*agent)->setUnitTypeTarget(BWAPI::UnitTypes::Terran_Refinery);
					break;
				}
			}
	}

	// get our refineries
	for (agent = agents.begin(); agent != agents.end(); agent++) {
		const Unit &unit = (*agent)->getUnit();
		if (unit.getType() == BWAPI::UnitTypes::Terran_Refinery) {
			refineries.push_back(*agent);
		}
	}

	if (refineries.size() == 0) {
		return;
	}

	for (agent = agents.begin(); agent != agents.end(); agent++)
	{
		Unit& unit = (*agent)->getUnit();
		if (unit.getType().isWorker() && !unit.isGatheringGas()) {
			for (refinery = refineries.begin(); refinery != refineries.end(); refinery++) {
				(*agent)->setUnitTarget(&((*refinery)->getUnit()));
				(*agent)->setState(GatherState);
			}
		}
	}

	// Update all agents
	Manager::update();
}
