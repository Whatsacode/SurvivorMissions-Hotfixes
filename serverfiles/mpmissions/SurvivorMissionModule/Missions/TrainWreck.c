class TrainWreckMission extends SurvivorMissions
{
	// Activate Expansion AI Patrol
	bool SpawnPatrolEnabled = false;

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

	//Mission related entities 
	Object WagonCargo;
	Object CrashedTrainEngine;
	Object CrashedTrainWagon_1;
	Object CrashedTrainWagon_2;
	Object CrashedTrainWagon_3;
	Object CrashedTrainWagon_4;
	Object CrashedTrainWagon_5;
	Object CrashedTrainWagon_6;
	ItemBase MissionObject;

	//Mission timeout
  	int MissionCutoffTime;

	//Mission parameters
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	int InfectedCount;
	
	//Mission containers
	ref array<vector> CargoSpawns = new array<vector>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<vector> FireSpawns = new array<vector>;
	ref array<vector> Wagons = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	ref array<string> InfectedGuardTypes = new array<string>;

	//Mission global variables
	string TrainEngineModel;
	string TrainWagonModel;
	string Wagon_3_CargoOrientation;
	
	override bool IsExtended() return false;
	
	void TrainWreckMission()
	{
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time
		
		//Mission zones
		m_MissionZoneOuterRadius = 750.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 8.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "United Nations Command";
	
		//Set mission messages
		m_MissionMessage1 = "The UN dispatched soldiers to secure key high-profile locations around Chernarus in an effort to restore order in South Zagoria.\nBut our worst nightmare has become reality.\nThe train never arrived at any of its destinations.";
		m_MissionMessage2 = "We received a report of an explosion on the rail tracks " + m_MissionLocationDir + " to " + m_MissionLocation + ".\nWe can only assume the rails were sabotaged to derail the train.";
		m_MissionMessage3 = "The train was carrying unknown cargo, most of which has likely already been stolen.\nWe need to find out if anyone survived the incident. Be careful!";

		//Train Engine fire spawns 
		FireSpawns.Insert("0 0 0");				//engine fire
		FireSpawns.Insert("0 -2.18 -2");		//engine fire
		FireSpawns.Insert("0 0 0.5");			//engine fire
		FireSpawns.Insert("0 0 1");				//engine fire
		FireSpawns.Insert("0 0 3");				//engine fire
		
		//Train Wagons positions 
		Wagons.Insert("-5 -2.31 12");
		Wagons.Insert("-5.4 -2.02 23");
		Wagons.Insert("-4 -2.02 35");
		Wagons.Insert("-6 -2.02 46.4");
		Wagons.Insert("-8 -1.85 59");
		Wagons.Insert("-11 -1.7 71");
		
		//Amount of infected to spawn on deployment
		InfectedCount = 30;
		
		//Infected spawnpoints for Mission Finale
		InfectedSpawns.Insert("17 0 40");
		InfectedSpawns.Insert("25 0 -10");
		InfectedSpawns.Insert("34 0 -20");
		InfectedSpawns.Insert("45 0 40");
		InfectedSpawns.Insert("50 0 30");
		InfectedSpawns.Insert("-20 0 15");
		InfectedSpawns.Insert("-30 0 14");
		InfectedSpawns.Insert("-30 0 15");
		InfectedSpawns.Insert("-40 0 -16");
		InfectedSpawns.Insert("-40 0 -50");
		InfectedSpawns.Insert("-50 0 -20");
		InfectedSpawns.Insert("-60 0 50");
		
		//CargoSpawns
		CargoSpawns.Insert("0.44 -0.5 1.87");
		CargoSpawns.Insert("0.35 -0.5 -3.68");
		CargoSpawns.Insert("0.21 -0.5 3.22");
		CargoSpawns.Insert("0.31 -0.5 2.15");
		CargoSpawns.Insert("0.48 -0.5 1.55");
		CargoSpawns.Insert("0.48 -0.5 -0.55");
		CargoSpawns.Insert("0.71 -0.5 -1.75");
		CargoSpawns.Insert("0.35 -0.5 -2.43");
		
		//Infected Types
		InfectedGuardTypes.Insert("ZmbM_usSoldier_normal_Woodland");
		InfectedGuardTypes.Insert("ZmbM_usSoldier_normal_Desert");
		
		InfectedTypes.Insert("ZmbM_usSoldier_normal_Woodland");
		InfectedTypes.Insert("ZmbM_usSoldier_normal_Desert");
		InfectedTypes.Insert("ZmbM_SoldierNormal");
		InfectedTypes.Insert("ZmbM_PatrolNormal_Summer");
		
		//Train modules Classnames
		TrainEngineModel = "Land_Train_742_Red";
		TrainWagonModel = "Land_Train_Wagon_Box";

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
	
	void ~TrainWreckMission()
	{
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
        float radius = 750.0;  // Example radius, you can change this value

        // Set name for the zone
        string name = "TrainWreck PVP Zone";
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
	
	void ExplodeWreck(int count)
	{
		for ( int n=0; n < count; n++)
		{
			Grenade_Base Explosives = Grenade_Base.Cast( GetGame().CreateObject( "M67Grenade",CrashedTrainEngine.ModelToWorld("5.15 -2.28 -4.63")));
			Explosives.ActivateImmediate();
		}

	}

	void SpawnObjects()
	{
		// Spawn crashed Train
		int TrainOriConversion;
		string TrainOrientation;
		string Wagon_4_CargoOrientation;
		
		// Train Engine
		TrainOriConversion = m_MissionDescription[3].ToInt() - 180;
		if ( TrainOriConversion < 0 ) TrainOriConversion += 360;
		TrainOrientation = TrainOriConversion.ToString() +" 0 -5";
		
		CrashedTrainEngine = GetGame().CreateObjectEx( TrainEngineModel, m_MissionPosition, ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH );
		CrashedTrainEngine.SetOrientation( TrainOrientation.ToVector() );
		CrashedTrainEngine.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedTrainEngine );

		// Train Wagon #1
		TrainOriConversion = m_MissionDescription[3].ToInt() - 40 - 180;
		if ( TrainOriConversion < 0 ) TrainOriConversion += 360;
		TrainOrientation = TrainOriConversion.ToString() +" 0 0";
		
		CrashedTrainWagon_1 = GetGame().CreateObjectEx( "Land_Train_Wagon_Box", CrashedTrainEngine.ModelToWorld( Wagons.Get(0) ), ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH );
		CrashedTrainWagon_1.SetOrientation( TrainOrientation.ToVector() );
		CrashedTrainWagon_1.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedTrainWagon_1 );

		// Train Wagon #2
		TrainOriConversion = m_MissionDescription[3].ToInt() + 25 - 180;
		if ( TrainOriConversion < 0 ) TrainOriConversion += 360;
		TrainOrientation = TrainOriConversion.ToString() +" 0 0";
		
		CrashedTrainWagon_2 = GetGame().CreateObjectEx( TrainWagonModel, CrashedTrainEngine.ModelToWorld( Wagons.Get(1) ), ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH );
		CrashedTrainWagon_2.SetOrientation( TrainOrientation.ToVector() );
		CrashedTrainWagon_2.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedTrainWagon_2 );

		// Train Wagon #3 - Loot here (locked wagon)
		TrainOriConversion = m_MissionDescription[3].ToInt() - 25 - 180;
		if ( TrainOriConversion < 0 ) TrainOriConversion += 360;
		Wagon_3_CargoOrientation = TrainOriConversion.ToString() +" 0 -20";
		
		CrashedTrainWagon_3 = GetGame().CreateObjectEx( TrainWagonModel, CrashedTrainEngine.ModelToWorld( Wagons.Get(2) ), ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH );
		CrashedTrainWagon_3.SetOrientation( Wagon_3_CargoOrientation.ToVector() );
		CrashedTrainWagon_3.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedTrainWagon_3 );
		
		//Close & lock the doors
		Building Wagon3 = Building.Cast( CrashedTrainWagon_3 );
		for (int d1=0; d1 < 2; d1++ )
		{
			if ( Wagon3.IsDoorOpen(d1) ) Wagon3.CloseDoor(d1);
			if ( !Wagon3.IsDoorLocked(d1) ) Wagon3.LockDoor(d1);
		}

        //Reset the mission's target position to Wagon #3, where the loot is.
		m_MissionPosition = CrashedTrainEngine.ModelToWorld( Wagons.Get(2));

		// Train Wagon #4 - a Medical crate and some empty crates  (one door locked)
		TrainOriConversion = m_MissionDescription[3].ToInt() - 20 - 180;
		if ( TrainOriConversion < 0 ) TrainOriConversion += 360;
		Wagon_4_CargoOrientation = TrainOriConversion.ToString() +" 0 10";
		
		CrashedTrainWagon_4 = GetGame().CreateObjectEx( TrainWagonModel, CrashedTrainEngine.ModelToWorld( Wagons.Get(3) ), ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH );
		CrashedTrainWagon_4.SetOrientation( Wagon_4_CargoOrientation.ToVector() );
		CrashedTrainWagon_4.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedTrainWagon_4 );
		
		//Close & lock the doors
		Building Wagon4 = Building.Cast( CrashedTrainWagon_4 );
		for (int k=0; k < 1; k++ )
		{
			if ( Wagon4.IsDoorOpen(k) ) Wagon4.CloseDoor(k);
			if ( !Wagon4.IsDoorLocked(k) ) Wagon4.LockDoor(k); 
		}
		
		// Train Wagon #5
		TrainOriConversion = m_MissionDescription[3].ToInt() - 180;
		if ( TrainOriConversion < 0 ) TrainOriConversion += 360;
		TrainOrientation = TrainOriConversion.ToString() +" 0 90";
		
		CrashedTrainWagon_5 = GetGame().CreateObjectEx( TrainWagonModel, CrashedTrainEngine.ModelToWorld( Wagons.Get(4) ), ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH );
		CrashedTrainWagon_5.SetOrientation( TrainOrientation.ToVector() );
		CrashedTrainWagon_5.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedTrainWagon_5 );

		// Train Wagon #6
		TrainOriConversion = m_MissionDescription[3].ToInt() - 20 - 180;
		if ( TrainOriConversion < 0 ) TrainOriConversion += 360;
		TrainOrientation = TrainOriConversion.ToString() +" 0 90";
		
		CrashedTrainWagon_6 = GetGame().CreateObjectEx( TrainWagonModel, CrashedTrainEngine.ModelToWorld( Wagons.Get(5) ), ECE_PLACE_ON_SURFACE | ECE_UPDATEPATHGRAPH );
		CrashedTrainWagon_6.SetOrientation( TrainOrientation.ToVector() );
		CrashedTrainWagon_6.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedTrainWagon_6 );

		// Create smoke on destroyed engine
		Object EngineSmoke = GetGame().CreateObject( "Wreck_UH1Y", CrashedTrainEngine.ModelToWorld("0 -5 0"),false,false,true);
		m_MissionObjects.Insert( EngineSmoke );
		
		//Explosion at crash time delayed	
		//ExplodeWreck(3);
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 800, false, 6 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 900, false, 6 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 1800, false, 5 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 4800, false, 5 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 7000, false, 6 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 7100, false, 6 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 8200, false, 6 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 8300, false, 6 );
		
		// Enginefire
		for (int s=0; s < FireSpawns.Count(); s++)
		{
			FireplaceBase Enginefire;
			
			if ( s > 1 )
			{
				vector posXZ = CrashedTrainEngine.ModelToWorld( FireSpawns.Get(s) );
				float x = posXZ[0];
				float z = posXZ[2];
				float y = GetGame().SurfaceY(x,z);
				vector Groundpos = {x,y,z};
				
				Enginefire = FireplaceBase.Cast( GetGame().CreateObject( "Fireplace", Groundpos));
				Enginefire.SetPosition( Groundpos - "0 0.3 0" );
			}
			else 
			Enginefire = FireplaceBase.Cast( GetGame().CreateObject( "Fireplace", CrashedTrainEngine.ModelToWorld( FireSpawns.Get(s) )));
			
			Enginefire.Synchronize();
			Enginefire.GetInventory().CreateAttachment("Firewood");
			Enginefire.GetInventory().CreateAttachment("WoodenStick");
			Enginefire.GetInventory().CreateAttachment("Rag");
			Enginefire.StartFire( true );
			m_MissionObjects.Insert( Enginefire );
		}
		
		//Wagon Cargo (Wagon #4)
		for (int wc=0; wc < CargoSpawns.Count(); wc++)
		{
			WagonCargo = GetGame().CreateObject( "WoodenCrate", CrashedTrainWagon_4.ModelToWorld( CargoSpawns.Get(wc) ));
			WagonCargo.SetOrientation( Wagon_4_CargoOrientation.ToVector() );
			WagonCargo.PlaceOnSurface();
			m_MissionObjects.Insert( WagonCargo );
		}
		
		//Spawn containers - Medical kit [Container 0]
		MissionObject = ItemBase.Cast( GetGame().CreateObject( "WoodenCrate", CrashedTrainWagon_4.ModelToWorld( CargoSpawns.Get(0))));
		MissionObject.SetOrientation( Wagon_4_CargoOrientation.ToVector() );
		MissionObject.PlaceOnSurface();
		
		//Spawn 3 First Aid Kits, fill them and put them inside the crate
		EntityAI FirstAidKitContainer;
		
		TStringArray LootTableMeds = {"BandageDressing","IodineTincture","BloodBagEmpty","BloodTestKit","CharcoalTablets","Epinephrine","Morphine","PainkillerTablets","PurificationTablets","SalineBag","SalineBagIV","StartKitIV","TetracyclineAntibiotics","VitaminBottle"};
		
		for (int fak=0; fak < 3; fak++)
		{
			FirstAidKitContainer = MissionObject.GetInventory().CreateInInventory("FirstAidKit");
			for (int fak_inv=0; fak_inv < 6; fak_inv++)
			{
				FirstAidKitContainer.GetInventory().CreateInInventory(LootTableMeds.GetRandomElement());
			}
		}
		
		//Spawn a Lockpick & other medical items
		MissionObject.GetInventory().CreateInInventory("DisinfectantSpray");
		for (int bd=0; bd < 9; bd++)
		{
			MissionObject.GetInventory().CreateInInventory("BandageDressing");
		}
		MissionObject.GetInventory().CreateInInventory("Lockpick");
		
		//Insert Medical supplies container into mission objects list
		m_MissionObjects.Insert( MissionObject );
		
		Print("[SMM] Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}
	
	void SpawnAIs()
	{	
		//Spawn infected train engineer
		vector InfectedPos = CrashedTrainEngine.ModelToWorld( "3 0 -9" );
		m_MissionAIs.Insert( GetGame().CreateObject( "ZmbM_Jacket_black" , InfectedPos , false , true ) );
		DayZInfected InfectedSurvivor = DayZInfected.Cast( m_MissionAIs[0] );
			InfectedSurvivor.GetInventory().CreateAttachment("ConstructionHelmet_Orange");
			InfectedSurvivor.GetInventory().CreateInInventory("PersonalRadio");
			InfectedSurvivor.GetInventory().CreateInInventory("Battery9v");
			InfectedSurvivor.GetInventory().CreateInInventory("Orange");
			InfectedSurvivor.GetInventory().CreateInInventory("Lockpick");
					
			InfectedSurvivor.SetHealth("","",90);
		
		//Spawn infected UN soldiers (with delay)
		int SpawnDlyTime = 500;
		
		for ( int j = 1 ; j < InfectedCount ; j++ )
		{
			//Spawn infected time delayed 500ms
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DelayedAISpawn, SpawnDlyTime, false, j );
			SpawnDlyTime += 500;
		}
		
	}
	
	void DelayedAISpawn(int j)
	{
		//calc new spawn position
		float x = Math.RandomFloatInclusive( 20 , 40 );
		float y = Math.RandomFloatInclusive( 20 , 40 );	
		int Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice > 4 ) x *= -1.0;
		Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice < 5 ) y *= -1.0;
		vector NewPosVector = { x, 0, y };
		
		//Spawn infected UN soldiers
		string RandomInfected = InfectedGuardTypes.GetRandomElement();
		string RandomHelmet;
		TStringArray RandomHelmets = {"BallisticHelmet_UN","MilitaryBeret_UN"};
		
		vector InfectedArmyPos = CrashedTrainEngine.ModelToWorld( "-10 0 25" );
		m_MissionAIs.Insert( GetGame().CreateObject( RandomInfected, InfectedArmyPos + NewPosVector, false, true ) );
		DayZInfected InfectedArmySurvivors = DayZInfected.Cast( m_MissionAIs[j] );
		
		RandomHelmet = RandomHelmets.GetRandomElement();
		InfectedArmySurvivors.GetInventory().CreateAttachment(RandomHelmet);
		InfectedArmySurvivors.GetInventory().CreateAttachment("PlateCarrierVest");
		
	}
	
	override void ObjDespawn() 
	{	
		//Despawn nothing
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone
	
        //Spawn Rewards
		SpawnRewards(Wagon_3_CargoOrientation);
		
		//Respawn some infected 
		for ( int m = 0 ; m < InfectedSpawns.Count() ; m++)
		{
    	   	string RandomInfected = InfectedTypes.GetRandomElement();
			vector InfectedPos = CrashedTrainEngine.ModelToWorld( InfectedSpawns.Get(m) );
			m_MissionAIs.Insert( GetGame().CreateObject( RandomInfected, InfectedPos, false, true ));
			DayZInfected InfectedSurvivors = DayZInfected.Cast( m_MissionAIs[m] );
			
		}
		
		//Finish mission
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;
	}
	
	override void PlayerChecks( PlayerBase player )
	{
		//Check if container gets taken from player
		if ( MissionSettings.Opt_DenyObjTakeaway )
		{
			if ( m_MissionObjects[0] && m_MissionObjects[0].ClassName() == "SeaChest" )
			{
				if ( player.GetInventory().HasEntityInInventory( EntityAI.Cast( m_MissionObjects[0] )) && !m_ContainerWasTaken )
				{
					m_ContainerWasTaken = true;
					Print("[SMM] Mission object container was taken by a player ...and will be deleted.");
					GetGame().ObjectDelete( m_MissionObjects[0] );
				}
			}
		}
	}
	
	void SpawnRewards(string Wagon_3_CargoOrientation)
	{
		
        int j = 0;
        int RewardCount = m_PlayersInZone.Count();
		
		//Spawn selected loadout items in mission object
		EntityAI weapon;
		
		for (RewardCount; RewardCount > 0; RewardCount--) 
		{
			
			//Spawn containers
			//Note: We are using "RewardCount" as a counter for containers, as the count of the players in the mission zone
			MissionObject = ItemBase.Cast( GetGame().CreateObject( "SeaChest", CrashedTrainWagon_3.ModelToWorld( CargoSpawns.Get(RewardCount))));
			MissionObject.SetOrientation( Wagon_3_CargoOrientation.ToVector() );
			MissionObject.PlaceOnSurface();
			
			//Get random loadout 
			int selectedLoadout = Math.RandomIntInclusive( 0, 9);	//!change randomization limit after adding new loadouts!	

			if ( selectedLoadout == 0 )
			{
				weapon = MissionObject.GetInventory().CreateInInventory("M4A1");
					weapon.GetInventory().CreateAttachment("M4_RISHndgrd");
					weapon.GetInventory().CreateAttachment("M4_MPBttstck");
					weapon.GetInventory().CreateAttachment("ACOGOptic");
					weapon.GetInventory().CreateAttachment("M4_Suppressor");
				MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
				MissionObject.GetInventory().CreateInInventory("M4_T3NRDSOptic");
				MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
				MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
				MissionObject.GetInventory().CreateInInventory("CanOpener");
				MissionObject.GetInventory().CreateInInventory("PeachesCan");
				MissionObject.GetInventory().CreateInInventory("Canteen");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 1)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("SVD");
					weapon.GetInventory().CreateAttachment("PSO1Optic");
				MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
				MissionObject.GetInventory().CreateInInventory("PSO1Optic");
				MissionObject.GetInventory().CreateInInventory("KazuarOptic");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("CanOpener");
				MissionObject.GetInventory().CreateInInventory("PeachesCan");
				MissionObject.GetInventory().CreateInInventory("Canteen");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 2)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("AKM");
					weapon.GetInventory().CreateAttachment("AK_RailHndgrd_Green");
					weapon.GetInventory().CreateAttachment("AK_PlasticBttstck_Green");
					weapon.GetInventory().CreateAttachment("PSO1Optic");
					weapon.GetInventory().CreateAttachment("AK_Suppressor");
				MissionObject.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("CanOpener");
				MissionObject.GetInventory().CreateInInventory("PeachesCan");
				MissionObject.GetInventory().CreateInInventory("Canteen");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 3)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("FAL");
					weapon.GetInventory().CreateAttachment("Fal_OeBttstck");
				MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
				MissionObject.GetInventory().CreateInInventory("M4_Suppressor");
				MissionObject.GetInventory().CreateInInventory("ACOGOptic");
				MissionObject.GetInventory().CreateInInventory("FNX45");
				MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
				MissionObject.GetInventory().CreateInInventory("FNP45_MRDSOptic");
				MissionObject.GetInventory().CreateInInventory("PistolSuppressor");
				MissionObject.GetInventory().CreateInInventory("PsilocybeMushroom");
				MissionObject.GetInventory().CreateInInventory("AmmoBox");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}	
			if (selectedLoadout == 4)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("SKS");
					weapon.GetInventory().CreateAttachment("PUScopeOptic");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				weapon = MissionObject.GetInventory().CreateInInventory("FNX45");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");
					EntityAI weaponlight = weapon.GetInventory().CreateAttachment("TLRLight");
						weaponlight.GetInventory().CreateAttachment("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
				MissionObject.GetInventory().CreateInInventory("AmmoBox");
			}	
			if (selectedLoadout == 5)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("Winchester70");
					weapon.GetInventory().CreateAttachment("HuntingOptic");
				MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
				MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
				MissionObject.GetInventory().CreateInInventory("FNX45");
				MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
				MissionObject.GetInventory().CreateInInventory("AmmoBox");
				MissionObject.GetInventory().CreateInInventory("PistolSuppressor");
				MissionObject.GetInventory().CreateInInventory("TLRLight");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 6)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("MP5K");
					weapon.GetInventory().CreateAttachment("MP5_RailHndgrd");
					weapon.GetInventory().CreateAttachment("MP5k_StockBttstck");
					weapon.GetInventory().CreateAttachment("M68Optic");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");			
				MissionObject.GetInventory().CreateInInventory("Mag_MP5_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_MP5_30Rnd");
				MissionObject.GetInventory().CreateInInventory("AmmoBox_9x19_25rnd");
				MissionObject.GetInventory().CreateInInventory("GP5GasMask");
				MissionObject.GetInventory().CreateInInventory("NBCGlovesGray");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");	
				MissionObject.GetInventory().CreateInInventory("SpaghettiCan");
				MissionObject.GetInventory().CreateInInventory("M18SmokeGrenade_Red");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Battery9V");				
			}
			if (selectedLoadout == 7)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("AK74");
					weapon.GetInventory().CreateAttachment("AK_RailHndgrd");
					weapon.GetInventory().CreateAttachment("AK74_WoodBttstck");	
					weapon.GetInventory().CreateAttachment("KashtanOptic");
					weapon.GetInventory().CreateAttachment("AK_Suppressor");			
				MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Headtorch_Grey");
				MissionObject.GetInventory().CreateInInventory("NBCBootsGray");
				MissionObject.GetInventory().CreateInInventory("Canteen");	
				MissionObject.GetInventory().CreateInInventory("TacticalBaconCan");
				MissionObject.GetInventory().CreateInInventory("Tomato");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Battery9V");				
			}
			if (selectedLoadout == 8)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("AKS74U");
					weapon.GetInventory().CreateAttachment("AKS74U_Bttstck");			
				MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_AK74_30Rnd");
				MissionObject.GetInventory().CreateInInventory("M67Grenade");
				MissionObject.GetInventory().CreateInInventory("M67Grenade");
				MissionObject.GetInventory().CreateInInventory("Matchbox");
				MissionObject.GetInventory().CreateInInventory("Canteen");	
				MissionObject.GetInventory().CreateInInventory("PortableGasStove");
				MissionObject.GetInventory().CreateInInventory("SmallGasCanister");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Battery9V");			
			}
			if (selectedLoadout == 9)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("Glock19");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");			
				MissionObject.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
				MissionObject.GetInventory().CreateInInventory("FishingRod");
				MissionObject.GetInventory().CreateInInventory("Carp");
				MissionObject.GetInventory().CreateInInventory("Hook");
				MissionObject.GetInventory().CreateInInventory("Worm");
				MissionObject.GetInventory().CreateInInventory("CombatKnife");
				MissionObject.GetInventory().CreateInInventory("FieldShovel");
				MissionObject.GetInventory().CreateInInventory("Canteen");	
				MissionObject.GetInventory().CreateInInventory("MackerelFilletMeat");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Battery9V");			
			}
				
			Print("[SMM] RewardCount: #" + RewardCount + " - Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );
				
			//Insert mission container into mission objects list
			m_MissionObjects.Insert( MissionObject );
				
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
		CreateCircularPatrolRoute(group, m_MissionPosition, 200); // 25m radius

		// Verify and Set Formation
		Print(string.Format("[SMM] Spawned patrol: %1/%2 AI (Leader: %3) Loadout:%4 Faction:%5", spawnedCount, squadSize, leader.GetType(), loadout, faction.GetName()));
		group.SetFormation(CreateFormation(formationType));
		
	}

	#endif
	#endif

	override bool DeployMission()
	{	
		//When first player enters the mission zone (primary/secondary)
		if ( m_MissionPosition && m_MissionPosition != "0 0 0" )
		{
			//Call spawners	
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );

			#ifdef ENFUSION_AI_PROJECT
			#ifdef EXPANSIONMODAI
			if (SpawnPatrolEnabled)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(this.SpawnPatrol);
			}
			if (!SpawnPatrolEnabled)
			{
				Print("[SMM] Expansion AI Patrol Turned Off");	
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