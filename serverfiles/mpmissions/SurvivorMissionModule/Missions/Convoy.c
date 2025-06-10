class ConvoyMission extends SurvivorMissions
{
	//Mission related entities 
	Car RewardCar; // vehicle reward, for this mission Heli is actually a hmmwv or dingo.
	Object TireFire;
	vector Foxholepos;

	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI

	eAIGroup group;
	eAIGroup m_ConvoyAIGroup;

	#endif
	#endif

	//Mission settings
	int HordeDensity = 30;					//infected per wave 

	//Mission parameters
	int MsgDlyFinish = 30;					//seconds, message delay time after player has finished mission

	// Activate Expansion AI Patrol
	bool SpawnPatrolEnabled = true;

	// Activate PVEZ PVP Zone
	bool SpawnPVPZone = false;

	#ifdef PVEZ
	#ifdef PVEZ

	// PVEZ PVP Zone Globals
	private PVEZ_Zone m_PVPZone;
	private string m_PVPZoneName;

	#endif
	#endif

	// Expansion AI Globals
	const int MIN_SQUAD_SIZE = 2;   // Minimum squad size when randomized
	const int MAX_SQUAD_SIZE = 8;   // Maximum squad size when randomized
	const int RANDOM = -1;
	const int DEFAULT_FACTION = 0;    // Raiders
	const int DEFAULT_LOADOUT = 1;    // Police
	const int DEFAULT_SQUAD_SIZE = 4; // Default squad size
    const int DEFAULT_FORMATION = 0; // Default Formation: Vee
	const float DEFAULT_PATROL_RADIUS = 25.0; // Default patrol radius

	//Mission containers
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;	
	ref array<string> WreckedVehicle = new array<string>;
	ref array<vector> WreckVector = new array<vector>;
	ref array<vector> scoutlocation = new array<vector>;
	ref array<ref Param3<string,vector,vector>> Wreckage = new array<ref Param3<string,vector,vector>>;
	ref array<ref Param1<string>> Vehicles = new array<ref Param1<string>>;


	//we need an array where we hold those dead infected
    ref array<Object> DeadInfected = new array<Object>;

	//Mission variables 	
	string SelectedVehicle;
	string VehicleType;
	vector scoutpos;

	//Mission timeout
  	int MissionCutoffTime;

	override bool IsExtended() return false;
	bool secondwavespawned = false;
	
	void ConvoyMission()
	{
		//Mission mission timeout
		m_MissionTimeout = 2400;			//seconds, mission duration time

		//Mission zones
		m_MissionZoneOuterRadius = 500.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 5.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Set mission messages

		if(SpawnPatrolEnabled)
		{
			m_MissionMessage1 = "A Convoy has been overrun by raiders "+ m_MissionLocationDir +" of "+ m_MissionLocation + ". The NBC hazmat crews are in need of assistance.";
			m_MissionMessage2 = "They just finished burning the dead in the tire-pile left by the raiders but now they see more waves of incoming.";
			m_MissionMessage3 = "Help them fight off the infected. This will give them time to find what the survivors dropped when they were attacked.";
		}
		else
		{
			m_MissionMessage1 = "A Convoy has been attacked by raiders "+ m_MissionLocationDir +" of "+ m_MissionLocation + ".";
			m_MissionMessage2 = "It seems the area was not safe to travel through. They were attacked, Please go to the Area and Investigate.";
			m_MissionMessage3 = "The Lone Survivor who came back said he was repairing the vehicle when he was attacked. He said he dropped a USB with info on the Infected somewhere in the area.";
			m_MissionMessage3 = "He said Be careful, you may need to clear the Infected in the area before you can find it.";		
		}
	
        //Wrecked vehicles Spawn Locations
		WreckVector.Insert("5 .70 -11.2");		WreckVector.Insert("-.05 .7 -10.2");	
		WreckVector.Insert("0.5 0 30");			WreckVector.Insert("5.5 0 29");
		WreckVector.Insert("5.5 0 18");			WreckVector.Insert("0 0 19");
		//WreckVector.Insert("0 0.25 -32");		WreckVector.Insert("5 0.25 -35");
		WreckVector.Insert("0 0 0");			WreckVector.Insert("5 0 0");
		scoutlocation.Insert("5 0 -30");        scoutlocation.Insert("0 0 -29");
		
		//Infected spawnpoints
		InfectedSpawns.Insert("-10.5186 0 25.0269");
		InfectedSpawns.Insert("24.9775 0 -10.4146");
		InfectedSpawns.Insert("-30.1726 0 -6.2729");
		InfectedSpawns.Insert("-20.9209 0 4.6636");
		InfectedSpawns.Insert("15.0283 0 -10.3276");
		InfectedSpawns.Insert("7.2461 0 30.5884");
		InfectedSpawns.Insert("-20.6855 0 5.9956");
		InfectedSpawns.Insert("40 0 20");
		InfectedSpawns.Insert("-20 0 -40");
		InfectedSpawns.Insert("-50 0 50");
		InfectedSpawns.Insert("35 0 40");
		InfectedSpawns.Insert("-40 0 -20");
		InfectedSpawns.Insert("-30 0 -40");
		
		//Infected types
		//Male												//Female
		InfectedTypes.Insert("ZmbM_CitizenASkinny_Brown");	InfectedTypes.Insert("ZmbF_JournalistNormal_White");
		InfectedTypes.Insert("ZmbM_priestPopSkinny");		InfectedTypes.Insert("ZmbF_Clerk_Normal_White");
		InfectedTypes.Insert("ZmbM_HermitSkinny_Beige");	InfectedTypes.Insert("ZmbF_CitizenANormal_Blue");
		InfectedTypes.Insert("ZmbM_CitizenBFat_Red");		InfectedTypes.Insert("ZmbF_CitizenBSkinny");
		InfectedTypes.Insert("ZmbM_FishermanOld_Green");	InfectedTypes.Insert("ZmbF_HikerSkinny_Grey");
		InfectedTypes.Insert("ZmbM_HunterOld_Autumn");		InfectedTypes.Insert("ZmbF_SurvivorNormal_Orange");
		InfectedTypes.Insert("ZmbM_CitizenBFat_Blue");		InfectedTypes.Insert("ZmbF_HikerSkinny_Green");
		InfectedTypes.Insert("ZmbM_MotobikerFat_Black");	InfectedTypes.Insert("ZmbF_JoggerSkinny_Green");
		InfectedTypes.Insert("ZmbM_JoggerSkinny_Red");		InfectedTypes.Insert("ZmbF_SkaterYoung_Striped");
		InfectedTypes.Insert("ZmbM_MechanicSkinny_Grey");	InfectedTypes.Insert("ZmbF_BlueCollarFat_Red");
		InfectedTypes.Insert("ZmbM_HandymanNormal_Green");	InfectedTypes.Insert("ZmbF_MechanicNormal_Beige");
		InfectedTypes.Insert("ZmbM_HeavyIndustryWorker");	InfectedTypes.Insert("ZmbF_PatientOld");
		InfectedTypes.Insert("ZmbF_BlueCollarFat_Green");	InfectedTypes.Insert("ZmbF_ShortSkirt_beige");
		InfectedTypes.Insert("ZmbM_PolicemanSpecForce");	InfectedTypes.Insert("ZmbF_DoctorSkinny");
		InfectedTypes.Insert("ZmbF_VillagerOld_Red");
		InfectedTypes.Insert("ZmbM_HikerSkinny_Blue");		InfectedTypes.Insert("ZmbF_JoggerSkinny_Red");
		InfectedTypes.Insert("ZmbM_HikerSkinny_Yellow");	InfectedTypes.Insert("ZmbF_MilkMaidOld_Beige");
		InfectedTypes.Insert("ZmbM_PolicemanFat");			InfectedTypes.Insert("ZmbF_VillagerOld_Green");
		InfectedTypes.Insert("ZmbM_PatrolNormal_Summer");	InfectedTypes.Insert("ZmbF_ShortSkirt_yellow");
		InfectedTypes.Insert("ZmbM_JoggerSkinny_Blue");		InfectedTypes.Insert("ZmbF_NurseFat");
		InfectedTypes.Insert("ZmbM_VillagerOld_White");		InfectedTypes.Insert("ZmbF_PoliceWomanNormal");
		InfectedTypes.Insert("ZmbM_SkaterYoung_Brown");		InfectedTypes.Insert("ZmbF_HikerSkinny_Blue");
		InfectedTypes.Insert("ZmbM_MechanicSkinny_Green");	InfectedTypes.Insert("ZmbF_ParamedicNormal_Green");
		InfectedTypes.Insert("ZmbM_DoctorFat");				InfectedTypes.Insert("ZmbF_JournalistNormal_Red");
		InfectedTypes.Insert("ZmbM_PatientSkinny");			InfectedTypes.Insert("ZmbF_SurvivorNormal_White");
		InfectedTypes.Insert("ZmbM_ClerkFat_Brown");		InfectedTypes.Insert("ZmbF_JoggerSkinny_Brown");
		InfectedTypes.Insert("ZmbM_ClerkFat_White");		InfectedTypes.Insert("ZmbF_MechanicNormal_Grey");
		InfectedTypes.Insert("ZmbM_Jacket_magenta");		InfectedTypes.Insert("ZmbM_Jacket_black");
		InfectedTypes.Insert("ZmbM_Jacket_stripes");
		
		//you can change this to whatever you want the vehicle to be.  beware of the naming scheme you might need to change code a bit.
		//these two vehicle types have a standard naming convention for door parts, which lets you select the color you want easily.
		Vehicles.Insert(new Param1<string>("Offroad_02")); // This will be the Reward Vehicle
		Vehicles.Insert(new Param1<string>("CivilianSedan"));
		Vehicles.Insert(new Param1<string>("CivilianSedan_Wine"));
		Vehicles.Insert(new Param1<string>("CivilianSedan_Black"));
		Vehicles.Insert(new Param1<string>("Hatchback_02"));
		Vehicles.Insert(new Param1<string>("Hatchback_02_Blue"));
		Vehicles.Insert(new Param1<string>("Hatchback_02_Black"));
		Vehicles.Insert(new Param1<string>("OffroadHatchback_Blue"));
		Vehicles.Insert(new Param1<string>("OffroadHatchback_White"));
		Vehicles.Insert(new Param1<string>("OffroadHatchback"));
		Vehicles.Insert(new Param1<string>("Sedan_02"));
		Vehicles.Insert(new Param1<string>("Sedan_02_Red"));
		Vehicles.Insert(new Param1<string>("Sedan_02_Grey"));
		Vehicles.Insert(new Param1<string>("Truck_01_Covered"));
		Vehicles.Insert(new Param1<string>("Truck_01_Covered_Blue"));
		Vehicles.Insert(new Param1<string>("Truck_01_Covered_Orange"));
		
		
		// If available on your server
		//Get random vehicle 
		int	RandomVehicle = 0;	//!change randomization limit after adding new vehicles!
		if (Vehicles.Count() > 0 && RandomVehicle >= 0 && RandomVehicle < Vehicles.Count())
		{
			Param1<string> VehiclesDef = Vehicles.Get(RandomVehicle);	
			VehicleType = VehiclesDef.param1;

			SelectedVehicle = VehicleType;
		}
		else
		{
			Print("[SMM] ERROR: Vehicles array is empty or RandomVehicle index is out of range!");
			VehicleType = "";
			SelectedVehicle = "";
		}
		// Insert the main vehicle as before
		WreckedVehicle.Insert(SelectedVehicle);

		// Now select random vehicles from the Vehicles array (excluding the main reward vehicle)
		string RandomWreck1 = SelectedVehicle;

		array<string> wreckChoices = {
			"CivilianSedan",
			"Hatchback_02",
			"OffroadHatchback",
			"Sedan_02",
			"Truck_01_Covered",
			"Truck_01_Covered_Blue",
			"Truck_01_Covered_Orange",
			"Hatchback_02_Blue",
			"Sedan_02_Grey",
			"Sedan_02_Red",
			"Sedan_02",
			"OffroadHatchback_White",
			"OffroadHatchback_Blue",
			"Offroad_02",
			"CivilianSedan",
			"CivilianSedan_Wine",
			"CivilianSedan_Black",
			"Hatchback_02_Black"
		};

		wreckChoices.RemoveItem(SelectedVehicle);

		string RandomWreck2 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck2);
		string RandomWreck3 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck3);
		string RandomWreck4 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck4);
		string RandomWreck5 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck5);
		string RandomWreck6 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck6);
		string RandomWreck7 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck7);
		string RandomWreck8 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck8);

		// You can randomize these if you want, but ensure no duplicates and not the main reward vehicle
		// For example:
		// array<string> wreckChoices = {"CivilianSedan", "Hatchback_02", "OffroadHatchback", "Sedan_02", "Truck_01_Chassis", "Truck_01_Covered"};
		// wreckChoices.RemoveItem(SelectedVehicle);
		// RandomWreck2 = wreckChoices.GetRandomElement(); wreckChoices.RemoveItem(RandomWreck2);
		// ... repeat for 3, 4, 5, 6

		vector Wpos2 = WreckVector.GetRandomElement();
		WreckVector.RemoveItem(Wpos2);
		vector Wpos3 = WreckVector.GetRandomElement();
		WreckVector.RemoveItem(Wpos3);
		vector Wpos4 = WreckVector.GetRandomElement();
		WreckVector.RemoveItem(Wpos4);
		vector Wpos5 = WreckVector.GetRandomElement();
		WreckVector.RemoveItem(Wpos5);
		vector Wpos6 = WreckVector.GetRandomElement();
		WreckVector.RemoveItem(Wpos6);
		vector Wpos7 = WreckVector.GetRandomElement();
		WreckVector.RemoveItem(Wpos7);
		vector Wpos8 = WreckVector.GetRandomElement();
		WreckVector.RemoveItem(Wpos8);

		scoutpos = scoutlocation.GetRandomElement();

		// Repeat for each wreck car, customizing missing parts as needed
		
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck1, scoutpos, "0 0 0"));
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck2, Wpos2, "0 0 0"));
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck3, Wpos3, "-1 0 0"));
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck4, Wpos4, "0 0 0"));
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck5, Wpos5, "0 0 0"));
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck6, Wpos6, "0 0 0"));
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck7, Wpos7, "0 0 0"));
		Wreckage.Insert( new Param3<string,vector,vector>(RandomWreck8, Wpos8, "0 0 0"));

		Wreckage.Insert( new Param3<string,vector,vector>("Misc_TirePile_Burning_DE", "-5 0 8", "0 0 0"));

		MissionCutoffTime = MissionSettings.RestartCycleTime - (m_MissionTimeout + MissionSettings.DelayTime);
    
    	if ( GetGame().GetTime() * 0.001 > MissionCutoffTime )
    	{
			MissionSettings.DelayTime = 3600;
		}

		#ifdef PVEZ
		#ifdef PVEZ

		if(SpawnPVPZone)
		{
		// Add New PVP Zone to Mission.
		AddNewDynamicZone();
		// Schedule zone cleanup after mission timeout
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(zoneCleanup, m_MissionTimeout * 1000, false);
		Print("[SMM] PVEZ PVP Zone to be Cleaned up at Mission Timeout");
		}
		else
		{
		Print("[SMM] PVEZ PVP Zone is Disabled");
		}

		#endif
		#endif
	}
	
	void ~ConvoyMission()
	{

		// Set the lifetime of each car in m_MissionObjects (ConvoyWreckage) to 3 seconds - Incase of Lack of Clean-Up
		for (int ih = 0; ih < m_MissionObjects.Count(); ih++)
		{
			Car car = Car.Cast(m_MissionObjects.Get(ih));
			if (car)
			{
				car.SetLifetime(3);
			}
		}

		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{
				if ( !m_MissionObjects.Get(i) ) continue;
				else GetGame().ObjectDelete( m_MissionObjects.Get(i) );		
			}
			m_MissionObjects.Clear();
		}
		
		//Despawn mission AI's
		if ( m_MissionAIs )
		{
			if ( m_MissionAIs.Count() > 0 )
			{
				Print("[SMM] Despawning "+ m_MissionAIs.Count() +" mission AI's from old mission...");
				for ( int k = 0; k < m_MissionAIs.Count(); k++ )
				{
					GetGame().ObjectDelete( m_MissionAIs.Get(k) );
				}
				m_MissionAIs.Clear();			
			}
			else Print("[SMM] No mission AI's to despawn.");
		}

		//Delete PlayersInZone list & reset container takeaway
		if ( m_PlayersInZone )	m_PlayersInZone.Clear();
		if ( m_ContainerWasTaken ) m_ContainerWasTaken = false;	
	}

	#ifdef PVEZ
	#ifdef PVEZ

	void AddNewDynamicZone()
    {    
        // Set the zone type, for example, a PVP zone or any other type
        int type = PVEZ_ZONE_TYPE_STATIC;  
        
        // Extract position from the mission position
        vector missionPos = m_MissionPosition; // Extract position
        float x = missionPos[0]; // X coordinate
        float z = missionPos[2]; // Z coordinate

        // Set the radius for the zone (you can adjust this as needed)
        float radius = 500.0;  // Example radius, you can change this value

        // Set name for the zone
        string name = "Convoy PVP Zone";
        // Do you want border on map.
        bool showBorder = true;
        // Do you want to show the name on Map
        bool showName = true;

        // Add the new dynamic PVP zone
        m_PVPZone = g_Game.pvez_Zones.AddZone(type, x, z, radius, name, showBorder, showName);

        // Store the zone's name
        m_PVPZoneName = name;

        // Check if the zone was added successfully
        if (m_PVPZone != NULL) {
            Print("New dynamic PVP zone added: " + m_PVPZoneName);
        } else {
            Print("Failed to add new dynamic PVP zone.");
        }

        // Update zones and push the changes to clients
        g_Game.pvez_Zones.Init();  // Re-initialize zones
        g_Game.pvez_Zones.PushUpdateToClients();  // Push updates to all clients
    }

	void zoneCleanup()
    {
        if (m_PVPZone)
        {
            g_Game.pvez_Zones.RemoveZone(m_PVPZone);
            Print("[SMM] PVP zone removed via zoneCleanup: " + m_PVPZoneName);

            // Update zones and push the changes to clients
            g_Game.pvez_Zones.Init();  
            g_Game.pvez_Zones.PushUpdateToClients();  
            Print("[SMM] PVP zone removal pushed to clients.");
        }
        else
        {
            Print("[SMM] No PVP zone found to remove.");
        }
    }

	#endif
	#endif

	void SetupWreckAttachments(Car wreck, string type, int numWheels)
	{
		string color = "";
		if (type == "CivilianSedan_Black") color = "_Black";
		else if (type == "CivilianSedan_Wine") color = "_Wine";
		else if (type == "OffroadHatchback_Blue") color = "_Blue";
		else if (type == "OffroadHatchback_White") color = "_White";
		else if (type == "Hatchback_02_Blue") color = "_Blue";
		else if (type == "Hatchback_02_Black") color = "_Black";
		else if (type == "Sedan_02_Red") color = "_Red";
		else if (type == "Sedan_02_Grey") color = "_Grey";

		array<string> doorSlots = {};
		int i; // Declare loop variable once at top
		string baseName; // Declare baseName once at top
		string attachName; // Declare attachName once at top
		string hoodName; // Declare hoodName once at top
		string trunkName; // Declare trunkName once at top

		if (type.Contains("CivilianSedan"))
		{
			for (i = 0; i < numWheels; i++)
				wreck.GetInventory().CreateAttachment("CivSedanWheel_Ruined");
			doorSlots = {"Driver", "CoDriver", "BackLeft", "BackRight"};
			for (i = 0; i < doorSlots.Count(); i++)
			{
				baseName = "CivSedanDoors_" + doorSlots[i] + color;
				if (Math.RandomInt(0, 2) == 0)
					attachName = baseName + "Rust";
				else
					attachName = baseName;
				wreck.GetInventory().CreateAttachment(attachName);
			}
			hoodName = "CivSedanHood" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					hoodName = "CivSedanHood_WhiteRust";
				else
					hoodName = hoodName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(hoodName);
			trunkName = "CivSedanTrunk" + color;
			if (Math.RandomInt(0, 2) == 0)
				if (color == "")
					trunkName = "CivSedanTrunk_WhiteRust";
				else
					trunkName = trunkName + "Rust";
			wreck.GetInventory().CreateAttachment(trunkName);
			if (Math.RandomInt(0, 2) == 0) wreck.GetInventory().CreateAttachment("CarBattery");
			if (Math.RandomInt(0, 2) == 0) wreck.GetInventory().CreateAttachment("HeadlightH7");
			if (Math.RandomInt(0, 2) == 0) wreck.GetInventory().CreateAttachment("HeadlightH7");
		}
		else if (type.Contains("OffroadHatchback"))
		{
			for (i = 0; i < numWheels; i++)
				wreck.GetInventory().CreateAttachment("HatchbackWheel_Ruined");
			doorSlots = {"Driver", "CoDriver"};
			for (i = 0; i < doorSlots.Count(); i++)
			{
				baseName = "HatchbackDoors_" + doorSlots[i] + color;
				if (Math.RandomInt(0, 2) == 0)
					attachName = baseName + "Rust";
				else
					attachName = baseName;
				wreck.GetInventory().CreateAttachment(attachName);
			}
			hoodName = "HatchbackHood" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					hoodName = "HatchbackHood_GreenRust";
				else
					hoodName = hoodName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(hoodName);
			trunkName = "HatchbackTrunk" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					trunkName = "HatchbackHood_GreenRust";
				else
					trunkName = trunkName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(trunkName);
		}
		else if (type.Contains("Hatchback_02"))
		{
			for (i = 0; i < numWheels; i++)
				wreck.GetInventory().CreateAttachment("HatchbackWheel_Ruined");
			doorSlots = {"_Door_1_1", "_Door_1_2", "_Door_2_1", "_Door_2_2"};
			for (i = 0; i < doorSlots.Count(); i++)
			{
				baseName = "Hatchback_02" + doorSlots[i] + color;
				if (Math.RandomInt(0, 2) == 0)
					attachName = baseName + "Rust";
				else
					attachName = baseName;
				wreck.GetInventory().CreateAttachment(attachName);
			}
			hoodName = "Hatchback_02_Hood" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					hoodName = "Hatchback_02_Hood_RedRust";
				else
					hoodName = hoodName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(hoodName);
			trunkName = "Hatchback_02_Trunk" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					trunkName = "Hatchback_02_Trunk_RedRust";
				else
					trunkName = trunkName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(trunkName);
		}
		else if (type.Contains("Sedan_02"))
		{
			for (i = 0; i < numWheels; i++)
				wreck.GetInventory().CreateAttachment("Sedan_02_Wheel_Ruined");
			doorSlots = {"_Door_1_1", "_Door_1_2", "_Door_2_1", "_Door_2_2"};
			for (i = 0; i < doorSlots.Count(); i++)
			{
				baseName = "Sedan_02" + doorSlots[i] + color;
				if (Math.RandomInt(0, 2) == 0)
					attachName = baseName + "Rust";
				else
					attachName = baseName;
				wreck.GetInventory().CreateAttachment(attachName);
			}
			hoodName = "Sedan_02_Hood" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					hoodName = "Sedan_02_Hood_YellowRust";
				else
					hoodName = hoodName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(hoodName);
			trunkName = "Sedan_02_Trunk" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					trunkName = "Sedan_02_Trunk_YellowRust";
				else
					trunkName = trunkName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(trunkName);
		}
		else if (type.Contains("Truck_01_Covered"))
		{
			for (i = 0; i < 2; i++)
				if (Math.RandomInt(0, 2) == 0)
					wreck.GetInventory().CreateAttachment("Truck_01_Wheel_Ruined");
			for (i = 0; i < 4; i++)
				if (Math.RandomInt(0, 2) == 0)
					wreck.GetInventory().CreateAttachment("Truck_01_WheelDouble_Ruined");
			array<string> truckDoors = {"Truck_01_Door_1_1", "Truck_01_Door_2_1"};
			for (i = 0; i < truckDoors.Count(); i++)
			{
				if (Math.RandomInt(0, 2) == 0)
					attachName = truckDoors[i] + "Rust";
				else
					attachName = truckDoors[i];
				wreck.GetInventory().CreateAttachment(attachName);
			}
			hoodName = "Truck_01_Hood" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					hoodName = "Truck_01_Hood_GreenRust";
				else
					hoodName = hoodName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(hoodName);
			trunkName = "Truck_01_Trunk" + color;
			if (Math.RandomInt(0, 2) == 0)
			{
				if (color == "")
					trunkName = "Truck_01_Trunk_GreenRust";
				else
					trunkName = trunkName + "Rust";
			}
			wreck.GetInventory().CreateAttachment(trunkName);
		}
	}

	void SpawnObjects()
	{
		Object SmokeGrenade;
		//Spawn crashed plane crashed C130 
		int PlaneOriConversion;
		string PlaneOrientation;
		string color = "";
		
		PlaneOriConversion = m_MissionDescription[3].ToInt() - 180;
		if ( PlaneOriConversion < 0 ) PlaneOriConversion += 360;
		PlaneOrientation = PlaneOriConversion.ToString() +" 0 0";
		
		if (!m_MissionPosition || m_MissionPosition == "0 0 0") {
			Print("[SMM] ERROR: m_MissionPosition is not set or invalid when spawning SmokeGrenade!");
			return;
		}
		SmokeGrenade = GetGame().CreateObject("M18SmokeGrenade_Green", m_MissionPosition);
		SmokeGrenade.SetOrientation(PlaneOrientation.ToVector());
		SmokeGrenade.PlaceOnSurface();
		m_MissionObjects.Insert(SmokeGrenade);
		M18SmokeGrenade_Green smoke = M18SmokeGrenade_Green.Cast(SmokeGrenade);
		if (smoke)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(smoke.ActivateImmediate, 30000, false);
		}

		for ( int i=0; i < Wreckage.Count(); i++ )
		{
			Param3<string,vector,vector> WreckageDef = Wreckage.Get(i);
			string WreckageType = WreckageDef.param1;
			vector WreckagePos = SmokeGrenade.ModelToWorld( WreckageDef.param2 );
			vector WreckageOri = WreckageDef.param3;
			vector WreckageDir = SmokeGrenade.GetDirection();
	
			if (WreckageType == SelectedVehicle)
			{
				RewardCar = Car.Cast(GetGame().CreateObject(SelectedVehicle, WreckagePos));
				
				if (VehicleType == "Offroad_02")
				{
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Wheel");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Wheel");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Wheel");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Wheel");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Wheel");
					RewardCar.GetInventory().CreateAttachment("CarBattery");
					RewardCar.GetInventory().CreateAttachment("HeadlightH7");
					RewardCar.GetInventory().CreateAttachment("HeadlightH7");
					RewardCar.GetInventory().CreateAttachment("CanisterGasoline");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Door_1_1");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Door_1_2");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Door_2_2");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Door_2_1");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Hood");
					RewardCar.GetInventory().CreateAttachment("Offroad_02_Trunk");
				}
				
				RewardCar.SetOrientation( PlaneOrientation.ToVector() );
				RewardCar.PlaceOnSurface();
				m_MissionObjects.InsertAt( RewardCar, 0 );
	

				RewardCar.SetLifetime( (m_MissionTimeout - m_MissionTime) + 30 );
			}

			if (WreckageType == "Misc_TirePile_Burning_DE")
			{
				Object BurningTires = GetGame().CreateObject(WreckageType, WreckagePos);
				TireFire = BurningTires;
				Foxholepos = BurningTires.GetPosition();
				TireFire.SetPosition(Foxholepos);
				TireFire.SetOrientation(BurningTires.GetOrientation());
				m_MissionObjects.Insert(TireFire);
			}
            else if (WreckageType != SelectedVehicle)
            {
                Car ConvoyWreckage = Car.Cast(GetGame().CreateObject(WreckageType, WreckagePos));
                if (!ConvoyWreckage)
                {
                    Print("[SMM] ERROR: Failed to create ConvoyWreckage of type " + WreckageType + " at " + WreckagePos.ToString());
                    continue;
                }
                ConvoyWreckage.SetPosition(WreckagePos);
                ConvoyWreckage.SetDirection(WreckageDir);
                vector ori = ConvoyWreckage.GetOrientation();
                ori[0] = ori[0] + 180;
                ConvoyWreckage.SetOrientation(ori + WreckageOri);
                ConvoyWreckage.PlaceOnSurface();

                int numWheels = Math.RandomIntInclusive(1, 4);
                SetupWreckAttachments(ConvoyWreckage, WreckageType, numWheels);

                Car.Cast(ConvoyWreckage).SetLifetime((m_MissionTimeout - m_MissionTime) + 300);
                m_MissionObjects.Insert(ConvoyWreckage);
            }

		}
		PlaneOriConversion = m_MissionDescription[3].ToInt();
		PlaneOrientation = PlaneOriConversion.ToString() +" 0 0";
		RewardCar.SetOrientation( PlaneOrientation.ToVector() );
		RewardCar.PlaceOnSurface();
	
		m_MissionPosition = TireFire.ModelToWorld("0 0 0");
							
		Print("[SMM] Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}
	
	void SpawnRewards()
	{
        if (!RewardCar) return;

		//Get random loadout
		int selectedLoadout = Math.RandomIntInclusive( 0, 9);	//!change randomization limit after adding new loadouts!	

		//Spawn selected loadout items in mission object
		EntityAI weapon;
				
		if ( selectedLoadout == 0 )
		{
			weapon = RewardCar.GetInventory().CreateInInventory("M4A1_Green");
				weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
				weapon.GetInventory().CreateAttachment("M4_MPBttstck");
				weapon.GetInventory().CreateAttachment("ACOGOptic");
				weapon.GetInventory().CreateAttachment("M4_Suppressor");
			RewardCar.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
			RewardCar.GetInventory().CreateInInventory("M4_T3NRDSOptic");
			RewardCar.GetInventory().CreateInInventory("Ammo_556x45");
			RewardCar.GetInventory().CreateInInventory("Ammo_556x45");
			RewardCar.GetInventory().CreateInInventory("CanOpener");
			RewardCar.GetInventory().CreateInInventory("PeachesCan");
			RewardCar.GetInventory().CreateInInventory("Canteen");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 1)
		{
			weapon = RewardCar.GetInventory().CreateInInventory("SVD");
				weapon.GetInventory().CreateAttachment("PSO1Optic");
			RewardCar.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
			RewardCar.GetInventory().CreateInInventory("PSO1Optic");
			RewardCar.GetInventory().CreateInInventory("KazuarOptic");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x54");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x54");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x54");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x54");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x54");
			RewardCar.GetInventory().CreateInInventory("CanOpener");
			RewardCar.GetInventory().CreateInInventory("PeachesCan");
			RewardCar.GetInventory().CreateInInventory("Canteen");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 2)
		{
			weapon = RewardCar.GetInventory().CreateInInventory("AKM");
				weapon.GetInventory().CreateAttachment("AK_RailHndgrd_Green");
				weapon.GetInventory().CreateAttachment("AK_PlasticBttstck_Green");
				weapon.GetInventory().CreateAttachment("PSO1Optic");
				weapon.GetInventory().CreateAttachment("AK_Suppressor");
			RewardCar.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x39");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x39");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x39");
			RewardCar.GetInventory().CreateInInventory("CanOpener");
			RewardCar.GetInventory().CreateInInventory("PeachesCan");
			RewardCar.GetInventory().CreateInInventory("Canteen");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 3)
		{
			weapon = RewardCar.GetInventory().CreateInInventory("FAL");
				weapon.GetInventory().CreateAttachment("Fal_OeBttstck");
			RewardCar.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
			RewardCar.GetInventory().CreateInInventory("M4_Suppressor");
			RewardCar.GetInventory().CreateInInventory("ACOGOptic");
			RewardCar.GetInventory().CreateInInventory("FNX45");
			RewardCar.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			RewardCar.GetInventory().CreateInInventory("Ammo_45ACP");
			RewardCar.GetInventory().CreateInInventory("FNP45_MRDSOptic");
			RewardCar.GetInventory().CreateInInventory("PistolSuppressor");
			RewardCar.GetInventory().CreateInInventory("PsilocybeMushroom");
			RewardCar.GetInventory().CreateInInventory("AmmoBox");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
		}	
		if (selectedLoadout == 4)
		{
			weapon = RewardCar.GetInventory().CreateInInventory("SKS");
				weapon.GetInventory().CreateAttachment("PUScopeOptic");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x39");
			RewardCar.GetInventory().CreateInInventory("Ammo_762x39");
			weapon = RewardCar.GetInventory().CreateInInventory("FNX45");
				weapon.GetInventory().CreateAttachment("PistolSuppressor");
				EntityAI weaponlight = weapon.GetInventory().CreateAttachment("TLRLight");
					weaponlight.GetInventory().CreateAttachment("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			RewardCar.GetInventory().CreateInInventory("Ammo_45ACP");
			RewardCar.GetInventory().CreateInInventory("AmmoBox");
		}	
		if (selectedLoadout == 5)
		{
			weapon = RewardCar.GetInventory().CreateInInventory("Winchester70");
				weapon.GetInventory().CreateAttachment("HuntingOptic");
			RewardCar.GetInventory().CreateInInventory("Ammo_308Win");
			RewardCar.GetInventory().CreateInInventory("Ammo_308Win");
			RewardCar.GetInventory().CreateInInventory("FNX45");
			RewardCar.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
			RewardCar.GetInventory().CreateInInventory("Ammo_45ACP");
			RewardCar.GetInventory().CreateInInventory("AmmoBox");
			RewardCar.GetInventory().CreateInInventory("PistolSuppressor");
			RewardCar.GetInventory().CreateInInventory("TLRLight");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
		}
		if (selectedLoadout == 6)
		{			
			weapon = RewardCar.GetInventory().CreateInInventory("MP5K");
				weapon.GetInventory().CreateAttachment("MP5_RailHndgrd");
				weapon.GetInventory().CreateAttachment("MP5k_StockBttstck");
				weapon.GetInventory().CreateAttachment("M68Optic");
				weapon.GetInventory().CreateAttachment("PistolSuppressor");			
			RewardCar.GetInventory().CreateInInventory("Mag_MP5_30Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_MP5_30Rnd");
			RewardCar.GetInventory().CreateInInventory("AmmoBox_9x19_25rnd");
			RewardCar.GetInventory().CreateInInventory("GP5GasMask");
			RewardCar.GetInventory().CreateInInventory("NBCGlovesGray");
			RewardCar.GetInventory().CreateInInventory("WaterBottle");	
			RewardCar.GetInventory().CreateInInventory("SpaghettiCan");
			RewardCar.GetInventory().CreateInInventory("M18SmokeGrenade_Red");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Battery9V");				
		}
		if (selectedLoadout == 7)
		{			
			weapon = RewardCar.GetInventory().CreateInInventory("AK74");
				weapon.GetInventory().CreateAttachment("AK_RailHndgrd");
				weapon.GetInventory().CreateAttachment("AK74_WoodBttstck");	
				weapon.GetInventory().CreateAttachment("KashtanOptic");
				weapon.GetInventory().CreateAttachment("AK_Suppressor");			
			RewardCar.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			RewardCar.GetInventory().CreateInInventory("Headtorch_Grey");
			RewardCar.GetInventory().CreateInInventory("NBCBootsGray");
			RewardCar.GetInventory().CreateInInventory("Canteen");	
			RewardCar.GetInventory().CreateInInventory("TacticalBaconCan");
			RewardCar.GetInventory().CreateInInventory("Tomato");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Battery9V");				
		}
		if (selectedLoadout == 8)
		{			
			weapon = RewardCar.GetInventory().CreateInInventory("AKS74U");
				weapon.GetInventory().CreateAttachment("AKS74U_Bttstck");			
			RewardCar.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
			RewardCar.GetInventory().CreateInInventory("M67Grenade");
			RewardCar.GetInventory().CreateInInventory("M67Grenade");
			RewardCar.GetInventory().CreateInInventory("Matchbox");
			RewardCar.GetInventory().CreateInInventory("Canteen");	
			RewardCar.GetInventory().CreateInInventory("PortableGasStove");
			RewardCar.GetInventory().CreateInInventory("SmallGasCanister");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Battery9V");			
		}
		if (selectedLoadout == 9)
		{			
			weapon = RewardCar.GetInventory().CreateInInventory("Glock19");
				weapon.GetInventory().CreateAttachment("PistolSuppressor");			
			RewardCar.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			RewardCar.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			RewardCar.GetInventory().CreateInInventory("FishingRod");
			RewardCar.GetInventory().CreateInInventory("Carp");
			RewardCar.GetInventory().CreateInInventory("Hook");
			RewardCar.GetInventory().CreateInInventory("Worm");
			RewardCar.GetInventory().CreateInInventory("CombatKnife");
			RewardCar.GetInventory().CreateInInventory("FieldShovel");
			RewardCar.GetInventory().CreateInInventory("Canteen");	
			RewardCar.GetInventory().CreateInInventory("MackerelFilletMeat");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Battery9V");
			RewardCar.GetInventory().CreateInInventory("Battery9V");			
		}
					
		Print("[SMM] Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );
	}

	void SpawnAIs()
	{
		//nothing here
	}
	
	override void ObjDespawn()
	{
		// Remove all mission objects
	}

	void SecondWave()
	{
		int Dice1 = Math.RandomIntInclusive( 0, 9);  //move the dice to inside the for loop for the horde to spawn in circle around convoy
		int Dice2 = Math.RandomIntInclusive( 0, 9);
		for ( int j2 = 0; j2 < HordeDensity; j2++ )
		{
			//calc new spawn position
			float x = Math.RandomFloatInclusive( 20 , 65 );
			float y = Math.RandomFloatInclusive( 20 , 65 );	

			if ( Dice1 > 4 ) x *= -1.0;  
			if ( Dice2 < 5 ) y *= -1.0;
	
			vector NewPosVector = { x, 0, y };
			
			//Spawn infected
			m_MissionAIs.Insert( GetGame().CreateObject( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, false, true ));	
		}
	}

	override void MissionFinal()
	{	
		//Spawn horde
		int Dice1 = Math.RandomIntInclusive( 0, 9);  // Dice are outside the For loop so infected all spawn in same quadrant.
		int Dice2 = Math.RandomIntInclusive( 0, 9);
		for ( int j = 0; j < HordeDensity; j++ )
		{
			//calc new spawn position
			float x = Math.RandomFloatInclusive( 10 , 70 );
			float y = Math.RandomFloatInclusive( 10 , 70 );	

			if ( Dice1 > 4 ) x *= -1.0;  
			if ( Dice2 < 5 ) y *= -1.0;  

			vector NewPosVector = { x, 0, y };
			
			//Spawn infected
			EntityAI horde = EntityAI.Cast( GetGame().CreateObject( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, false, true ));	
			m_MissionAIs.Insert(horde);
		}
	}

	override void PlayerChecks( PlayerBase player )
	{	

	
		#ifdef ENFUSION_AI_PROJECT
		#ifdef EXPANSIONMODAI

		// Add player to AI group and set as leader if not already
		if (m_ConvoyAIGroup && player.GetGroup() != m_ConvoyAIGroup)
		{
			player.SetGroup(m_ConvoyAIGroup);
			Print("[SMM] Added player " + player.GetIdentity().GetName() + " to Convoy AI group.");
		}

		#endif
		#endif

		for ( int ix=0; ix < m_MissionAIs.Count(); ix++)
		{
			Object MissionAI = m_MissionAIs.Get(ix);
			if ( MissionAI && !MissionAI.IsAlive() )
			{
				if ( DeadInfected.Find( MissionAI ) > -1 )
				continue;
				else
				{
					DeadInfected.Insert( MissionAI );
				}
			}
		}

		//now we check if the array has the wanted amount of dead infected( I use 5 less than spawn in case a few get stuck in a rock)
		if ( DeadInfected.Count() >= 25 && !secondwavespawned)
		{
			Print("[SMM] The players in the mission zone successfully killed the first requested amount of infected.");
			SecondWave(); 
			secondwavespawned = true;
			string message = "That just about takes care of that...\nWatch Out!\nAnother Horde is incoming!";
			string icon = "set:dayz_inventory image:walkietalkie";            
			NotificationSystem.SendNotificationToPlayerIdentityExtended( player.GetIdentity() , 10.0, m_MissionInformant + ":", message, icon );
		}
		
		//now we check if the array has the wanted amount of dead infected ( I use 5 less than spawn in case a few get stuck in a rock)
		if ( DeadInfected.Count() >= 55 && !m_RewardsSpawned)
		{
			
			SpawnRewards();
			m_MissionObjects.Remove(0); //this removes vehicle from items to be deleted
			RewardCar.SetLifetime( 3888000 ); //this sets vehicle lifetime to 45 days rather than 40 min
			Print("[SMM] The players in the mission zone successfully killed the requested amount of infected.");
			m_RewardsSpawned = true;
			m_MsgNum = -1;
			m_MsgChkTime = m_MissionTime + MsgDlyFinish;
			string messageF = "It seem they found the USB, and Seem to be leaving you the Humvee!\nTake care of the remaining infected and then evac!";
			string messageFa = "The Survivor, said he found the USB in this pocket!!\nTake care of the remaining infected and then evac using the Humvee!";
			
			if(SpawnPatrolEnabled){NotificationSystem.SendNotificationToPlayerIdentityExtended( player.GetIdentity() , 10.0, m_MissionInformant + ":", messageF, icon );}
			else{NotificationSystem.SendNotificationToPlayerIdentityExtended( player.GetIdentity() , 10.0, m_MissionInformant + ":", messageFa, icon );}
			
			RewardCar.Fill(CarFluid.FUEL, 50);  //this puts in about a third of a tank of gas.
			RewardCar.Fill(CarFluid.OIL, 100);
			RewardCar.Fill(CarFluid.BRAKE, 100);
			RewardCar.Fill(CarFluid.COOLANT, 100);
		}
	}
		
	void UpdateBots(float dt)
	{
		//no bots involved in this mission		
	}

	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI

	// Replicates eAIDynamicPatrol's SetupAI() functionality
	void SafeConfigureAI(eAIBase ai, eAIGroup group, int lootingBehavior)
	{
		// 1. Ensure AI is properly grouped first
		if (ai.GetGroup() != group) {
			ai.SetGroup(group);
		}

		// 2. Basic movement settings (safe)
		ai.SetMovementSpeedLimits(1.5, 3.0);
		
		// 3. Inventory settings (safe)
		ai.Expansion_SetCanBeLooted(true);
		ai.eAI_SetUnlimitedReload(false);
		
		// 4. Combat settings that WON'T break faction
		ai.eAI_SetAccuracy(0.5, 0.8);
		ai.eAI_SetThreatDistanceLimit(300);
		
		// 5. Behavior settings (safe)
		ai.eAI_SetSniperProneDistanceThreshold(150);
		ai.eAI_SetLootingBehavior(lootingBehavior); // 1=DEFAULT, 2=ALL
		
		// AVOID these as they can affect faction behavior:
		// ai.eAI_SetDamageMultiplier();
		// ai.eAI_SetDamageReceivedMultiplier();
		// ai.eAI_SetNoiseInvestigationDistanceLimit();
	}

    // Creates a Patrol Route Around the Object... Maybe will Make better in future.
	void CreateCircularPatrolRoute(eAIGroup group, vector center, float radius)
	{
		group.AddWaypoint(center + Vector(radius, 0, 0));  // East
		group.AddWaypoint(center + Vector(0, 0, radius));  // North
		group.AddWaypoint(center + Vector(-radius, 0, 0)); // West
		group.AddWaypoint(center + Vector(0, 0, -radius)); // South
		group.SetWaypointBehaviour(eAIWaypointBehavior.LOOP);
	}

	eAIFaction CreateFaction(int factionType = 0) // 0=Raiders (default)
	{
		switch (factionType)
		{
			case 1:  return new eAIFactionBrawlers();
			case 2:  return new eAIFactionCivilian();
			case 3:  return new eAIFactionEast();
			case 4:  return new eAIFactionGuards();
			case 5:  return new eAIFactionMercenaries();
			case 6:  return new eAIFactionObservers();
			case 7:  return new eAIFactionPassive();
			case 8:  return new eAIFactionShamans();
			case 9:  return new eAIFactionWest();
			case 10: return new eAIFactionYeetBrigade();
			default: return new eAIFactionRaiders();
		}
		return new eAIFactionRaiders(); // This line is technically redundant but ensures safety
	}

	eAIFormation CreateFormation(int formationType = 0) // 0 = Vee (default)
	{
		switch (formationType) 
		{
			case 1:  Print("[SMM] Formation: Circle");      return new eAIFormationCircle();
			case 2:  Print("[SMM] Formation: CircleDot");   return new eAIFormationCircleDot();
			case 3:  Print("[SMM] Formation: Column");      return new eAIFormationColumn();
			case 4:  Print("[SMM] Formation: File");        return new eAIFormationFile();
			case 5:  Print("[SMM] Formation: InvColumn");   return new eAIFormationInvColumn();
			case 6:  Print("[SMM] Formation: InvFile");     return new eAIFormationInvFile();
			case 7:  Print("[SMM] Formation: InvVee");      return new eAIFormationInvVee();
			case 8:  Print("[SMM] Formation: Star");        return new eAIFormationStar();
			case 9:  Print("[SMM] Formation: StarDot");     return new eAIFormationStarDot();
			case 10: Print("[SMM] Formation: Wall");        return new eAIFormationWall();
			default: Print("[SMM] Formation: Vee");         return new eAIFormationVee();
		}
		return new eAIFormationVee();
	}

	string GetLoadout(int loadoutType = 1) // 1=Police (default)
	{
		switch (loadoutType)
		{
			case 0:  return "HumanLoadout.json";
			case 1:  return "PoliceLoadout.json";
			case 2:  return "FireFighterLoadout.json";
			case 3:  return "TTsKOLoadout.json";
			case 4:  return "GorkaLoadout.json";
			case 5:  return "NBCLoadout.json";
			case 6:  return "EastLoadout.json";
			case 7:  return "WestLoadout.json";
			case 8:  return "SurvivorLoadout.json";
			case 9:  return "BanditLoadout.json";
			case 10: return "PlayerSurvivorLoadout.json";
			case 11: return "PlayerMaleSuitLoadout.json";
			case 12: return "FemaleSuitLoadout.json";
			case 13: return "YellowKingLoadout.json";
			case 14: return "YeetBrigadeLoadout.json";
			default: return "PoliceLoadout.json"; // Fallback for invalid inputs
		}
		return "PoliceLoadout.json";
	}

	int GetRandomFactionType()
	{
		return Math.RandomInt(0, 10); // 0-10 (matches faction cases)
	}

	int GetRandomLoadoutType()
	{
		return Math.RandomInt(0, 14); // 0-14 (matches loadout cases)
	}

	int GetFormationType()
	{
		return Math.RandomInt(1, 10); // 0-14 (matches loadout cases)
	}

	int GetRandomSquadSize()
	{
		return Math.RandomInt(MIN_SQUAD_SIZE, MAX_SQUAD_SIZE);
	}

	void RemovePlayerFromConvoyGroup(PlayerBase player)
	{
		if (player && m_ConvoyAIGroup && player.GetGroup() == m_ConvoyAIGroup)
		{
			player.SetGroup(null);
			Print("[SMM] Removed player " + player.GetIdentity().GetName() + " from Convoy AI group.");
		}
	}

	// Spawn Expansion AI Patrol
	void SpawnPatrol()
	{
		// Initialize with "RANDOM" to trigger randomization (or use defaults)
		int factionType = 7;    // RANDOM will randomize unless 1-10 then DEFAULT_FACTION set at Top
		int loadoutType = 5;    // RANDOM will randomize unless 1-14 then DEFAULT_FACTION set at Top
		int squadSize = 3;      // RANDOM will randomize unless 1 or higher then DEFAULT_FACTION set at Top
		float patrolRadius = DEFAULT_PATROL_RADIUS; // Default radius set at Top
		int formationType = RANDOM;
		int spawnedCount = 1;

		if (factionType == RANDOM) 
			factionType = GetRandomFactionType(); 
		else if (factionType < 0 || factionType > 10) 
			factionType = DEFAULT_FACTION;

		if (loadoutType == RANDOM) 
			loadoutType = GetRandomLoadoutType(); 
		else if (loadoutType < 0 || loadoutType > 14) 
			loadoutType = DEFAULT_LOADOUT;

		if (formationType == RANDOM) 
			formationType = GetFormationType(); 
		else if (formationType < 0 || formationType > 10)
			formationType = DEFAULT_FORMATION;

		if (squadSize == RANDOM) 
			squadSize = GetRandomSquadSize(); 
		else if (squadSize < 1) 
			squadSize = DEFAULT_SQUAD_SIZE;

		if (patrolRadius <= 0) 
			patrolRadius = DEFAULT_PATROL_RADIUS;


		array<vector> spawnPositions = {
			m_MissionPosition + "3 0 3",
			m_MissionPosition + "-3 0 3",
			m_MissionPosition + "-3 0 -3",
			m_MissionPosition + "3 0 -3",
			m_MissionPosition + "4 0 4",
			m_MissionPosition + "-4 0 4",
			m_MissionPosition + "-4 0 -4",
			m_MissionPosition + "4 0 -4",
			m_MissionPosition + "5 0 5",
			m_MissionPosition + "-5 0 5",
			m_MissionPosition + "-5 0 -5",
			m_MissionPosition + "5 0 -5"
			//Can add more spawn points if need follow scheme.
		};

		eAIFaction faction = CreateFaction(factionType);
		string loadout = GetLoadout(loadoutType);

		// Spawn Leader
		eAIBase leader = eAIBase.Cast(GetGame().CreateObject(GetRandomAI(), spawnPositions[0]));

		if (!leader) {
			Print("[SMM] Failed to create leader AI");
			return;
		}

		// Create Group
		eAIGroup group = eAIGroup.GetGroupByLeader(leader, true, faction);
		if (!group) {
			Print("[SMM] Failed to create AI group");
			return;
		}

		// Config Leader
		ExpansionHumanLoadout.Apply(leader, loadout, false);
		m_MissionAIs.Insert(leader);
		SafeConfigureAI(leader, group, 1); // 1 = DEFAULT looting
		
		// Spawn Followers and Add them to group.
		for (int i = 1; i < squadSize; i++) 
		{
			eAIBase ai = eAIBase.Cast(GetGame().CreateObject(GetRandomAI(), spawnPositions[i]));
			if (ai) 
			{
				group.AddMember(ai);
				ExpansionHumanLoadout.Apply(ai, loadout, false);
				m_MissionAIs.Insert(ai);
				spawnedCount++;
				SafeConfigureAI(ai, group, 1);
			}
			else
			{
				Print("[SMM] FAILED to spawn bot at index " + i.ToString());
			}
		}

		// 6. Set up Patrol Route
		CreateCircularPatrolRoute(group, m_MissionPosition, 10); // 25m radius

		// Verify and Set Formation
		Print(string.Format("[SMM] Spawned patrol: %1/%2 AI (Leader: %3) Loadout:%4 Faction:%5", spawnedCount, squadSize, leader.GetType(), loadout, faction.GetName()));
		group.SetFormation(CreateFormation(formationType));

		m_ConvoyAIGroup = group;
		
	}

	#endif
	#endif
	
	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI

	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		if ( m_MissionPosition && m_MissionPosition != "0 0 0" )
		{
			//Call spawners	
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			//GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );

			if (SpawnPatrolEnabled)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnPatrol);
			}
			else
			{
				Print("[SMM] Expansion AI Patrol Turned Off");	
			}

			SecondWave();
			return true;		
		}
		else 
		{
			Print("[SMM] ERROR : Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}

	#endif
	#endif
}

