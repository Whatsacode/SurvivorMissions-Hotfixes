class EscortDefendMission extends SurvivorMissions
{
	//IMPORTANT !!!
	//This mission requires the "Expansion AI" MOD to be installed. Download on Steam -> https://steamcommunity.com/sharedfiles/filedetails/?id=2542638513

	// Activate PVEZ PVP Zone
	bool SpawnPVPZone = false;

	#ifdef PVEZ
	#ifdef PVEZ

	// PVEZ PVP Zone Globals
	private PVEZ_Zone m_PVPZone; 
	private string m_PVPZoneName;

	#endif
	#endif

    bool secondwavespawned = false;
	bool firstwavespawned = false;
	bool Secondtrigger = false;
	bool Finaltrigger = false;
	bool FinalDest = false;
	bool turnOffCheck = false;

	//Mission related entities
	ItemBase MissionObject;
	Object MissionBuilding;
	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI
	eAIGroup MissionGroup;
	eAIBase EscortBot;
	#endif
	#endif
	Grenade_Base TargetSmoke;
	string loadout = "PoliceLoadout.json";
	UndergroundStash stash;

	//Mission parameters
	int ExtendedTimout = 1800;				//seconds, mission duration time for extended mission	
	int MsgDlyFinish = 30;					//seconds, message delay time after player has finished mission
	int HordeDensity = 10;					//infected, spawned in circle area
	int Killrequiredwave1 = 2;				// # of Kills to push to Wave 2
	int Killrequiredwave2 = 5;				// # of Kills to Push to Final
	int Killrequiredfinal = 10;				// # of Kills to Trigger FinalChase

	//Mission timeout
  	int MissionCutoffTime;

	//mission vars
	string SurvivorName;
	int EscortWalkChkTime;
	int StuckChkTime;
    vector currentPosition;
    vector lastPosition;
	vector FinalDestination;

	bool EscortBotFollowsLeader = false;
	bool EscortLeftHouse = false;
	bool BotFound = false;

	// The model vector of the Police Station to Enable Defense Part of Mission.
	vector EscortFinalPos = "1.25 -2.45 -3.07";

	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<string> InfectedTypes = new array<string>;
    ref array<vector> InfectedSpawns = new array<vector>;
    ref array<Object> DeadInfected = new array<Object>;
	ref TVectorArray IdleWaypoints = new TVectorArray;
	
	override bool IsExtended() return true;
	
	// Mission Constructor
	void EscortDefendMission()
	{
		#ifndef ENFUSION_AI_PROJECT
		#ifndef EXPANSIONMODAI

		Print("[SMM] ERROR :: Escort mission requires the 'Enfusion AI' MOD, but this mod is not installed!");

		#endif
		#endif
		
		//Select primary mission
		m_MissionExtended = true;
						
		//Mission mission timeout
		m_MissionTimeout = 2700;			//seconds, mission duration time

		//Mission zones
		m_MissionZoneOuterRadius = 70.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 3.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		SurvivorName = "Lt. Lehm";
		
		//Mission group
		EscortBotFollowsLeader = false;
		
			
		//Activate EntityChecks

		#ifdef ENFUSION_AI_PROJECT
		#ifdef EXPANSIONMODAI

    	GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( EntityChecks, 10, true );
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( PlayerChecks, 10, true );

		#endif
		#endif
	
		//Set mission messages
        m_MissionMessage1 = SurvivorName +", once a high ranking officer within the Chernarus Police Dept. Has managed to survive for quite some time after the accident.";
        m_MissionMessage2 = "He believes that there may be some left over supplies that himself and you could possibly use. He is looking for a escort though.";
        m_MissionMessage3 = "If you are up to the task, he is holding out in a house \n** "+ m_MissionLocationDir +" "+ m_MissionLocation +" **\n";
		

		//Infected types for secondary mission position
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

		MissionCutoffTime = MissionSettings.RestartCycleTime - (m_MissionTimeout + MissionSettings.DelayTime + ExtendedTimout );
		
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

	//Mission Deconstructor
	void ~EscortDefendMission()
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
		#ifdef ENFUSION_AI_PROJECT
		#ifdef EXPANSIONMODAI
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
		#endif
		#endif
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
        float radius = 250.0;  // Example radius, you can change this value

        // Set name for the zone
        string name = "Psilo PVP Zone";
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

	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI

	//Spawning the Escort Bot	
	void SpawnAIs()
	{
		if ( m_MissionExtended )
		{

			EscortBot = eAIBase.Cast(GetGame().CreateObject(GetRandomAI(), m_MissionPosition));

			EscortBot.SetPosition(m_MissionPosition);
			EscortBot.PlaceOnSurface();

			eAIFaction Passive = new eAIFactionPassive();

			EscortBot.SetMovementSpeedLimits(2.0, 3.0);

			MissionGroup = EscortBot.GetGroup();
			MissionGroup.SetFaction(Passive); // <--- Also set group faction

			// Ensure EscortBot is a member of the group
			if (!MissionGroup.IsMember(EscortBot))
				MissionGroup.AddMember(EscortBot);

			//Add escort bot to MissionAI's
			m_MissionAIs.Insert(EscortBot);
			
			//Set Bot as Leader of Group
			MissionGroup.SetLeader(EscortBot);
			//Set Bot as Escort
			EscortBotFollowsLeader = false;
			EscortLeftHouse = false;

			ExpansionHumanLoadout.Apply(EscortBot, "PoliceLoadout.json", true);

			if ( MissionSettings.DebugMode )
			{
				string aigroupfaction = EscortBot.GetGroup().GetFaction().GetName();
				
				Print (" AI faction is: "+ aigroupfaction);
				Print(" Number in group: " + MissionGroup.Count());
			}
		
		}
		else
		{
			if ( EscortBotFollowsLeader )
			{
				//spanwn no AIs at secondary
			}
		}
	}
	
	#endif
	#endif

	void DelayedAISpawn()
	{
		//calc new spawn position
		float x = Math.RandomFloatInclusive( 10 , 20 );
		float y = Math.RandomFloatInclusive( 10 , 20 );	
		int Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice > 4 ) x *= -1.0;
		Dice = Math.RandomIntInclusive( 0, 9);
		if ( Dice < 5 ) y *= -1.0;
		vector NewPosVector = { x, 0, y };
		
		//spawn infected
		Object horde = GetGame().CreateObject( InfectedTypes.GetRandomElement(), m_MissionPosition + NewPosVector, false, true );
		m_MissionAIs.Insert( horde );
	}	
	
	void SpawnObjects()
	{
		if ( m_MissionExtended )
		{
			//spawn no Objects at primary
		}
		else
		{
			if ( EscortBotFollowsLeader )
			{
				//spawn no Objects at secondary 
			}
		}
	}

	override void ObjDespawn()
	{
		//nothing to despawn
	}
	
	override void ExtendMission()
	{
		//init Messenger for new messages
		m_MsgNum = 0;					//skip msg 0, begin with msg 1
		m_MsgChkTime = m_MissionTime + 15;
		MsgCutoff = true;
		
		//increase mission MissionTimeout
		m_MissionTimeout = m_MissionTime + ExtendedTimout;  
		
		//deployment settings & init for extended mission
		m_MissionZoneOuterRadius = 50.0;
		m_MissionZoneInnerRadius = 0.5;
		
		//Get building position from building type and location
		if ( EventsWorldData.GetBuildingsAtLoc("Land_Village_PoliceStation", m_MissionDescription[3], ExtendedPosList ))
		{
			bool SecBuildFound = false;
			
			//Get MissionPosition for MissionBuilding from secondary mission
			m_MissionPosition = ExtendedPosList.GetRandomElement();						
			GetGame().GetObjectsAtPosition( m_MissionPosition, 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int m=0; m < m_ObjectList.Count(); m++ )
			{ 
				Object FoundObject = m_ObjectList.Get(m);
				if ( FoundObject.GetType() == "Land_Village_PoliceStation" )
				{	
					MissionBuilding = FoundObject;			 
					m_MissionDescription[3].Replace("_"," ");
					SecBuildFound = true;					
					break;
				}	
			}
			if ( !SecBuildFound )
			Print("[SMM] ERROR :: Can't find object Land_Village_PoliceStation in "+ m_MissionDescription[3] +" on the map!");
			else	//open all doors at police station
			{
				Building police = Building.Cast( MissionBuilding );
				
				for ( int i=0; i < 15; i++)
				{
					if ( police.IsDoorLocked(i) )		police.UnlockDoor(i);
					if ( !police.IsDoorOpen(i) )		police.OpenDoor(i);
				}
			}
		}
		else Print("[SMM] ERROR :: Can't find MissionBuilding Medical Center in "+ m_MissionDescription[3] +" from EventsWorldData!");	
	}

	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI

	void MarkTargetSmoke()
	{
		//spawn & activate smokegrenade at target position
		TargetSmoke = Grenade_Base.Cast( GetGame().CreateObject( "M18SmokeGrenade_Yellow" , EscortBot.GetPosition() + "2 1 2" ));		
		TargetSmoke.Unpin();
	}

	#endif
	#endif	


	// Here is where we spawn our first wave - Reference the DelayedAI Spawn from Above.
	void Firstwave()
	{
		//Spawn horde
		int SpawnDlyTime = 500;
		
		for ( int j = 0; j < HordeDensity; j++ )
		{
			//Spawn infected time delayed 500ms
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DelayedAISpawn, SpawnDlyTime, false );
			SpawnDlyTime += 500;	
		}
	}

	// Once the First Wave as been Cleared We will start this.
    void SecondWave()
	{
		int Dice1 = Math.RandomIntInclusive( 0, 9);  
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
	#ifdef ENFUSION_AI_PROJECT
	#ifdef EXPANSIONMODAI
		
	override void PlayerChecks( PlayerBase player )
	{
		if ( m_MissionExtended )
		{
			if ( player && !EscortBotFollowsLeader && !m_RewardsSpawned && !firstwavespawned && player.IsAlive() )
			{
				if ( !MissionGroup.IsMember( player ) ) 
				{		
					if ( vector.Distance( m_MissionPosition , player.GetPosition()) < m_MissionZoneInnerRadius + 3.0 )
					{
						if ( EscortLeftHouse )
						{
							//Reset secondary mission zone
							m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(0) );
							m_MissionZoneOuterRadius = 120.0;
							m_MissionZoneInnerRadius = 2.0;
						}
						
						Print("[SMM] Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" was set to group leader. The escort bot follows him now.");

						EscortBotFollowsLeader = true;
						
						//Add player to MissionGroup
						MissionGroup.SetLeader( player );
						player.SetGroup( MissionGroup );
						
						//Set formation to file for following mode
						MissionGroup.SetFormation( new eAIFormationFile );
						
						if ( MissionSettings.DebugMode )
						{
							if ( MissionGroup.IsMember( player ) )
							Print("[SMM] Player is member.");
							else Print("[SMM] Player is not a member.");
							
							if ( MissionGroup.IsMember( EscortBot ) )
							Print("[SMM] MissionAI is member.");
							else Print("[SMM] MissionAI is not a member.");
						
							Print("[SMM] Player "+ MissionGroup.GetLeader().GetIdentity().GetPlainId() +" is leader. MemberCount = "+ MissionGroup.Count() );
							Print("[SMM] Old formation was "+ MissionGroup.GetFormation().ClassName() );
						}
					}
				}
			}
		}		
	}
	
	void FinalChase()
	{

		//Bot talks about last destination with player
		string icon = "set:dayz_gui image:notification_friend";		
		string msgs[7] = { "Not only was the large stash of provsions, I believe to be there was not... There was that Horde.",
		"Im sorry, this mission was a failure in the terms of finding the supplies we were looking for.",
		"I want you to have at least something for assisting me in checking out this Police Station.",
		"I have a stash of supplies nearby I will take you too, Please Keep up and Keep me safe!!!",
		"Once again thank you. If you could do me a one last favor. It seems I may have gotten infected... ",
		"I dont feel there is much time. If you could kill me and bury my body when we get there...",
		"I always enjoyed this place, Thought it might double as a grave site as well."  };
			
		int timedelay = 0;
		
		for ( int i=0; i < 7; i++)
		{
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( NotificationSystem.SendNotificationToPlayerIdentityExtended, timedelay, false, MissionGroup.GetMember(1).GetIdentity(), 10.0, SurvivorName + ":", msgs[i], icon );
			timedelay += 12000; 
		}

		//update group waypoints
		MissionGroup.ClearWaypoints();
		Print("[SMM] Waypoints cleared.");
		
		//try to get a nearby random position on land, retry if under water
		for ( int k=0; k < 20 ; k++)
		{
			//Get random nearby location
			float x = Math.RandomFloatInclusive( 200 , 500 );
			float y = Math.RandomFloatInclusive( 200 , 500 );
			int Dice = Math.RandomIntInclusive(0,9);
			if ( Dice > 4 ) x *= -1;
			Dice = Math.RandomIntInclusive(0,9);
			if ( Dice < 5 ) y *= -1;
			vector RandomNearbyDest = MissionBuilding.ModelToWorld( Vector( x, 0, y ) );
			RandomNearbyDest[1] = GetGame().SurfaceY( RandomNearbyDest[0], RandomNearbyDest[2]);
			
			if ( RandomNearbyDest[1] > 0.0 )
			break;
			else
			Print("[SMM] WARNING : Attempt "+ (k + 1) +" - The random stash position is under sea level! "+ RandomNearbyDest.ToString() );
			if ( k == 19 )
			Print("[SMM] ERROR : Couldn't find a proper position for the mission stash after 20 attempts!");
		}

		FinalDestination = RandomNearbyDest;
		
		//update group waypoints
		MissionGroup.AddWaypoint( RandomNearbyDest );
		Print("[SMM] Waypoint for Loot and Mission End Set.");
		Print("[SMM] Bot Pathing Looping False");

		//Spawn shovel at stash pos
		Object shovel = GetGame().CreateObject( "Shovel", RandomNearbyDest );
		shovel.SetPosition( RandomNearbyDest + "0.3 0.5 0.3");
		shovel.SetOrientation("0 180 0");

		//Create underground stash  
		stash = UndergroundStash.Cast( GetGame().CreateObject( "UndergroundStash", RandomNearbyDest ));
		stash.PlaceOnGround(); 
		Print(" Orientation UndergroundStash : "+ stash.GetOrientation().ToString() );
		vector pos = stash.GetPosition();
		vector ori = stash.GetOrientation();
		
		if ( ori[2] == -180 || ori[2] == 180 )
		{	
			ori[2] = 0;
			stash.SetOrientation( ori );
		}
		
		//Spawn seachest in stash
		MissionObject = ItemBase.Cast( stash.GetInventory().CreateInInventory("SeaChest"));		
		
		//Clutter cutter stash 	
		Object cc_stash = GetGame().CreateObject( "ClutterCutter2x2" , MissionObject.GetPosition() );
		GetGame().RemoteObjectCreate( cc_stash );		
		
		//Insert mission objects into mission objects list
		m_MissionObjects.Insert( MissionObject );
		m_MissionObjects.Insert( stash );		 
		m_MissionObjects.Insert( cc_stash );		

		//Get random loadout 
		int selectedLoadout = Math.RandomIntInclusive( 0, 9);	//!change randomization limit after adding new loadouts!	

		//Spawn selected loadout items in mission object
		EntityAI weapon;
				
		if ( selectedLoadout == 0 )
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
					
		Print("[SMM] Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );			
		
		Print("[SMM] Randomly selected rewards were added plus  Mission rewards spawned in underground stash at poition "+ RandomNearbyDest.ToString() );	

	}

	override void MissionFinal()
	{
		//not used in this template
	}
	
	PlayerBase TempLeader;
	vector TempLeaderPos;

	void EntityChecks()
	{
		//Escort bot checks 
		if ( EscortBot && EscortBot.IsAlive() )
		{
			//if bot is dead, abbort mission
			if ( !EscortBot.IsAlive() && !m_MissionTimedOut && !m_RewardsSpawned )
			{
				m_MissionTimedOut = true;
				EscortBotFollowsLeader = false;
				m_MsgNum = -3;
				m_MsgChkTime = m_MissionTime + 3;
				
				EscortBot = null;
				
				Print("[SMM] The mission target died, the mission is set to 'Timed Out'. Sending Mission Failed message via radio.");
				return;
			}

			//Get server runtime for timer functions
			int crntTime = GetGame().GetTime();
			
			//Bot Anti-stuck
			if ( !lastPosition ) lastPosition =  EscortBot.GetPosition(); //init
			
			if ( crntTime >= StuckChkTime + 1000 && !m_RewardsSpawned )
			{
				currentPosition = EscortBot.GetPosition();

				if ( vector.Distance( currentPosition, lastPosition ) < 0.2 )
				{
					//Anti-Stuck in following mode
					if ( !TempLeader && vector.Distance( currentPosition, MissionGroup.GetLeader().GetPosition() ) > 4 )
					{						
						TempLeader = MissionGroup.GetLeader();
						TempLeaderPos = TempLeader.GetPosition();
						
						if ( TempLeader )
						Print("[SMM] Bot stuck detected. Temporary set the bot as leader. Leader was "+ TempLeader.GetIdentity().GetPlainId() );
						else Print("[SMM] Leader is NULL before temporary adding Bot as leader.");
						
						MissionGroup.SetLeader( EscortBot );
						
						MissionGroup.ClearWaypoints();
						MissionGroup.AddWaypoint( TempLeaderPos );

					}
					else if ( TempLeader && vector.Distance( currentPosition, TempLeaderPos ) < 3.0 )
					{
						Print("[SMM] Bot reached the leader's last position. Player "+ TempLeader.GetIdentity().GetPlainId() +" is now leader again.");
						MissionGroup.SetLeader( TempLeader );
						TempLeader = null;
						MissionGroup.ClearWaypoints();
						MissionGroup.SetFormation( new eAIFormationFile );
					}
					
					//Anti-Stuck in chasing Mode
					if ( Finaltrigger )
					{
						int Dice1 = Math.RandomIntInclusive(0,9);
						int Turn = 160;
						if ( Dice1 > 4 ) Turn *= -1;
						vector VecOri = { Turn , 0, 0 };
						
	                    EscortBot.SetOrientation( EscortBot.GetOrientation() + VecOri );
					}
				}
				
				//rem
				lastPosition = currentPosition;
				StuckChkTime = crntTime;
			}
						
			//Escort bot joins the group leader and starts a conversation
			if ( EscortBotFollowsLeader && !m_RewardsSpawned ) 
			{
				if ( vector.Distance( EscortBot.GetPosition(), MissionGroup.GetLeader().GetPosition() ) < 6 && MissionGroup.GetLeader().IsAlive() && !BotFound )
				{

					Print("[SMM] Sending Found msg");

					BotFound = true;
					EscortLeftHouse = true;
					
					//reset secondary message to escort mode
					if ( EscortLeftHouse )
					{
						m_MissionMessage1 = "Allright survivor, you have found "+ SurvivorName +". Protect his life under all circumstances. Be aware of infected attracted by your shots.";
						m_MsgChkTime = m_MissionTime + 15;
					}
						
					// Set GrpLeaderName to the player who triggered the inner mission zone
					string GrpLeaderName = "Survivor";
					PlayerBase triggeringPlayer;
					if (m_PlayersInZone)
					{
						for (int i = 0; i < m_PlayersInZone.Count(); i++)
						{
							PlayerBase playerPt;
							string playerId = m_PlayersInZone.Get(i);
							foreach (Man man : ClientData.m_PlayerBaseList)
							{
								PlayerBase testPlayer = PlayerBase.Cast(man);
								if (testPlayer && testPlayer.GetIdentity() && testPlayer.GetIdentity().GetId() == playerId)
								{
									playerPt = testPlayer;
									break;
								}
							}
							if (playerPt && vector.Distance(m_MissionPosition, playerPt.GetPosition()) < m_MissionZoneInnerRadius + 3.0)
							{
								triggeringPlayer = playerPt;
								break;
							}
						}
					}
					if (triggeringPlayer && triggeringPlayer.GetIdentity())
					{
						GrpLeaderName = triggeringPlayer.GetIdentity().GetName();
					}
					else if (MissionGroup && MissionGroup.GetLeader() && MissionGroup.GetLeader().GetIdentity())
					{
						GrpLeaderName = MissionGroup.GetLeader().GetIdentity().GetName();
					}
					
					string icon = "set:dayz_gui image:notification_friend";
					string msg = "Hello "+ GrpLeaderName +"!\nI am glad that you found me. "+ m_MissionInformant +" told me about you. Please bring me to the Police Station there should be some supplies on the 2nd Floor. I will follow you!";
					NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 8.0, SurvivorName + ":", msg, icon );				
				}
				if ( !MissionGroup.GetLeader().IsAlive() && !m_RewardsSpawned && !turnOffCheck ) 
				{
					Print("[SMM] The group leader died. Removing leader from group and setting Escort to idle.");
					EscortBotFollowsLeader = false;
					MissionGroup.RemoveMember( MissionGroup.GetIndex( MissionGroup.GetLeader() ));
					MissionGroup.SetLeader( EscortBot );
				
					//remove all members of the group except leader escort bot
					for ( int k=1; k < MissionGroup.Count(); k++) 
					MissionGroup.RemoveMember(k);

					m_MissionZoneOuterRadius = 50.0;
					m_MissionZoneInnerRadius = 0.0001;	
					
					//mark with smoke
					MarkTargetSmoke();
				}								
			}
			
			//Escort bot walks around in finding mode
			if ( !EscortBotFollowsLeader )
			{
				//get new destination every 10 sec
				if ( crntTime >= EscortWalkChkTime + 10000 )
				{
					//bot walks slowly in idle mode
					//if ( EscortBot.SetMovementSpeedLimits(1.0, 1.0) )
					{
						//EscortBot.SetMovementSpeedLimits(1.0, 1.0);
						//EscortBot.SetPosition( EscortBot.GetPosition() + "0 0.3 0");
					}
					
					//Get random nearby location
					float x = Math.RandomFloatInclusive( 2.0, 5.0);
					float y = Math.RandomFloatInclusive( 2.0, 5.0);
					int Dice = Math.RandomIntInclusive(0,9);
					if ( Dice > 4 ) x *= -1;
					Dice = Math.RandomIntInclusive(0,9);
					if ( Dice < 5 ) y *= -1;
					vector RandomNearbyDest = EscortBot.ModelToWorld( Vector( x, 0, y ) );
					RandomNearbyDest[1] = GetGame().SurfaceY( RandomNearbyDest[0], RandomNearbyDest[2]);
					
					//update goup waypoints
					MissionGroup.ClearWaypoints();
					MissionGroup.AddWaypoint( RandomNearbyDest );
				
					EscortWalkChkTime = crntTime;
				}
				
				if ( EscortLeftHouse )
				{
					//update mission position
					m_MissionPosition = EscortBot.GetPosition();
					string Coords = Math.Round( m_MissionPosition[0] ).ToString() +" / "+ Math.Round( m_MissionPosition[2] ).ToString();
					
					//set secondary messages to finding mode
					m_MissionMessage1 = "It seems "+ SurvivorName +" has lost his leader. He has marked his location with yellow smoke anywhere around the coordinates:\n** "+ Coords +" **\nTry to find him there and hurry up!";
				}
				//renew yellow smoke after previous grenade smoked out
				if ( TargetSmoke && TargetSmoke.GetHealth() == 0 )	
				{
					GetGame().ObjectDelete( TargetSmoke );
					MarkTargetSmoke();
				}							
			}
			else m_MissionPosition = EscortBot.GetPosition();
		}

		// if the mission is extended do this.
		if ( !m_MissionExtended && EscortBot && !m_RewardsSpawned )
		{
			//Escort Bot to 2nd POS - DEFEND HIM for 2 Waves! - !!!!
            if ( MissionBuilding && m_PlayersInZone.Count() > 0 && !m_RewardsSpawned )
            {
              if ( !EscortBot ) Print("[SMM] ERROR :: EscortBot is Null after Esort part was accomplished!");
              if ( !MissionBuilding ) Print("[SMM] ERROR :: MissionBuilding is Null after Esort part was accomplished!");

              if ( vector.Distance( EscortBot.GetPosition(), MissionBuilding.ModelToWorld(EscortFinalPos) ) < 20.0 && MissionGroup.GetLeader() != EscortBot && !FinalDest )
				{   
					Print("[SMM] Player has successfully escorted the Bot to the requested position.");

					turnOffCheck = true;
					
					//Set Bots Speed Limit
					//MissionGroup.SetSpeedLimit(1);
					
					// Set the Escort Bot as the Leader.
					MissionGroup.SetLeader( EscortBot );
			
					//update goup waypoints
					MissionGroup.ClearWaypoints();
					Print("[SMM] Waypoints cleared. Adding indoor waypoint. Bot is now on his way to 2nd floor office.");
					MissionGroup.AddWaypoint( MissionBuilding.ModelToWorld( EscortFinalPos ));
					
					//bot walks slowly to 2nd floor
					//if ( !EscortBot.SetMovementSpeedLimits(1.0, 1.0) )
					//{
						//EscortBot.SetMovementSpeedLimits(1.0, 1.0)
						//EscortBot.SetPosition( EscortBot.GetPosition() + "0 0.3 0");
					//}
				}
									
				// Once Bot is at office desk - Print Message - Start Firstwave Spawning
				if ( vector.Distance( EscortBot.GetPosition(), MissionBuilding.ModelToWorld(EscortFinalPos) ) < 3.0 )   
				{
					//First Wave Spawning only if First Wave isnt True and RewardSpawned Isnt True.
					if(!firstwavespawned && !m_RewardsSpawned)
					{

						//Set new bot messages in police station
						icon = "set:dayz_gui image:notification_friend";	
						string message1 = "We made it!, Thank you. Tell "+ m_MissionInformant +" that if we meet in person, I owe him a drink. ";
						string message2 = "There should be supplies up here. Unless it was taken by another survivor.\nWait... What's that Sound. I think I hear a Horde Outside!!!!\n";
						string message3 = "Get down stairs, Close the doors to the building and prepare to defend us!!!";
						
						NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message1 + message2 + message3, icon );

						
						firstwavespawned = true;
						Firstwave();
						Print("[SMM] Spawning Wave 1");

					}					
				}
			}

			//Here we setup our counting of deadinfected.
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
			icon = "set:dayz_gui image:notification_friend"; 
			string message;
			
			if ( DeadInfected.Count() >= Killrequiredwave1 && !secondwavespawned && !m_RewardsSpawned && turnOffCheck )
			{
				Print("[SMM] The players in the mission zone successfully killed the first requested amount of "+ Killrequiredwave1 +" infected.");
				SecondWave(); 
				secondwavespawned = true;
				message = "That's it "+ GrpLeaderName +"!\nI am glad that you are here!! "+ m_MissionInformant +" was right to send you!!!\nJust a little more now!";
				           
				NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message, icon );
			}
			
			//now we check if the array has the wanted amount of dead infected ( I use 5 less than spawn in case a few get stuck in a rock) and once met proceed to Final Chase.
			if ( DeadInfected.Count() >= Killrequiredwave2 && !m_RewardsSpawned && !Secondtrigger && turnOffCheck )
			{
				Secondtrigger = true;
				Print("[SMM] The players in the mission zone successfully killed the 2nd requested amount of "+ Killrequiredwave2 +" infected.");
				message = "That's it"+ GrpLeaderName +"!\nI think that gave us the opening we needed!! Let's Head outside and finish the rest off!!!";
				           
				NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message, icon );

			}
			//now we check if the array has the wanted amount of dead infected ( I use 5 less than spawn in case a few get stuck in a rock) and once met proceed to Final Chase.
			if ( DeadInfected.Count() >= Killrequiredfinal && !m_RewardsSpawned && !Finaltrigger && turnOffCheck )
			{
				Finaltrigger = true;
				Print("[SMM] The players in the mission zone successfully killed the 2nd requested amount of "+ Killrequiredfinal +" infected.");
				message = "Ok "+ GrpLeaderName +"!\n Let's get out of here!!!!";
				           
				NotificationSystem.SendNotificationToPlayerIdentityExtended( MissionGroup.GetLeader().GetIdentity() , 10.0, SurvivorName + ":", message, icon );
				
				//send the bot to the final destination / stash position
				FinalChase();
			}		
			
			if ( FinalDestination && vector.Distance( EscortBot.GetPosition(), FinalDestination ) <= 3.0 && !FinalDest  )
			{
				//finish mission
				Print("[SMM] Bot Reached FinalDestination - ENDING MISSION");
				GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Remove( EntityChecks );
				m_RewardsSpawned = true;
				FinalDest = true;
				m_MsgNum = -1;
				m_MsgChkTime = m_MissionTime + MsgDlyFinish;

			}
		}
	}

	override bool DeployMission()	
	{	//When first player enters the mission zone (primary/secondary)
		if ( m_MissionPosition != "0 0 0" )
		{
			//Call spawners
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnObjects );
			GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Call( this.SpawnAIs );
			return true;
		}
		else 
		{
			Print("[SMM] ERROR : Side Mission position was rejected or doesn't exist. MissionPosition is NULL!");
			return false;
		}
	}

	#endif
	#endif	
	
}

