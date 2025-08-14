class PlaneCrashMission extends SurvivorMissions
{
	// Activate Expansion AI Patrol
	bool SpawnPatrolEnabled = true;

	// Activate PVEZ PVP Zone
	bool SpawnPVPZone = true;

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

	//Mission timeout
  	int MissionCutoffTime;
	  
	//Mission related entities 
	Object CrashedC130;
	ItemBase MissionObject;

	//Mission parameters
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	
	//Mission containers
	ref array<vector> ContainerSpawns = new array<vector>;
	ref array<vector> InfectedSpawns = new array<vector>;
	ref array<vector> FireSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;

	//Mission variables 
	string SurvivorName;		
	
	override bool IsExtended() return false;
	
	void PlaneCrashMission()
	{
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time
		
		//Mission zones
		m_MissionZoneOuterRadius = 700.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 8.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		SurvivorName = "Gerry Lane";
		
		//Set mission messages
        m_MissionMessage1 = "I just received a emergency call from a military pilot. One crew member was infected and suddenly attacked others. He said this flight has the highest international priority.";
        m_MissionMessage2 = "Also onboard is Mr. "+ SurvivorName +", he is an UN agent and it seems he has some information about the cure for this pandemic disease. They were at flight level 300 on their way to Wales.";
        m_MissionMessage3 = "The pilot said he was bitten and tried to emergency land somewhere in the fields\n** "+ m_MissionLocationDir +" of "+ m_MissionLocation +" **\nFind out if they made it. They have some equipment on board!";		
		
		//Engine fire spawns 
		FireSpawns.Insert("5.15 -2.18 -4.63");		//engine
		FireSpawns.Insert("5.15 -2.18 -5");			//engine
		FireSpawns.Insert("5.15 0 0");			//fire trace
		FireSpawns.Insert("4.8 0 0.5");			//fire trace
		FireSpawns.Insert("5.3 0 1");			//fire trace
		FireSpawns.Insert("5.15 0 15");			//fire trace
		FireSpawns.Insert("5.15 0 25");			//fire trace
		//FireSpawns.Insert("5.15 0 30");			//fire trace
		//FireSpawns.Insert("5.15 0 45");			//fire trace
		//FireSpawns.Insert("5.15 0 55");			//fire trace
		
		//Container spawnpoints
		ContainerSpawns.Insert("0.87 -4.5 -2.5");
		ContainerSpawns.Insert("0.87 -4.47 -0.5");
		
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
		InfectedTypes.Insert("ZmbM_Jacket_black");			InfectedTypes.Insert("ZmbF_ShortSkirt_beige");
		InfectedTypes.Insert("ZmbM_Jacket_stripes");		InfectedTypes.Insert("ZmbF_VillagerOld_Red");
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
		InfectedTypes.Insert("ZmbM_Jacket_magenta");		InfectedTypes.Insert("ZmbF_BlueCollarFat_Green");
		InfectedTypes.Insert("ZmbM_PolicemanSpecForce");	InfectedTypes.Insert("ZmbF_DoctorSkinny");
	
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
	
	void ~PlaneCrashMission()
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
        string name = "Plane Crash PVP Zone";
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
			Grenade_Base Explosives = Grenade_Base.Cast( GetGame().CreateObject( "M67Grenade",CrashedC130.ModelToWorld("5.15 -2.28 -4.63")));
			Explosives.ActivateImmediate();
		}

	}
	void SpawnObjects()
	{
		//Spawn crashed plane crashed C130 
		int PlaneOriConversion;
		string PlaneOrientation;
		
		PlaneOriConversion = m_MissionDescription[3].ToInt() - 180;
		if ( PlaneOriConversion < 0 ) PlaneOriConversion += 360;
		PlaneOrientation = PlaneOriConversion.ToString() +" 0 0";
		
		CrashedC130 = GetGame().CreateObject( "Land_Wreck_C130J", m_MissionPosition );
		CrashedC130.SetOrientation( PlaneOrientation.ToVector() );
		CrashedC130.PlaceOnSurface();
		m_MissionObjects.Insert( CrashedC130 );

		//Enginefire
		for (int s=0; s < FireSpawns.Count(); s++)
		{
			FireplaceBase Enginefire;
			
			if ( s > 1 )
			{
				vector posXZ = CrashedC130.ModelToWorld( FireSpawns.Get(s) );
				float x = posXZ[0];
				float z = posXZ[2];
				float y = GetGame().SurfaceY(x,z);
				vector Groundpos = {x,y,z};
				
				Enginefire = FireplaceBase.Cast( GetGame().CreateObject( "Fireplace", Groundpos));
				Enginefire.SetPosition( Groundpos - "0 0.3 0" );
			}
			else 
			Enginefire = FireplaceBase.Cast( GetGame().CreateObject( "Fireplace", CrashedC130.ModelToWorld( FireSpawns.Get(s) )));
			
			Enginefire.Synchronize();
			Enginefire.GetInventory().CreateAttachment("Firewood");
			Enginefire.GetInventory().CreateAttachment("WoodenStick");
			Enginefire.GetInventory().CreateAttachment("Rag");
			Enginefire.StartFire( true );
			m_MissionObjects.Insert( Enginefire );
		}
		
		//Create smoke on destroyed engine
		Object EngineSmoke = GetGame().CreateObject( "Wreck_UH1Y", CrashedC130.ModelToWorld("5.15 -7 -3.63"),false,false,true);
		m_MissionObjects.Insert( EngineSmoke );
		
		//Explosion at planecrash time delayed	
		//ExplodeWreck(3);
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 800, false, 6 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 1400, false, 5 );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.ExplodeWreck, 1800, false, 6 );
		
		//Spawn containers 
		int selectedLoadout;
		
		for (int i=0; i < ContainerSpawns.Count(); i++)
		{	
			MissionObject = ItemBase.Cast( GetGame().CreateObject( "WoodenCrate", CrashedC130.ModelToWorld( ContainerSpawns.Get(i))));
			
			//Get random loadout 
			selectedLoadout = Math.RandomIntInclusive(0,9);	//!change randomization limit after adding new loadouts!	
		
			//Spawn selected loadout items in mission object
			EntityAI weapon;
					
			if (selectedLoadout == 0)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("M4A1_Green");
					weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
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
				MissionObject.GetInventory().CreateInInventory("AK_Suppressor");
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
				weapon.GetInventory().CreateAttachment("'AK_Suppressor");			
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
					
			Print("[SMM] Mission rewards spawned in reward container"+i+". Randomly selected loadout was "+selectedLoadout+"." );
				
			//Insert mission container into mission objects list
			m_MissionObjects.Insert( MissionObject );
		}
							
		Print("[SMM] Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}
	
	void SpawnAIs()
	{	
		//Spawn infected pilot
		vector InfectedPos = CrashedC130.ModelToWorld( "3 0 -9" );
		m_MissionAIs.Insert( GetGame().CreateObject( "ZmbM_CommercialPilotOld_Olive" , InfectedPos , false , true ) );
		DayZInfected InfectedSurvivor = DayZInfected.Cast( m_MissionAIs[0] );
			InfectedSurvivor.GetInventory().CreateAttachment("ZSh3PilotHelmet_Green");
			InfectedSurvivor.GetInventory().CreateAttachment("UKAssVest_Olive");
			InfectedSurvivor.GetInventory().CreateInInventory("Glock19");
			InfectedSurvivor.GetInventory().CreateInInventory("Mag_Glock_15Rnd");
			InfectedSurvivor.GetInventory().CreateInInventory("Battery9V");
			InfectedSurvivor.GetInventory().CreateInInventory("Battery9V");
			InfectedSurvivor.GetInventory().CreateInInventory("Battery9V");
					
			InfectedSurvivor.SetHealth("","",15);
		
		
		//Spawn infected
		for ( int j = 0 ; j < InfectedSpawns.Count() ; j++ )
		{
    	   	string RandomInfected = InfectedTypes.GetRandomElement();
			vector InfectedPos1 = CrashedC130.ModelToWorld( InfectedSpawns.Get(j) );
			m_MissionAIs.Insert( GetGame().CreateObject( RandomInfected, InfectedPos1, false, true ) );
		}	
	}
	
	override void ObjDespawn() 
	{	
		//Despawn nothing
	}
	
	override void MissionFinal()
	{	//When player enters last mission target zone	
		//Respawn some infected 
		for ( int m = 0 ; m < InfectedSpawns.Count() ; m++)
		{
    	   	string RandomInfected = InfectedTypes.GetRandomElement();
			vector InfectedPos = CrashedC130.ModelToWorld( InfectedSpawns.Get(m) );
			m_MissionAIs.Insert( GetGame().CreateObject( RandomInfected, InfectedPos, false, true ));			
		}

		//Finish mission
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;			
	}
	
	override void PlayerChecks( PlayerBase player )
	{
		//nothing to check
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
		CreateCircularPatrolRoute(group, m_MissionPosition, 25); // 25m radius

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

