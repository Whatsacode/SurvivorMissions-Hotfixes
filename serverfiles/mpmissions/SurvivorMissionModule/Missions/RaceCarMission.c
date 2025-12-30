class RaceCarMission extends SurvivorMissions
{
	// Activate Expansion AI Patrol
	bool SpawnPatrolEnabled = true;
	
	// Debug settings
	bool m_DebugMode = false; // Set to true to enable debug print messages

	// Expansion AI Globals
	const int MIN_SQUAD_SIZE = 2;   // Minimum squad size when randomized
	const int MAX_SQUAD_SIZE = 8;   // Maximum squad size when randomized
	const int RANDOM = -1;
	const int DEFAULT_FACTION = 0;    // Raiders
	const int DEFAULT_LOADOUT = 1;    // Police
	const int DEFAULT_SQUAD_SIZE = 4; // Default squad size
    const int DEFAULT_FORMATION = 0; // Default Formation: Vee
	const float DEFAULT_PATROL_RADIUS = 25.0; // Default patrol radius
	int numAIGroups = 4; // Number of AI groups to spawn (3-4 groups)
	int BotDlyTime = 600;// 5min delay before first patrol spawn

	//Mission parameters
	int MsgDlyFinish = 300;					//seconds, message delay time after player has finished mission
	int carCount = 1;

	//Mission variables 
	string SurvivorName;
	PlayerBase m_Player;
	PlayerBase m_WinningPlayer = null; // Track the winning player
	ref array<vector> carStartPoint = new array<vector>;
	ref array<ref Param3<string,vector,vector>> BuildingSpawns = new array<ref Param3<string,vector,vector>>;
	ref array<vector> m_CheckpointPositions = new array<vector>();
	ref array<string> InfectedTypes = new array<string>;
	ref array<Car> m_SpawnedCars = new array<Car>();
    int m_CurrentCheckpointIndex = 0;
    Object m_CurrentCheckpointSmoke;
	Object m_roadflare;
	
	// Timer variables
	bool m_RaceStarted = false;
	bool m_CountdownActive = false;
	int m_CurrentWarningIndex = 0;
	bool m_EntityCheckActive = false; // Track if entity checking is active
	//////
	int m_CountdownTime = 600; // Start with 600 seconds (10 minutes)
	//ref array<int> m_WarningTimes = {60, 30, 10, 3, 2}; // Times to show warnings: 10min, 1min, 30sec, 10sec, 3sec, 2sec

	ref array<int> m_WarningTimes = {600, 60, 30, 10, 3, 2}; // Times to show warnings: 10min, 1min, 30sec, 10sec, 3sec, 2sec
	
	
	// Lap system variables
	int m_TotalLaps = 1; // Total number of laps required to win
	int m_CurrentLap = 1; // Current lap number (start at 1, not 0)
	float m_CheckpointRadius = 20.0; // Set your desired radius here

	int numGroups = 8; // Number of infected groups to spawn
	int infectedPerGroup = 0; // Default Value (Dont Change!!!)
	int infectedmin = 10; // Minimum infected per group
	int infectedmax = 20; // Maximum infected per group

	override bool IsExtended() return false;
	
	void RaceCarMission()
	{
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time
		
		//Mission zones
		m_MissionZoneOuterRadius = 170.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 0.0001;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		TStringArray SurvivorNames = {"Yuri", "Michail", "Boris", "Valeri", "Anatoli", "Ivan", "Alexej", "Dimitrij", "Sergej", "Nikolai"};
		SurvivorName = SurvivorNames.GetRandomElement();
		
		//Set mission messages
		m_MissionMessage1 = "My friend " + SurvivorName + " is a car enthusiast. He misses the thrill of watching a race,\nand now he has a challenge for all who dare!";
		m_MissionMessage2 = m_MissionLocationDir + " of " + m_MissionLocation + "\nRace your car through " + m_TotalLaps + " laps, making sure to hit every checkpoint each lap.\nThe first one to complete " + m_TotalLaps + " laps wins!";
		m_MissionMessage3 = "If you don't have a car, you can take one from the starting point.\nComplete the full circuit " + m_TotalLaps + " times to win.\nGood luck!";

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


		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_BusStop_Village2", "-7.7 1 -0.7", "-5 0 0")); 
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_BusStop_Village2", "-7.0 1 -6.0", "-5 0 0")); 
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_BusStop_Village2", "-8.0 1 4.6", "-5 0 0")); 
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_BusStop_Village2", "6.9 1 -6.7", "176 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_BusStop_Village2", "6.3 1 3.9", "176 0 0")); 
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_BusStop_Village2", "6.4 1 -1.4", "176 0 0")); 

		// Car start points
		carStartPoint.Insert("5.7 0 -2.6");		carStartPoint.Insert("-5.5 0 2.1");
		carStartPoint.Insert("0 0 2.2");		carStartPoint.Insert("0 0 -2.2");
		carStartPoint.Insert("5.5 0 2.1");		carStartPoint.Insert("-5.7 0 -2.6");


		if ( m_MissionLocation == "Balota Airfield" )
		{
			// Insert checkpoint positions
			m_CheckpointPositions.Insert("5486.97 6.8858 2065.28");
			m_CheckpointPositions.Insert("5580.93 7.29655 2084.41");
			m_CheckpointPositions.Insert("5684.01 7.68408 2119.8");
			m_CheckpointPositions.Insert("5812.11 8.97487 2161.29");
			m_CheckpointPositions.Insert("5979.19 9.98019 2214.71");
			m_CheckpointPositions.Insert("6109.54 11.8543 2250.91");
			m_CheckpointPositions.Insert("6189.32 11.4335 2331.84");
			m_CheckpointPositions.Insert("6168.19 11.1049 2380.67");
			m_CheckpointPositions.Insert("6092.21 21.6604 2394.45");
			m_CheckpointPositions.Insert("6027.01 43.3178 2443.47");
			m_CheckpointPositions.Insert("5934.7 58.1366 2464.37");
			m_CheckpointPositions.Insert("5863.71 59.3702 2443.4");
			m_CheckpointPositions.Insert("5730.92 57.9401 2415.91");
			m_CheckpointPositions.Insert("5634.4 61.2566 2406.55");
			m_CheckpointPositions.Insert("5544.31 62.7074 2507.24");
			m_CheckpointPositions.Insert("5494.91 63.2322 2581.32");
			m_CheckpointPositions.Insert("5410.26 60.6317 2657.26");
			m_CheckpointPositions.Insert("5331.2 56.803 2726.44");
			m_CheckpointPositions.Insert("5399.65 50.4105 2837.91");
			m_CheckpointPositions.Insert("5355.07 39.5048 2920.73");
			m_CheckpointPositions.Insert("5253.17 30.9033 2888.03");
			m_CheckpointPositions.Insert("5135.35 24.3958 2882.24");
			m_CheckpointPositions.Insert("5028.74 29.8711 2890.14");
			m_CheckpointPositions.Insert("4891.89 25.4488 2802.57");
			m_CheckpointPositions.Insert("4779.53 18.4462 2763.71");
			m_CheckpointPositions.Insert("4610.88 7.17765 2721.69");
			m_CheckpointPositions.Insert("4552.96 9.81575 2633.87");
			m_CheckpointPositions.Insert("4517.76 9.22841 2548.7");
			m_CheckpointPositions.Insert("4514.84 5.50516 2448.44");
			m_CheckpointPositions.Insert("4640.74 6.82498 2427.01");
			m_CheckpointPositions.Insert("4819.54 5.96449 2339.54");
			m_CheckpointPositions.Insert("4954.43 4.92495 2249.91");
			m_CheckpointPositions.Insert("5018.25 5.36847 2209.23");
			m_CheckpointPositions.Insert("5082.52 5.82495 2137.49");
			m_CheckpointPositions.Insert("5289.00 5.62498 2087.64");
			m_CheckpointPositions.Insert("5371.6 5.77707 2066.72");
			m_CheckpointPositions.Insert("5415.73 6.16801 2061.58"); // FINISH LINE CHECKPOINT
			// Can always add more checkpoints!! or Follow above to create a new Full Circuit for a new m_MissionLocation!!!!
		}
		if ( m_MissionLocation == "Pulkovo" )
		{
			// Insert checkpoint positions
			m_CheckpointPositions.Insert("4678 321.714 5704.75");
			m_CheckpointPositions.Insert("4749.28 321.614 5699.07");
			m_CheckpointPositions.Insert("4817.46 321.683 5670.07");
			m_CheckpointPositions.Insert("4878.83 320.247 5646.1");
			m_CheckpointPositions.Insert("4914.26 319.265 5697.03");
			m_CheckpointPositions.Insert("4855.97 317.649 5730.31");
			m_CheckpointPositions.Insert("4824.87 312.625 5811.93");
			m_CheckpointPositions.Insert("4751.8 304.519 5948.03");
			m_CheckpointPositions.Insert("4746.08 295.44 6017.22");
			m_CheckpointPositions.Insert("4760.68 289.049 6082.09");
			m_CheckpointPositions.Insert("4716.73 284.773 6171.67");
			m_CheckpointPositions.Insert("4721.3 284.387 6267.01");
			m_CheckpointPositions.Insert("4720.2 283.758 6338.01");
			m_CheckpointPositions.Insert("4672.19 281.866 6361.72");
			m_CheckpointPositions.Insert("4606.69 282.319 6416.66");
			m_CheckpointPositions.Insert("4537.85 289.127 6432.82");
			m_CheckpointPositions.Insert("4459.95 296.54 6414.02");
			m_CheckpointPositions.Insert("4401.74 302.836 6375.23");
			m_CheckpointPositions.Insert("4403.5 307.161 6268.37");
			m_CheckpointPositions.Insert("4398.2 309.539 6118.97");
			m_CheckpointPositions.Insert("4396.1 311.117 6001.06");
			m_CheckpointPositions.Insert("4388.85 314.463 5888.17");
			m_CheckpointPositions.Insert("4395.79 314.582 5780.41");
			m_CheckpointPositions.Insert("4387.99 317.803 5709.12");
			m_CheckpointPositions.Insert("4397.37 321.263 5650.63");
			m_CheckpointPositions.Insert("4450.2 320.863 5669.78");
			m_CheckpointPositions.Insert("4508.23 320.149 5687.27");
			m_CheckpointPositions.Insert("4593.81 320.186 5694.96");
			m_CheckpointPositions.Insert("4617.77 320.648 5697.45"); // FINISH LINE CHECKPOINT
			// Can always add more checkpoints!! or Follow above to create a new Full Circuit for a new m_MissionLocation!!!!
		}
		else
		{
			Print("[SMM] ERROR: m_MissionLocation is not set to TEST when spawning checkpoints!");
			return;
		}

		// Initialize timer variables
		m_RaceStarted = false;
		m_CountdownActive = false;
		m_CountdownTime = 600; // 10 minutes
		m_CurrentWarningIndex = 0;
		m_EntityCheckActive = false;
		m_WinningPlayer = null;
		m_CurrentLap = 1; // Start at lap 1, not 0
		m_CurrentCheckpointIndex = 0; // Start at checkpoint 0
		
	}

	void ~RaceCarMission()
	{
		if (m_CountdownActive)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.CountdownTick);
			m_CountdownActive = false;
		}
		
		if (m_EntityCheckActive)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.EntityChecks);
			m_EntityCheckActive = false;
		}
		
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{
				if ( !m_MissionObjects.Get(i) ) continue;
				else
				{
					//Delete Object clientside first
					if ( m_MissionObjects.Get(i).GetType() == "ClutterCutterFireplace" )
					GetGame().RemoteObjectDelete( m_MissionObjects.Get(i) );				
					
					//Delete Object serverside
					GetGame().ObjectDelete( m_MissionObjects.Get(i) );
				}		
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

	void SpawnNextCheckpoint()
    {
		// Clean up previous checkpoint
		GetGame().ObjectDelete(m_roadflare);
		m_roadflare = null; // Clear reference to road flare
	
		// Check if we have valid checkpoints and current index is within range
		if (m_CheckpointPositions.Count() == 0)
		{
			Print("[SMM] ERROR: No checkpoint positions available!");
			return;
		}
		
		if (m_CurrentCheckpointIndex < m_CheckpointPositions.Count())
		{
			vector pos = m_CheckpointPositions.Get(m_CurrentCheckpointIndex);
			m_roadflare = GetGame().CreateObject("Roadflare", pos);
			if (m_roadflare)
			{
				m_roadflare.PlaceOnSurface();
				Roadflare flare = Roadflare.Cast(m_roadflare);
				if (flare)
					flare.SetBurningStateSynchronized(RoadflareBurningState.INITIAL_BURN);
				m_MissionObjects.Insert(m_roadflare);
				//Print("[SMM] Spawned checkpoint " + (m_CurrentCheckpointIndex + 1).ToString() + " of " + m_CheckpointPositions.Count().ToString() + " at position: " + pos.ToString());
			}
			else
			{
				Print("[SMM] ERROR: Failed to create roadflare at checkpoint " + m_CurrentCheckpointIndex.ToString());
			}
		}
		else
		{
			Print("[SMM] ERROR: Invalid checkpoint index " + m_CurrentCheckpointIndex.ToString() + ", total checkpoints: " + m_CheckpointPositions.Count().ToString());
		}
    }
	
	void OnLapCompleted()
	{
		//Print("[SMM] Lap " + m_CurrentLap.ToString() + " of " + m_TotalLaps.ToString() + " completed!");
		
		// Check if all laps are completed
		if (m_CurrentLap >= m_TotalLaps)
		{
			// Race is finished
			OnRaceFinished();
		}
		else
		{
			// Start next lap
			m_CurrentLap++;
			m_CurrentCheckpointIndex = 0; // Reset checkpoint index for next lap
			
			//Print("[SMM] Starting lap " + m_CurrentLap.ToString() + " of " + m_TotalLaps.ToString());
			
			// Spawn first checkpoint of next lap
			SpawnNextCheckpoint();
		}
	}
	
	void SpawnObjects()
	{	

		m_MissionMessage1 = "The Race will begin in 10 minutes! Please Get to the Starting Location "+ m_MissionLocationDir +" of "+ m_MissionLocation + "";
		m_MissionMessage2 = "You can bring your own Car to participate, If you dont have a car, you can take one from the starting point.";		
		m_MissionMessage3 = "Complete the full circuit " + m_TotalLaps + " times to win.\nGood luck!";

		m_MsgNum = 1;
		MsgCutoff = false;
		

		Object SmokeGrenade;
		int CarOriConversion;
		string CarOrientation;
		m_CurrentCheckpointIndex = 0; // Reset checkpoint index to start at first checkpoint
		m_CurrentLap = 1; // Ensure lap starts at 1
		
		CarOriConversion = 0 + 90; // Adjusted for RaceCarMission
		if ( CarOriConversion < 0 ) CarOriConversion += 360;
		CarOrientation = CarOriConversion.ToString() +" 0 0";
	
		if (!m_MissionPosition || m_MissionPosition == "0 0 0") 
		{
			Print("[SMM] ERROR: m_MissionPosition is not set or invalid when spawning SmokeGrenade!");
			return;
		}
		SmokeGrenade = GetGame().CreateObject("M18SmokeGrenade_Green", m_MissionPosition);
		SmokeGrenade.SetOrientation(CarOrientation.ToVector());
		SmokeGrenade.PlaceOnSurface();
		m_MissionObjects.Insert(SmokeGrenade);
		M18SmokeGrenade_Green smoke = M18SmokeGrenade_Green.Cast(SmokeGrenade);

		//Spawn additional flag base buildings and objects
		for ( int r=0; r < BuildingSpawns.Count(); r++ )
		{
			Param3<string,vector,vector> BuildingDef = BuildingSpawns.Get(r);
			string SM_BuildingType = BuildingDef.param1;
			vector BuildingPos = SmokeGrenade.ModelToWorld( BuildingDef.param2 );
			vector BuildingOri = BuildingDef.param3;
			vector BuildingDir = SmokeGrenade.GetDirection();
			
			BuildingPos[1] = GetGame().SurfaceY( BuildingPos[0], BuildingPos[2] );
			Object HQextra = GetGame().CreateObject( SM_BuildingType, BuildingPos );
			HQextra.SetOrientation(BuildingOri);
			HQextra.PlaceOnSurface();
			m_MissionObjects.Insert( HQextra );											
		}

		if (carStartPoint.Count() < carCount)
		{
			Print("[SMM] Not enough car start points defined. Required: " + carCount + ", Available: " + carStartPoint.Count());
			return;
		}
		for (int i = 0; i < carCount; i++)
		{
			vector offset = carStartPoint.Get(0); 
			carStartPoint.Remove(0);               

			vector carPos = m_MissionPosition + offset;
			Car car = Car.Cast(GetGame().CreateObject("OffroadHatchback", carPos, false, true, true));
			if (car)
			{
				for (int j = 0; j < 4; j++) car.GetInventory().CreateAttachment("HatchbackWheel");
				car.GetInventory().CreateAttachment("CarBattery");
				car.GetInventory().CreateAttachment("SparkPlug");
				car.GetInventory().CreateAttachment("CarRadiator");
				car.GetInventory().CreateAttachment("HatchbackTrunk");
				car.GetInventory().CreateAttachment("HatchbackHood");
				car.GetInventory().CreateAttachment("HatchbackDoors_Driver");
				car.GetInventory().CreateAttachment("HatchbackDoors_CoDriver");
				car.GetInventory().CreateAttachment("HatchbackDoors_BackLeft");
				car.GetInventory().CreateAttachment("HatchbackDoors_BackRight");
				car.Fill(CarFluid.FUEL, car.GetFluidCapacity(CarFluid.FUEL));
				car.Fill(CarFluid.OIL, car.GetFluidCapacity(CarFluid.OIL));
				car.Fill(CarFluid.COOLANT, car.GetFluidCapacity(CarFluid.COOLANT));
				car.SetLifetime((m_MissionTimeout - m_MissionTime) + 30);
				car.SetOrientation(CarOrientation.ToVector());
				car.PlaceOnSurface();
				m_MissionObjects.Insert(car);
				m_SpawnedCars.Insert(car);
			}
		}
		Print("[SMM] Spawned " + carCount + " cars at mission position: " + m_MissionPosition.ToString());
		
		// Start the countdown timer instead of spawning checkpoint immediately
		StartRaceCountdown();
	}
	
	// Timer Functions
	void StartRaceCountdown()
	{
		if (m_CountdownActive) return; // Prevent multiple timers
		
		m_CountdownActive = true;
		m_CountdownTime = 600; // 10 minutes
		m_CurrentWarningIndex = 0;
		
		// Send initial message to all players in zone
		SendCountdownMessage("Race will begin in 10 minutes! Get ready!");
		
		// Start the countdown timer
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.CountdownTick, 1000, true);
	}
	
	void CountdownTick()
	{
		if (!m_CountdownActive) return;
		
		m_CountdownTime--;
		
		//Print("[SMM] Countdown tick: " + m_CountdownTime.ToString() + " seconds remaining");
		
		// Check if we need to show a warning for the current countdown time
		for (int i = 0; i < m_WarningTimes.Count(); i++)
		{
			int warningTime = m_WarningTimes.Get(i);
			if (m_CountdownTime == warningTime)
			{
				string message = GetCountdownMessage(warningTime);
				SendCountdownMessage(message);
				//Print("[SMM] Sent warning message for " + warningTime.ToString() + " seconds");
				break; // Only send one message per tick
			}
		}
		
		// Check if countdown is finished
		if (m_CountdownTime <= 0)
		{
			// Stop the timer
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.CountdownTick);
			m_CountdownActive = false;
			
			// Start the race
			StartRace();
		}
	}
	
	string GetCountdownMessage(int timeLeft)
	{
		string message = "";
		
		switch (timeLeft)
		{
			case 600:
				message = "Race will begin in 10 minutes! Get ready!";
				break;
			case 60:
				message = "Race starts in 1 minute! Get to your vehicles!";
				break;
			case 30:
				message = "Race starts in 30 seconds! Prepare for launch!";
				break;
			case 10:
				message = "Race starts in 10 seconds!";
				break;
			case 3:
				message = "3...";
				break;
			case 2:
				message = "2...";
				break;
			default:
				message = "Race starts in " + timeLeft.ToString() + " seconds!";
				break;
		}
		
		return message;
	}
	
	void SendCountdownMessage(string message)
	{
		// Send notification directly to all players in the mission zone
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase player = PlayerBase.Cast(players.Get(i));
			if (player && player.GetIdentity())
			{
				string uid = player.GetIdentity().GetPlainId();
				// Check if this player is in the mission zone
				if (m_PlayersInZone.Find(uid) != -1)
				{
					NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 10.0, m_MissionInformant, message, "set:dayz_gui image:walkie_talkie");
					//Print("[SMM] Sent countdown message to " + player.GetIdentity().GetName() + ": " + message);
				}
			}
		}
	}
	
	void StartRace()
	{
		m_RaceStarted = true;
		SendCountdownMessage("GO! GO! GO! The race has started!");
		
		Print("[SMM] Starting race - Current lap: " + m_CurrentLap.ToString() + ", Total checkpoints: " + m_CheckpointPositions.Count().ToString() + ", Starting at checkpoint index: " + m_CurrentCheckpointIndex.ToString());
		
		// Spawn the first checkpoint now that the race has started
		SpawnNextCheckpoint();
		
		if (m_EntityCheckActive) return; // Prevent multiple timers
		
		m_EntityCheckActive = true;

		// Spawn infected groups around checkpoint positions
		SpawnInfectedGroups();

		#ifdef ENFUSION_AI_PROJECT
		#ifdef EXPANSIONMODAI
		
		// Spawn AI patrol groups around checkpoint positions
		SpawnAIGroups();

		#endif
		#endif
		
		// Start the entity checking timer - runs every 1 second, calling EntityChecks directly
		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this.EntityChecks, 1000, true);
		
		Print("[SMM] Race initialization complete - Entity checking started");
	}
	
	void SpawnAIs(){}

	override void ObjDespawn(){}

	override void MissionFinal(){}

	override void PlayerChecks( PlayerBase player ){}

	void EntityChecks( PlayerBase player )
	{
		// Only allow checkpoint progression if the race has started
		if (!m_RaceStarted || !m_EntityCheckActive) return;
		
		// Check all players on the server since race participants will leave the mission zone
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase currentPlayer = PlayerBase.Cast(players.Get(i));
			if (currentPlayer && currentPlayer.IsAlive())
			{
				// Make sure we have a valid checkpoint to check
				if (m_CurrentCheckpointIndex < m_CheckpointPositions.Count())
				{
					vector checkpointPos = m_CheckpointPositions.Get(m_CurrentCheckpointIndex);
					float distance = vector.Distance(currentPlayer.GetPosition(), checkpointPos);

					if (distance <= m_CheckpointRadius)
					{
						string playerName = "Unknown";
						if (currentPlayer.GetIdentity())
							playerName = currentPlayer.GetIdentity().GetName();
							
						//Print("[SMM] Player " + playerName + " reached checkpoint " + (m_CurrentCheckpointIndex + 1).ToString() + " of " + m_CheckpointPositions.Count().ToString() + " in lap " + m_CurrentLap.ToString());
						
						// Move to next checkpoint
						m_CurrentCheckpointIndex++;
						
						// Check if this was the last checkpoint in the current lap
						if (m_CurrentCheckpointIndex >= m_CheckpointPositions.Count())
						{
							// Store the player who completed this lap
							m_WinningPlayer = currentPlayer;
							
							Print("[SMM] All checkpoints completed by " + playerName + " in lap " + m_CurrentLap.ToString());
							
							// Call lap completion handler
							OnLapCompleted();
						}
						else
						{
							// Spawn next checkpoint
							SpawnNextCheckpoint();
						}
						
						// Break out of the loop since checkpoint was reached
						break;
					}
				}
			}
		}
	}

    void OnRaceFinished()
    {
		if (m_EntityCheckActive)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.EntityChecks);
			m_EntityCheckActive = false;
		}

		// Spawn fireworks launchers for celebration (but don't ignite yet)
		SpawnFireworksLaunchers();

        // Clean up last smoke
        if (m_CurrentCheckpointSmoke)
        {
			GetGame().ObjectDelete(m_roadflare);
			m_roadflare = null; // Clear reference to road flare
        }

		// Spawn reward in the winning player's car
		if (m_WinningPlayer)
		{
			// Find the winning car among race participants
			Car winningCar = null;
			
			// Check if player is currently in a car and is the driver
			Transport vehicle = m_WinningPlayer.GetParent();
			if (vehicle && vehicle.IsKindOf("Car") && vehicle.CrewMemberIndex(m_WinningPlayer) == 0)
			{
				winningCar = Car.Cast(vehicle);
			}
			
			// If not in a car as driver, find the nearest spawned mission car
			if (!winningCar)
			{
				float nearestDistance = 50.0; // Maximum search radius
				for (int i = 0; i < m_SpawnedCars.Count(); i++)
				{
					Car car = m_SpawnedCars.Get(i);
					if (car)
					{
						float distance = vector.Distance(m_WinningPlayer.GetPosition(), car.GetPosition());
						if (distance < nearestDistance)
						{
							nearestDistance = distance;
							winningCar = car;
						}
					}
				}
			}
			
			if (winningCar)
			{
				string playerName = "Unknown";
				if (m_WinningPlayer.GetIdentity())
					playerName = m_WinningPlayer.GetIdentity().GetName();
					
				Print("[SMM] Race winner: " + playerName + " driving " + winningCar.GetType());
				
				// Place reward in car inventory
				if (winningCar.GetInventory().CreateInInventory("M4A1"))
				{
					winningCar.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
					winningCar.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
					winningCar.GetInventory().CreateInInventory("M65Jacket_Black");
					winningCar.GetInventory().CreateInInventory("CombatBoots_Black");
					winningCar.GetInventory().CreateInInventory("ProtectiveGloves_Black");
					Print("[SMM] Reward spawned in " + playerName + "'s car inventory!");
				}
				else
				{
					// Car inventory full, try player inventory
					if (m_WinningPlayer.GetInventory().CreateInInventory("M4A1"))
					{
						m_WinningPlayer.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
						m_WinningPlayer.GetInventory().CreateInInventory("M65Jacket_Black");
						Print("[SMM] Reward spawned in " + playerName + "'s player inventory!");
					}
					else
					{
						Print("[SMM] Both car and player inventories are full - no reward spawned!");
					}
				}

				// Notify the winner
				if (m_WinningPlayer.GetIdentity())
				{
					NotificationSystem.SendNotificationToPlayerIdentityExtended(m_WinningPlayer.GetIdentity(), 15.0, "Race Official", "Congratulations " + playerName + "! You won the race! Check your car for your reward.", "set:dayz_gui image:walkie_talkie");
				}
			}
			else
			{
				// No car found, spawn in player inventory only
				string uplayerName = "Unknown";
				if (m_WinningPlayer.GetIdentity())
					uplayerName = m_WinningPlayer.GetIdentity().GetName();
					
				if (m_WinningPlayer.GetInventory().CreateInInventory("M4A1"))
				{
					m_WinningPlayer.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
					m_WinningPlayer.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
					m_WinningPlayer.GetInventory().CreateInInventory("M65Jacket_Black");
					Print("[SMM] Reward spawned in " + uplayerName + "'s player inventory!");
				}
				else
				{
					Print("[SMM] Player inventory is full - no reward spawned!");
				}
				
				// Notify the winner
				if (m_WinningPlayer.GetIdentity())
				{
					NotificationSystem.SendNotificationToPlayerIdentityExtended(m_WinningPlayer.GetIdentity(), 15.0, "Race Official", "Congratulations " + playerName + "! You won the race! Check your inventory for your reward.", "set:dayz_gui image:walkie_talkie");
				}
			}
		}

        // Notify all players about the race winner
		string winnerName = "Unknown";
		if (m_WinningPlayer && m_WinningPlayer.GetIdentity())
			winnerName = m_WinningPlayer.GetIdentity().GetName();
			
		//Print("[SMM] Race finished! Winner: " + winnerName + " after " + m_TotalLaps.ToString() + " laps");

        Print("Race completed!");
		
		// Ignite fireworks for celebration!
		IgniteFireworks();

		//Finish mission
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;
    }
	
	// Infected spawning function
	void SpawnInfectedGroups()
	{
		if (m_CheckpointPositions.Count() == 0)
		{
			//Print("[SMM] No checkpoint positions available for infected spawning!");
			return;
		}
		
		// Check if we have enough checkpoints to skip first 3 and last 3
		if (m_CheckpointPositions.Count() <= 6)
		{
			//Print("[SMM] Not enough checkpoints for infected spawning (need more than 6, have " + m_CheckpointPositions.Count() + ")");
			return;
		}
		
		// Create array of valid checkpoint positions (skip first 3 and last 3)
		ref array<vector> validCheckpoints = new array<vector>();
		for (int cp = 3; cp < m_CheckpointPositions.Count() - 3; cp++)
		{
			validCheckpoints.Insert(m_CheckpointPositions.Get(cp));
		}
		
		//Print("[SMM] Using " + validCheckpoints.Count() + " valid checkpoints for infected spawning (skipping first 3 and last 3)");
		
		for (int g = 0; g < numGroups; g++)
		{

			int infectedPerGroup = Math.RandomIntInclusive(infectedmin, infectedmax);
			// Get random checkpoint position from valid checkpoints only
			vector spawnPos = validCheckpoints.GetRandomElement();
			
			// Offset the spawn position slightly to avoid spawning directly on checkpoint
			spawnPos[0] = spawnPos[0] + Math.RandomFloatInclusive(-20.0, 20.0);
			spawnPos[2] = spawnPos[2] + Math.RandomFloatInclusive(-20.0, 20.0);
			spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
			
			//Print("[SMM] Spawning infected group " + (g + 1) + " at position: " + spawnPos.ToString());
			
			// Spawn infected in a small area around the position
			for (int i = 0; i < infectedPerGroup; i++)
			{
				vector infectedPos = spawnPos;
				infectedPos[0] = infectedPos[0] + Math.RandomFloatInclusive(-5.0, 5.0);
				infectedPos[2] = infectedPos[2] + Math.RandomFloatInclusive(-5.0, 5.0);
				infectedPos[1] = GetGame().SurfaceY(infectedPos[0], infectedPos[2]);
				
				// Get random infected type
				string infectedType = InfectedTypes.GetRandomElement();
				
				// Spawn the infected
				ZombieBase infected = ZombieBase.Cast(GetGame().CreateObject(infectedType, infectedPos, false, true, true));
				if (infected)
				{
					infected.PlaceOnSurface();
					m_MissionAIs.Insert(infected);
					//Print("[SMM] Spawned " + infectedType + " at " + infectedPos.ToString());
				}
			}
		}
		
		//Print("[SMM] Spawned " + numGroups + " infected groups around checkpoint positions");
	}
	
	// Fireworks spawning function
	void SpawnFireworksLaunchers()
	{
		//Print("[SMM] Spawning fireworks launchers for race celebration!");
		
		// Find bus stops and place fireworks behind them using ModelToWorld
		for (int i = 0; i < m_MissionObjects.Count(); i++)
		{
			Object obj = m_MissionObjects.Get(i);
			if (obj && obj.GetType() == "Land_BusStop_Village2")
			{
				// Place fireworks 3 meters behind each bus stop
				vector fireworksOffset = Vector(0, 0, 3.0); // 3 meters back (negative Z)
				vector fireworksPos = obj.ModelToWorld(fireworksOffset);
				fireworksPos[1] = GetGame().SurfaceY(fireworksPos[0], fireworksPos[2]);
				
				FireworksLauncher fireworks = FireworksLauncher.Cast(GetGame().CreateObject("FireworksLauncher", fireworksPos));
				if (fireworks)
				{
					fireworks.PlaceOnSurface();
					// Orient fireworks same direction as the bus stop
					fireworks.SetOrientation(obj.GetOrientation());
					m_MissionObjects.Insert(fireworks);
					//Print("[SMM] Spawned fireworks launcher behind bus stop at position: " + fireworksPos.ToString());
				}
			}
		}
		
		//Print("[SMM] Fireworks launchers spawned behind all bus stops");
	}
	
	// Function to ignite all fireworks launchers
	void IgniteFireworks()
	{
		//Print("[SMM] Igniting fireworks for race celebration!");
		
		int fireworksCount = 0;
		for (int i = 0; i < m_MissionObjects.Count(); i++)
		{
			Object obj = m_MissionObjects.Get(i);
			if (obj && obj.IsKindOf("FireworksLauncher"))
			{
				FireworksLauncher fireworks = FireworksLauncher.Cast(obj);
				if (fireworks)
				{
					// Ignite the fireworks using the correct method from FireworksBase
					fireworks.OnIgnitedThis(null);
					fireworksCount++;
					//Print("[SMM] Ignited fireworks launcher " + fireworksCount);
				}
			}
		}
		
		//Print("[SMM] Successfully ignited " + fireworksCount + " fireworks launchers!");
	}
	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI

	// AI patrol groups spawning function
	void SpawnAIGroups()
	{

		if (m_CheckpointPositions.Count() == 0)
		{
			//Print("[SMM] No checkpoint positions available for AI group spawning!");
			return;
		}
		
		// Check if we have enough checkpoints to skip first 3 and last 3
		if (m_CheckpointPositions.Count() <= 6)
		{
			//Print("[SMM] Not enough checkpoints for AI group spawning (need more than 6, have " + m_CheckpointPositions.Count() + ")");
			return;
		}
		
		// Create array of valid checkpoint positions (skip first 3 and last 3)
		ref array<vector> validCheckpoints = new array<vector>();
		for (int cp = 3; cp < m_CheckpointPositions.Count() - 3; cp++)
		{
			validCheckpoints.Insert(m_CheckpointPositions.Get(cp));
		}
		
		//Print("[SMM] Using " + validCheckpoints.Count() + " valid checkpoints for AI group spawning (skipping first 3 and last 3)");
		
		for (int g = 0; g < numAIGroups; g++)
		{
			int aiSquadSize = Math.RandomIntInclusive(MIN_SQUAD_SIZE, MAX_SQUAD_SIZE);
			
			// Get random checkpoint position from valid checkpoints only
			vector spawnPos = validCheckpoints.GetRandomElement();
			
			// Offset the spawn position to avoid spawning directly on checkpoint
			spawnPos[0] = spawnPos[0] + Math.RandomFloatInclusive(-30.0, 30.0);
			spawnPos[2] = spawnPos[2] + Math.RandomFloatInclusive(-30.0, 30.0);
			spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
			
			//Print("[SMM] Spawning AI group " + (g + 1) + " at position: " + spawnPos.ToString());
			
			// Set faction to Shaman, randomize loadout and formation
			int factionType = 8; // 8 = Shaman faction
			int loadoutType = GetRandomLoadoutType();
			int formationType = GetFormationType();
			
			eAIFaction faction = CreateFaction(factionType);
			string loadout = GetLoadout(loadoutType);
			
			// Create spawn positions for the squad members in a small area
			ref array<vector> squadSpawnPositions = new array<vector>();
			for (int s = 0; s < aiSquadSize; s++)
			{
				vector memberPos = spawnPos;
				memberPos[0] = memberPos[0] + Math.RandomFloatInclusive(-5.0, 5.0);
				memberPos[2] = memberPos[2] + Math.RandomFloatInclusive(-5.0, 5.0);
				memberPos[1] = GetGame().SurfaceY(memberPos[0], memberPos[2]);
				squadSpawnPositions.Insert(memberPos);
			}
			
			// Spawn Leader
			eAIBase leader = eAIBase.Cast(GetGame().CreateObject(GetRandomAI(), squadSpawnPositions[0]));
			if (!leader)
			{
				Print("[SMM] Failed to create leader AI for group " + (g + 1));
				continue;
			}
			
			// Create Group
			eAIGroup group = eAIGroup.GetGroupByLeader(leader, true, faction);
			if (!group)
			{
				Print("[SMM] Failed to create AI group " + (g + 1));
				GetGame().ObjectDelete(leader);
				continue;
			}
			
			// Config Leader
			ExpansionHumanLoadout.Apply(leader, loadout, false);
			m_MissionAIs.Insert(leader);
			SafeConfigureAI(leader, group, 1); // 1 = DEFAULT looting
			
			int spawnedCount = 1;
			
			// Spawn Followers and Add them to group
			for (int i = 1; i < aiSquadSize; i++)
			{
				eAIBase ai = eAIBase.Cast(GetGame().CreateObject(GetRandomAI(), squadSpawnPositions[i]));
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
					Print("[SMM] FAILED to spawn AI at index " + i.ToString() + " for group " + (g + 1));
				}
			}
			
			// Set up Patrol Route around the spawn position
			CreateRandomIntersectingPatrolRoute(group, spawnPos, 50.0); // 50m radius around spawn position
			
			// Set Formation
			group.SetFormation(CreateFormation(formationType));
			
			//Print(string.Format("[SMM] Spawned AI group %1: %2/%3 AI (Leader: %4) Loadout:%5 Faction:%6", (g + 1), spawnedCount, aiSquadSize, leader.GetType(), loadout, faction.GetName()));
		}
		
		//Print("[SMM] Spawned " + numAIGroups + " AI patrol groups around checkpoint positions");
		
	}

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

	// Creates a patrol route that always passes through the mission center, with random entry/exit points
	void CreateRandomIntersectingPatrolRoute(eAIGroup group, vector center, float radius)
	{
		// Generate 4 random directions (N, E, S, W) and shuffle them
		ref array<vector> directions = {
			Vector(1, 0, 0),   // East
			Vector(0, 0, 1),   // North
			Vector(-1, 0, 0),  // West
			Vector(0, 0, -1)   // South
		};
		// Fisher-Yates shuffle
		for (int i = directions.Count() - 1; i > 0; i--)
		{
			int j = Math.RandomInt(0, i + 1);
			vector tmp = directions[i];
			directions[i] = directions[j];
			directions[j] = tmp;
		}

		// Build waypoints: edge -> center -> opposite edge -> center -> next edge, etc.
		for (int ik = 0; ik < directions.Count(); ik++)
		{
			vector edge = center + directions[ik] * radius;
			group.AddWaypoint(edge);
			group.AddWaypoint(center);
		}
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

	// Spawn Expansion AI Patrol
	void SpawnPatrol()
	{
		// Initialize with "RANDOM" to trigger randomization (or use defaults)
		int factionType = RANDOM;    // RANDOM will randomize unless 1-10 then DEFAULT_FACTION set at Top
		int loadoutType = RANDOM;    // RANDOM will randomize unless 1-14 then DEFAULT_FACTION set at Top
		int squadSize = RANDOM;      // RANDOM will randomize unless 1 or higher then DEFAULT_FACTION set at Top
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
		//CreateRandomIntersectingPatrolRoute(group, m_MissionPosition, 700); // 25m radius

		// Verify and Set Formation
		Print(string.Format("[SMM] Spawned patrol: %1/%2 AI (Leader: %3) Loadout:%4 Faction:%5", spawnedCount, squadSize, leader.GetType(), loadout, faction.GetName()));
		group.SetFormation(CreateFormation(formationType));
		
	}
	#endif
	#endif

	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		if ( m_MissionPosition && m_MissionPosition != "0 0 0" )
		{
			//Call spawners
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			
			#ifdef ENFUSION_AI_PROJECT
			#ifdef EXPANSIONMODAI
			if (SpawnPatrolEnabled)
			{
				Print("[SMM] Expansion AI Patrols Turned On");
				GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SpawnAIGroups, BotDlyTime, false );
			}
			else
			{
				Print("[SMM] Expansion AI Patrols Turned Off");
			}
			#endif
			#endif

			return true;
		}
		else
		{
			Print("[SMM] ERROR : Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}
}


