class CaptureFlagMission extends SurvivorMissions
{
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

	//Mission related entities 
	ItemBase MissionObject;
	Object FlagBase, Table1, Table2;
	BarrelHoles_ColorBase BarrelFire;
	Flag_Base Flag;

	//Mission timeout
  	int MissionCutoffTime;
		
	//Mission parameters
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	int HordeDensity = 60;					//infected, spawned in circle area
	int CaptureTime = 1200;					//seconds, time to capture the flag ..full minutes (60,120,...)
	string DownFlag = "Flag_Bohemia";		//class name of flag to lower
	string UpFlag = "Flag_Chernarus";		//class name of flag to raise
	
	//mission vars
	vector PoleHighPos, PoleLowPos;		
	float flag_speed, delta_height;
	int FlagElevationChkTime;
	int FireplaceSpendChkTime;
	int FlagCaptureProgress;
	int TeamPlayerCount;
	bool CaptureFinished = false;
	
	//Mission containers
	ref array<ref Param3<string,vector,vector>> BuildingSpawns = new array<ref Param3<string,vector,vector>>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<vector> FireSpawns = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
	
	override bool IsExtended() return true;
	
	void CaptureFlagMission()
	{
		//Select primary mission
		m_MissionExtended = true;
		
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time
		
		//Mission zones
		m_MissionZoneOuterRadius = 700.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 100.0;	//meters (!Do not set outside of base radius), mission finishing distance to target object
		
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
		
		//Translate mission location name and direction
/*
		if ( MissionSettings.Opt_TranslateEWData )
		{
			m_MissionLocation = EventsWorldData.TransLoc( m_MissionLocation );
			m_MissionLocationDir = EventsWorldData.TransDir( m_MissionLocationDir );
		}
*/				
		//Set mission messages
		m_MissionMessage1 = "One of our defense positions was occupied by the Bohemian Bandits yesterday. Because of the exchange of fire between the bandits and a few survivor's alot of infected were attracted.";
		m_MissionMessage2 = "Find the defense position and capture the base flag for us. It needs atleast "+ ( CaptureTime / 60 ).ToString() +" minutes to lower the bandit's flag and raise our own.";
		m_MissionMessage3 = "The defense position is at the ** "+ m_MissionLocationDir +" side of "+ m_MissionLocation +" **\nBe careful, there are alot of infected and the bandits might be still around there!";
		
		//Rewards spawns
		Spawnpoints.Insert("-0.422 0.453 0");	//table 1
		Spawnpoints.Insert("0.486 0.453 0");	//table 1
		Spawnpoints.Insert("0 0.453 0");		//table 2
		
		//Fireplace spawns
		FireSpawns.Insert("0 0 10");
		FireSpawns.Insert("0 0 -10");
		
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

		//Spawn surrounding flag base buildings
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_WoodenCrate", "-1.5 0 4", "65 0 90"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_WoodenCrate", "1.5 0 4", "135 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_Table", "-1.5 0 -3", "15 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Roadblock_Table", "1.5 0 -3", "0 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("BarrelHoles_Blue", "1.8 0 1.8", "0 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Mil_Fortified_Nest_Watchtower", "9 0 9", "135 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Mil_Fortified_Nest_Watchtower", "-9 0 -9", "315 0 0"));		
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Wreck_Ikarus", "13 0 -2.8", "-160 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_Wreck_Uaz", "-12 0 2.8", "15 0 0"));
		BuildingSpawns.Insert( new Param3<string,vector,vector>("Land_wreck_truck01_aban1_orange", "0 0 13.5", "90 0 0"));
		
		//Clean up flag base position		
		GetGame().GetObjectsAtPosition( m_MissionPosition, 18.0 , m_ObjectList, m_ObjectCargoList );
		for ( int b=0; b < m_ObjectList.Count(); b++) 
		{
			Object FoundObject = m_ObjectList.Get(b);
			if ( FoundObject.IsItemBase() )
			{
				Print("[SMM] BeforeSpawnCleanUp :: Object deleted: "+ FoundObject );  
				GetGame().ObjectDelete( FoundObject );
			}
		}
		
		//Activate EntityChecks
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( EntityChecks, 0, true );

		//Set mission timeout
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
	
	void ~CaptureFlagMission()
	{		
		//Despawn all remaining mission objects
		if ( m_MissionObjects )
		{	
			Print("[SMM] Despawning "+ m_MissionObjects.Count() +" mission objects from old mission...");				
			for ( int i = 0; i < m_MissionObjects.Count(); i++ )
			{ 
				if ( !m_MissionObjects.Get(i) ) continue;
				else
				{
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
        float radius = 700.0;  // Example radius, you can change this value

        // Set name for the zone
        string name = "Capture the Flag PVP Zone";
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
	
	void SpawnObjects()
	{		
		//spawn flagpole (flagpole is the reference for all mission building & object spawnpoints!) 
		FlagBase = GetGame().CreateObjectEx( "StaticObj_Misc_Flagpole", m_MissionPosition, ECE_PLACE_ON_SURFACE );
		FlagBase.SetPosition( m_MissionPosition + "0 4 0" ); // Raised by 2 more units (was 3, now 5)
		FlagBase.SetOrientation( Vector( Math.RandomFloatInclusive(0,359.9), 180 , 0)); // Rotate Y by 180 to flip upright
		m_MissionObjects.Insert( FlagBase );                                            //Object 0

        //get flag height limits and offset height positions min max
        vector temp = FlagBase.ModelToWorld("0.1 0 -0.01");
        temp[1] = GetGame().SurfaceY(temp[0], temp[2]);
        PoleHighPos = temp + "0 6.5 0";
        PoleLowPos = temp + "0 1 0";
		
		//flag hight and speed calculations
		delta_height = PoleHighPos[1] - PoleLowPos[1];						//height diff min max ...[m]
		flag_speed = -( delta_height / (( CaptureTime / 2 ) * 10 ) );		//vertical speed of flag movement, v = s / t ...[m/100ms]
		
		//spawn flag
		Flag = Flag_Base.Cast( GetGame().CreateObject( DownFlag, PoleHighPos ));
		Flag.SetPosition( PoleHighPos );
		Flag.SetOrientation( FlagBase.GetOrientation() + "0 180 0");
		Flag.ShowSelection("unfolded");
		Flag.HideSelection("folded");
		Flag.SetTakeable(false);
		Flag.SetLifetime(7200);
		m_MissionObjects.Insert( Flag );												//Object 1
		
		//Spawn additional flag base buildings and objects
		for ( int i=0; i < BuildingSpawns.Count(); i++ )
		{
			Param3<string,vector,vector> BuildingDef = BuildingSpawns.Get(i);
			string BuildingType = BuildingDef.param1;
			vector BuildingPos = FlagBase.ModelToWorld( BuildingDef.param2 );
			vector BuildingOri = BuildingDef.param3;
			vector BuildingDir = FlagBase.GetDirection();
			
			BuildingPos[1] = GetGame().SurfaceY( BuildingPos[0], BuildingPos[2] );
			Object HQextra = GetGame().CreateObject( BuildingType, BuildingPos );
			HQextra.SetDirection( BuildingDir );
			HQextra.SetOrientation( HQextra.GetOrientation() + BuildingOri );
			HQextra.PlaceOnSurface();
			m_MissionObjects.Insert( HQextra );											//Object 2 to 8
		}
		
		//check darkness for location light support and spawn fireplaces
		int year, month, day, hour, minute;
		
		GetGame().GetWorld().GetDate( year, month, day, hour, minute );		
		
		Mission currentMission = GetGame().GetMission();

		if ( currentMission )
		{			
			WorldData worldData = currentMission.GetWorldData();
			if ( worldData )
			{
				float ApproxSunrise = worldData.GetApproxSunriseTime( month + ( day / 32.0 ) );
				float ApproxSunset = worldData.GetApproxSunsetTime( month + ( day / 32.0 ) );
				
				int SunRiseHour = Math.Round( ApproxSunrise + 0.25 );
				int SunSetHour = Math.Round( ApproxSunset - 0.25 );
				
				if ( MissionSettings.DebugMode )
				Print("[SMM] Test Hour is "+ hour +", SunRiseHour: "+ SunRiseHour +", SonSetHour: "+ SunSetHour );
				
				if ( hour >= SunSetHour || hour <= SunRiseHour ) 
				{
					EntityAI wood, stones;
					
					for ( int k=0; k < FireSpawns.Count(); k++)
					{
						vector CampfirePosition = FlagBase.ModelToWorld( FireSpawns.Get(k) );
						
						//fireplace
						FireplaceBase Campfire = FireplaceBase.Cast( GetGame().CreateObject( "Fireplace", CampfirePosition, ECE_PLACE_ON_SURFACE ) );
						Campfire.Synchronize();
						wood = Campfire.GetInventory().CreateAttachment("Firewood");
						ItemBase.Cast( wood ).SetQuantity(6);
						Campfire.GetInventory().CreateAttachment("WoodenStick");
						Campfire.GetInventory().CreateAttachment("Rag");

						for (int ij = 0; ij < 16; ij++)
						{
							Campfire.GetInventory().CreateAttachment("Stone");
						}

						Campfire.SetStoneCircleState(true);
						Campfire.PlaceOnSurface();
						Campfire.StartFire(true);
						m_MissionObjects.Insert(Campfire);

						// To create a stone circle (the visual effect around a fireplace), you should use the built-in method:
						

						//Cluttercutter fireplace
						Object cc_fireplace = GetGame().CreateObject( "ClutterCutterFireplace" , Campfire.GetPosition() );
						cc_fireplace.SetOrientation( Campfire.GetOrientation() );
						GetGame().RemoteObjectCreate( cc_fireplace );
						cc_fireplace.PlaceOnSurface();
						m_MissionObjects.Insert( cc_fireplace );
					}
				}
			}
		}
		
		//flip flagpole after spawning all objects
		FlagBase.SetOrientation( FlagBase.GetOrientation() + "0 180 0");
		
		Print("[SMM] Survivor Mission "+ m_selectedMission +" :: "+ m_MissionName +" ...mission deployed!");
	}
	
	void SpawnRewards()
	{
		Table1 = m_MissionObjects[4];
		Table2 = m_MissionObjects[5];
		
		for (int i=0; i < TeamPlayerCount; i++)
		{
			vector spawnpos;
			
			if ( i < 2 )
			spawnpos = Table1.ModelToWorld( Spawnpoints.Get(i) );
			else spawnpos = Table2.ModelToWorld( Spawnpoints.Get(i) );
			
			//Mission object is seachest
			MissionObject = ItemBase.Cast( GetGame().CreateObject("SeaChest", spawnpos) ); 
			
			//Get random loadout 			
			int selectedLoadout = Math.RandomIntInclusive(0,11);	//!Change randomization limit after adding new loadouts!
			
			//Spawn selected loadout items in mission object
			EntityAI weapon;
									
			if (selectedLoadout == 0)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("M4A1");
					weapon.GetInventory().CreateAttachment("M4_RISHndgrd_Green");
					weapon.GetInventory().CreateAttachment("M4_MPBttstck");
					weapon.GetInventory().CreateAttachment("M4_Suppressor");
					weapon.GetInventory().CreateAttachment("ACOGOptic");
					//weapon.GetInventory().CreateAttachment("M4_T3NRDSOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_STANAG_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
				MissionObject.GetInventory().CreateInInventory("Ammo_556x45");
				MissionObject.GetInventory().CreateInInventory("HuntingKnife");
				MissionObject.GetInventory().CreateInInventory("BakedBeansCan");
				MissionObject.GetInventory().CreateInInventory("Canteen");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 1)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("SVD");
					weapon.GetInventory().CreateAttachment("PSO11Optic");
					//weapon.GetInventory().CreateAttachment("PSO1Optic");
					//weapon.GetInventory().CreateAttachment("KazuarOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_SVD_10Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x54");
				MissionObject.GetInventory().CreateInInventory("CanOpener");
				MissionObject.GetInventory().CreateInInventory("PeachesCan");
				MissionObject.GetInventory().CreateInInventory("WaterBottle"); 
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 2)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("AKM");
					weapon.GetInventory().CreateAttachment("AK_RailHndgrd");
					weapon.GetInventory().CreateAttachment("AK_Suppressor");
					weapon.GetInventory().CreateAttachment("AK_PlasticBttstck");
					weapon.GetInventory().CreateAttachment("PSO1Optic");
					//weapon.GetInventory().CreateAttachment("KobraOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_AKM_30Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("KitchenKnife");
				MissionObject.GetInventory().CreateInInventory("SardinesCan");
				MissionObject.GetInventory().CreateInInventory("Canteen");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 3)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("FAL");
					weapon.GetInventory().CreateAttachment("Fal_OeBttstck");
					weapon.GetInventory().CreateAttachment("M4_Suppressor");
					weapon.GetInventory().CreateAttachment("M68Optic");
					//weapon.GetInventory().CreateAttachment("ReflexOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_FAL_20Rnd");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");
				MissionObject.GetInventory().CreateInInventory("MilitaryBelt");	
				MissionObject.GetInventory().CreateInInventory("M18SmokeGrenade_Yellow");	
				MissionObject.GetInventory().CreateInInventory("TacticalBaconCan");
				MissionObject.GetInventory().CreateInInventory("SteakKnife");
			}	
			if (selectedLoadout == 4)
			{
				weapon = MissionObject.GetInventory().CreateInInventory("SKS");
					weapon.GetInventory().CreateAttachment("PUScopeOptic");
					weapon.GetInventory().CreateAttachment("SKS_Bayonet");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				MissionObject.GetInventory().CreateInInventory("Ammo_762x39");
				weapon = MissionObject.GetInventory().CreateInInventory("FNX45");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");
					weapon.GetInventory().CreateAttachment("FNP45_MRDSOptic");
					weapon.GetInventory().CreateAttachment("TLRLight");
				MissionObject.GetInventory().CreateInInventory("Mag_FNX45_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");			
				MissionObject.GetInventory().CreateInInventory("Battery9V");
			}
			if (selectedLoadout == 5)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("Winchester70");
					weapon.GetInventory().CreateAttachment("HuntingOptic");		
				MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
				MissionObject.GetInventory().CreateInInventory("Ammo_308Win");
			
				weapon = MissionObject.GetInventory().CreateInInventory("CZ75");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");
					weapon.GetInventory().CreateAttachment("FNP45_MRDSOptic");
				MissionObject.GetInventory().CreateInInventory("Mag_CZ75_15Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
						
				weapon = MissionObject.GetInventory().CreateInInventory("NVGHeadstrap");
					weapon.GetInventory().CreateInInventory("NVGoggles");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("PsilocybeMushroom");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");	
				MissionObject.GetInventory().CreateInInventory("SpaghettiCan");
				MissionObject.GetInventory().CreateInInventory("StoneKnife");
				MissionObject.GetInventory().CreateInInventory("M67Grenade");					
			}
			if (selectedLoadout == 6)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("UMP45");
					weapon.GetInventory().CreateAttachment("M68Optic");
					weapon.GetInventory().CreateAttachment("PistolSuppressor");			
				MissionObject.GetInventory().CreateInInventory("Mag_UMP_25Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_UMP_25Rnd");
				MissionObject.GetInventory().CreateInInventory("Ammo_45ACP");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("Cannabis");
				MissionObject.GetInventory().CreateInInventory("CannabisSeedsPack");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");	
				MissionObject.GetInventory().CreateInInventory("TunaCan");
				MissionObject.GetInventory().CreateInInventory("RDG5Grenade");					
			}	
			if (selectedLoadout == 7)
			{			
				weapon = MissionObject.GetInventory().CreateInInventory("VSS");
					weapon.GetInventory().CreateAttachment("PSO11Optic");
					//weapon.GetInventory().CreateAttachment("PistolSuppressor");			
				MissionObject.GetInventory().CreateInInventory("Mag_VSS_10Rnd");
				MissionObject.GetInventory().CreateInInventory("Mag_VSS_10Rnd");
				MissionObject.GetInventory().CreateInInventory("AmmoBox_9x39_20Rnd");
				MissionObject.GetInventory().CreateInInventory("Battery9V");
				MissionObject.GetInventory().CreateInInventory("GardenLime");
				MissionObject.GetInventory().CreateInInventory("CannabisSeedsPack");
				MissionObject.GetInventory().CreateInInventory("WaterBottle");	
				MissionObject.GetInventory().CreateInInventory("SardinesCan");
				MissionObject.GetInventory().CreateInInventory("M18SmokeGrenade_Green");					
			}
			if (selectedLoadout == 8)
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
			if (selectedLoadout == 9)
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
			if (selectedLoadout == 10)
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
			if (selectedLoadout == 11)
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
		
			//barrel add firewood and start fire
			BarrelFire = BarrelHoles_ColorBase.Cast( m_MissionObjects[6] );
			BarrelFire.Open();
			BarrelFire.Synchronize();
			
			EntityAI barrel_wood = BarrelFire.GetInventory().CreateAttachment("Firewood"); 		
			BarrelFire.GetInventory().CreateAttachment("WoodenStick");
			BarrelFire.GetInventory().CreateAttachment("Rag");
			BarrelFire.StartFire( true );
			ItemBase.Cast( barrel_wood ).SetQuantity(6);
			
			//spawn 5 fish filets for smoking in barrel
			if ( BarrelFire.IsOpen() )	BarrelFire.Close();
			for ( i=0; i < 7; i++)
			BarrelFire.GetInventory().CreateAttachment("MackerelFilletMeat");

			Print("[SMM] Mission rewards spawned in reward container. Randomly selected loadout was "+ selectedLoadout +"." );
		}
		
		//log team work players
		for ( i=0; i < m_PlayersInZone.Count(); i++)
		Print("[SMM] Player with SteamID64: "+ m_PlayersInZone.Get(i) +" has assisted capturing the flag.");
	}
	
	void SpawnAIs()
	{
		int SpawnDlyTime = 500;
		
		//Spawn horde
		for ( int j = 0; j < HordeDensity; j++ )
		{
			//Spawn infected time delayed 500ms
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DelayedAISpawn, SpawnDlyTime, false );
			SpawnDlyTime += 500;
		}
	}
	
	void DelayedAISpawn()
	{
		//calc new spawn position
		float x = Math.RandomFloatInclusive( 20 , 50 );
		float y = Math.RandomFloatInclusive( 20 , 50 );	
		int Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice > 4 ) x *= -1.0;
		Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice < 5 ) y *= -1.0;
		vector NewPosVector = { x, 0, y };
		
		//spawn infected
		m_MissionAIs.Insert( GetGame().CreateObject( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, false, true ));
	}
	
	override void ObjDespawn() 
	{	
		//nothing to despawn
	}
	
	override void ExtendMission()
	{
		//select secondary mission
		m_MissionExtended = false;
		
		//Set new mission messages
		m_MissionMessage1 = "Allright survivor, it seems you have found the defence position "+ m_MissionLocationDir +" of "+ m_MissionLocation +". There are probably alot of infected, try to get in there safely and hold the position.";
		m_MissionMessage2 = "Be aware, Bohemian Bandits could counter attack the defence position. If you could capture it completely, there will be some interesting equipment for you probably.";
		m_MissionMessage3 = "The flag was already captured by "+ FlagCaptureProgress +"%. Hold on, you will make it.";
		
		//init Messenger for new messages
		m_MsgNum = 1;					//skip msg 0, begin with msg 1
		m_MsgChkTime = m_MissionTime;
		MsgCutoff = false;
		
		//increase mission MissionTimeout
		m_MissionTimeout = m_MissionTime + CaptureTime + 900;  
		
		//deployment settings & init for extended mission
		m_MissionZoneOuterRadius = 20.0;
		m_MissionZoneInnerRadius = 0.0001;
	}
			
	override void MissionFinal()
	{	//When player enters last mission target zone	

		//Finish mission
		m_RewardsSpawned = true;
		m_MsgNum = -1;
		m_MsgChkTime = m_MissionTime + MsgDlyFinish;			
	}
	
	override void PlayerChecks( PlayerBase player )
	{	//Executed continuously only on players in the mission zone. Avoid complex calculations or big data processing!
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
	
	void EntityChecks()
	{	//Executed continuously while mission is active and triggered. Avoid complex calculations or big data processing!
		int crntTime = GetGame().GetTime();
		
		//flag movement & capture progress calculation
		if ( !m_MissionExtended && !CaptureFinished && FlagBase && Flag && m_PlayersInZone.Count() > 0 )
		{			
			if ( !FlagElevationChkTime ) FlagElevationChkTime = crntTime;
			
			if ( crntTime >= FlagElevationChkTime + 100 )	//checking every 100ms
			{
				vector FlagPos = Flag.GetPosition();
				vector OldFlagPos;
				vector OldFlagOri;
				float flag_height = FlagPos[1];
			
				//check if flag is at lowest position
				if ( flag_height < PoleLowPos[1] && flag_speed < 0 ) 
				{
					flag_speed *= -1;
					
					//replace flag
					OldFlagPos = FlagPos;
					OldFlagOri = Flag.GetOrientation();
					
					GetGame().ObjectDelete( Flag );
					
					Flag = Flag_Base.Cast( GetGame().CreateObject( UpFlag, OldFlagPos ));
					Flag.SetPosition( OldFlagPos );
					Flag.SetOrientation( OldFlagOri );
					Flag.ShowSelection("unfolded");
					Flag.HideSelection("folded");
					Flag.SetTakeable(false);					
				}
				
				//check if flag is at highest position
				if ( flag_height > PoleHighPos[1] && flag_speed > 0 ) 
				{
					flag_speed *= -1;
					
					//check if flag has desired type
					if ( Flag.GetType() == UpFlag )
					{
						CaptureFinished = true;
						
						TeamPlayerCount = m_PlayersInZone.Count();
						if ( TeamPlayerCount > 3 ) TeamPlayerCount = 3;
						
						//spawn rewards & finish mission
						SpawnRewards();
						MissionFinal();
						
						//Deactivate EntityChecks
						GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove( EntityChecks );
					}
				}
				
				//update flag position
				Flag.SetPosition( FlagPos + Vector(0,flag_speed,0) );
				
				FlagElevationChkTime = crntTime;
			}

			//flag capture progress 0-100% (only for the progress in the message)
			if ( FlagPos[1] - PoleLowPos[1] > 0 && FlagPos[1] - PoleHighPos[1] < 0 )
			{
				if ( flag_speed < 0 ) 	//when lowering flag
				FlagCaptureProgress = 50 + ( FlagPos[1] - PoleLowPos[1] ) / ( -delta_height * 2 / 100 ); 
				else					//when raising flag
				FlagCaptureProgress = 50 + ( FlagPos[1] - PoleLowPos[1] ) / ( delta_height * 2 / 100 );
			}
			
			//refresh message 3 (capture progress)
			if ( !m_MissionExtended )
			m_MissionMessage3 = "The flag was already captured by "+ FlagCaptureProgress +"%. Hold the position, you will make it.";
		}
		
		//refuel fireplaces (!does only work when fireplaces are element 2 and 4 in m_MissionObjects array!)
		if ( !CaptureFinished )
		{
			if ( !FireplaceSpendChkTime ) FireplaceSpendChkTime = crntTime;
			
			if ( crntTime >= FireplaceSpendChkTime + 300000 )	//checking every 5min
			{
				for ( int i=0; i < m_MissionObjects.Count(); i++)
				{
					Object AnyObject = m_MissionObjects.Get(i);
					
					if ( AnyObject && AnyObject.IsFireplace() )  //Inherited( FireplaceBase ) )
					{
						FireplaceBase Campfire = FireplaceBase.Cast( AnyObject ); //m_MissionObjects.[i * 2 + 2] ); 
						if ( Campfire )
						{
							for (int j=0; j < Campfire.GetInventory().AttachmentCount(); ++j )
							{
								ItemBase attachment = ItemBase.Cast( Campfire.GetInventory().GetAttachmentFromIndex(j) );
								if ( attachment && attachment.GetType() == "Firewood" )
								{
									if ( attachment.GetQuantity() < 4 )		attachment.SetQuantity(6);
								}
								
								//to delete
								Print("[SMM] "+ AnyObject.GetType() +" Fireplace "+ attachment.GetType() +", quantity: "+ attachment.GetQuantity() );
							}												
						}
					}
				}
				
				FireplaceSpendChkTime = crntTime;
			}
		}
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
	/*
	void CreateCircularPatrolRoute(eAIGroup group, vector center, float radius)
	{
		group.AddWaypoint(center + Vector(radius, 0, 0));  // East
		group.AddWaypoint(center + Vector(0, 0, radius));  // North
		group.AddWaypoint(center + Vector(-radius, 0, 0)); // West
		group.AddWaypoint(center + Vector(0, 0, -radius)); // South
		group.SetWaypointBehaviour(eAIWaypointBehavior.LOOP);
	}
	*/

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
		CreateRandomIntersectingPatrolRoute(group, m_MissionPosition, 700); // 25m radius

		// Verify and Set Formation
		Print(string.Format("[SMM] Spawned patrol: %1/%2 AI (Leader: %3) Loadout:%4 Faction:%5", spawnedCount, squadSize, leader.GetType(), loadout, faction.GetName()));
		group.SetFormation(CreateFormation(formationType));
		
	}
	#endif
	#endif 
		
	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		if ( m_MissionPosition != "0 0 0" )
		{
			if ( m_MissionExtended )
			{	//only if primary is selected
				//Call spawners	
				GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
				GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnAIs, 5000, false );


				#ifdef ENFUSION_AI_PROJECT
				#ifdef EXPANSIONMODAI
				if (SpawnPatrolEnabled)
				{
					Print("[SMM] Expansion AI Patrol Turned On");
					int BotDlyTime = 600000;
					// 25min delay before first patrol spawn
					GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SpawnPatrol, BotDlyTime, false );
				}
				if (!SpawnPatrolEnabled)
				{
					Print("[SMM] Expansion AI Patrol Turned Off");
				}
				#endif
				#endif

			}
			return true;
		}
		else 
		{
			Print("[SMM] ERROR : Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}

}