class FishBanditMission extends SurvivorMissions											
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
	Object MissionBuilding;
	TentBase TentObject;
	FireplaceBase MissionCampfire;

	//Mission timeout
  	int MissionCutoffTime;

	//Mission parameters
	int FishCount1;				//pieces, requested fish amount
	string FishType1;
	int ExtendedTimout = 1800;				//seconds, mission duration time for extended mission
	int MsgDlyFinish = 60;					//seconds, message delay time after player has finished mission
	
	//Mission containers
	ref array<vector> ExtendedPosList = new array<vector>;
	ref array<vector> Spawnpoints = new array<vector>;
	ref array<string> FishTypes = {"Mackerel","Carp","Sardines","Acanthopagrus","Herring","Bitterlings","Chub","Mulloway"};

	//Mission variables 
	string SurvivorName;		
	
	override bool IsExtended() { return true; }
	
	void FishBanditMission()
	{	
	
	
		//Select Fish Count
		FishCount1 = Math.RandomIntInclusive(3,7);
		
		//Select Fish Type
		FishType1 = FishTypes.GetRandomElement();
		
	
		//Select primary mission
		m_MissionExtended = true;
				
		//Mission timeout
		m_MissionTimeout = 2700;			//seconds, mission max duration time
		
		//Mission zones
		m_MissionZoneOuterRadius = 300.0;	//meters (!Do not set lower than 200m), mission activation distance
		m_MissionZoneInnerRadius = 3.0;		//meters (!Do not set outside of 1-5m), mission finishing distance to target object
				
		//Mission informant
		m_MissionInformant = "Dr. Legasov";
	
		//Mission person names
		TStringArray SurvivorNames = {"Yuri", "Michail", "Boris", "Valeri", "Anatoli", "Ivan", "Alexej", "Dimitrij", "Sergej", "Nikolai"};
		SurvivorName = SurvivorNames.GetRandomElement();
		
		//Set mission messages for primary mission
		m_MissionMessage1 = "A survivor recently told me that a group of bandits attacked my friend " + SurvivorName + " and killed two other survivors. They went to the wrong location while trying to fish near " + m_MissionLocation + ".";
		m_MissionMessage2 = "I still can't believe it. " + SurvivorName + " was one of the best fishermen left on the island. Once a week he brought fresh fish to the hidden camp to feed a group of survivors. He was the group's only hope in these dark times.";
		m_MissionMessage3 = "His camp was near the shore\n** " + m_MissionLocationDir + " of " + m_MissionLocation + " **\nI need you to investigate the area. Please be careful! There is something in his tent I need you to recover.";

		//Spawnpoint for MissionObject for secondary mission
		Spawnpoints.Insert("-18.40 2.19 18.57");
		
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
	
	void ~FishBanditMission()
	{	
		//Despawn all remaining objects
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
		
		//Delete mission AI's
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
        float radius = 300.0;  // Example radius, you can change this value

        // Set name for the zone
        string name = "FishBandit PVP Zone";
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
	
	void SpawnContainer()
	{
		MissionObject =  ItemBase.Cast( GetGame().CreateObject( "SeaChest", m_MissionPosition ));
		m_MissionObjects.InsertAt( MissionObject, 0); 		
	}
	
	void SpawnRewards()
	{						
		//new MissionObject after deleting hunting bag
		MissionObject = ItemBase.Cast( GetGame().CreateObject("SeaChest", m_MissionPosition ));
		MissionObject.SetPosition( m_MissionPosition );
		
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
					
		Print("[SMM] Mission rewards spawned in reward container. Randomly selected loadout was "+selectedLoadout+"." );			
	}
	
	void SpawnObjects()
	{			
		if ( m_MissionExtended )
		{	
	
		//Clean up place from droped items before new tent spawns 
		bool ItemsAtGround = false; 
		
		GetGame().GetObjectsAtPosition( m_MissionPosition , 4.0 , m_ObjectList , m_ObjectCargoList );
		for ( int i = 0 ; i < m_ObjectList.Count(); i++ )
		{ 
			Object FoundObject = m_ObjectList.Get(i);
			if ( FoundObject.IsItemBase() )
			{
				ItemsAtGround = true;
				Print("[SMM] BeforeSpawnCleanUp :: Object  " + FoundObject.ToString() + " at new mission position was deleted.");				
				GetGame().ObjectDelete( FoundObject );
			}
		}
		//...and in case move tent pos a bit
		if ( ItemsAtGround )
		{
			float x = Math.RandomFloatInclusive( 1.5 , 3.0 );
			float y = Math.RandomFloatInclusive( 1.5 , 3.0 );
			int Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice > 4 ) x *= -1.0;
			Dice = Math.RandomIntInclusive( 0, 9);
			if ( Dice < 5 ) y *= -1.0;
			vector NewPosVector = { x, 0, y };
			m_MissionPosition += NewPosVector;
			Print("[SMM] BeforeSpawnCleanUp :: Items on the ground detected. Tent position predictively was moved a bit.");
		}
		
			//Spawn 2 Dead Bodies with randomized fisherman outfits and survivor types
			ref array<ref array<string>> fishermanOutfits = {
				{"HikingJacket_Blue", "CargoPants_Blue", "Wellies_Black", "FishingHat_Blue"},
				{"Raincoat_Yellow", "Jeans_Blue", "Wellies_Brown", "BaseballCap_Blue"},
				{"HikingJacket_Green", "CargoPants_Green", "Wellies_Green", "BeanieHat_Blue"},
				{"Jacket_Black", "CargoPants_Black", "Wellies_Black", "FlatCap_Black"},
				{"HikingJacket_Red", "Jeans_Blue", "Wellies_Brown", "FishingHat_Red"}
			};
			ref array<string> survivorTypes = {
				"SurvivorM_Mirek",
				"SurvivorM_Denis",
				"SurvivorM_Simeon",
				"SurvivorM_Boris",
				"SurvivorM_Cyril",
				"SurvivorM_Elias",
				"SurvivorM_Francis",
				"SurvivorM_Guo",
				"SurvivorM_Hassan",
				"SurvivorM_Indar"
			};

			for (int bodyIdx = 0; bodyIdx < 2; bodyIdx++)
			{
				// Pick a random angle and distance (18-22m) from camp
				float angle = Math.RandomFloatInclusive(0, Math.PI2);
				float dist = Math.RandomFloatInclusive(18.0, 22.0);
				vector offset = Vector(Math.Cos(angle) * dist, 0, Math.Sin(angle) * dist);
				vector bodyPos = m_MissionPosition + offset;

				// Randomize survivor type
				string survivorType = survivorTypes.GetRandomElement();

				// Spawn dead survivor
				PlayerBase deadBody = PlayerBase.Cast(GetGame().CreatePlayer(null, survivorType, bodyPos, 0, "TEST"));
				if (deadBody)
				{
					//Mission person names
					TStringArray SurvivorNames = {"Yuri", "Michail", "Boris", "Valeri", "Anatoli", "Ivan", "Alexej", "Dimitrij", "Sergej", "Nikolai"};
					SurvivorName = SurvivorNames.GetRandomElement();

					// Set position and orientation
					deadBody.SetPosition(bodyPos);
					deadBody.SetOrientation(Vector(0, Math.RandomFloatInclusive(0, 360), 0));
					deadBody.SetAllowDamage(true); // Allow damage so explosion can kill
					deadBody.SetLifetime(3600); // 1 hour

					// Randomize fisherman outfit
					ref array<string> outfit = fishermanOutfits.GetRandomElement();
					for (int o = 0; o < outfit.Count(); o++)
					{
						deadBody.GetInventory().CreateInInventory(outfit[o]);
					}

					// Randomize hunter/drybags and supplies on the dead body

					// Bags to choose from
					ref array<string> bags = {"HuntingBag", "DryBag_Black", "DryBag_Green", "DryBag_Orange", "DryBag_Yellow", "DryBag_Blue"};
					string selectedBag = bags.GetRandomElement();
					EntityAI bag = deadBody.GetInventory().CreateInInventory(selectedBag);

					// Supplies to randomize
					ref array<string> possibleSupplies = {
						"FishingRod", "Carp", "Hook", "Worm", "Knife", "Canteen", "WaterBottle", "PeachesCan", "SpaghettiCan", "Matchbox", "Rope", "LongWoodenStick", "StoneKnife"
					};

					// Always add at least one fishing item
					string fishingItem = possibleSupplies.GetRandomElement();
					deadBody.GetInventory().CreateInInventory(fishingItem);

					// Add 2-4 random supplies to the bag if it exists
					if (bag)
					{
						int supplyCount = Math.RandomIntInclusive(2, 4);
						for (int s = 0; s < supplyCount; s++)
						{
							string supply = possibleSupplies.GetRandomElement();
							bag.GetInventory().CreateInInventory(supply);
						}
					}

					// Spawn an explosion next to the NPC to kill it
					// Spawn a landmine under the body and trigger it to simulate a trap
					vector minePos = bodyPos + Vector(Math.RandomFloatInclusive(-0.2, 0.2), 0, Math.RandomFloatInclusive(-0.2, 0.2));
					Grenade_Base explosives = Grenade_Base.Cast(GetGame().CreateObject("M67Grenade", minePos));
					if (explosives)
					{
						GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(explosives.ActivateImmediate, 3000, false);
					}

					// Optionally, you can still set stats to 0 as a fallback
					deadBody.GetStatStamina().Set(0);
					deadBody.GetStatEnergy().Set(0);
					deadBody.GetStatWater().Set(0);

					deadBody.SetName(SurvivorName);
					deadBody.SetBloodyHands(true);
					deadBody.SetSynchDirty();
					deadBody.PlaceOnSurface();

					m_MissionObjects.Insert(deadBody);

					Print("[SMM] Spawned dead body of " + survivorType + " with fishing outfit at position: " + bodyPos.ToString());
				}
			}
			

			//Spawn & pitch tent		
			TentObject = TentBase.Cast( GetGame().CreateObject( "MediumTent", m_MissionPosition ) );
			TentObject.Pitch( true , true );
			TentObject.PlaceOnSurface();
			
			TentObject.GetInventory().CreateInInventory("LongWoodenStick");
			TentObject.GetInventory().CreateInInventory("Rope");
			TentObject.GetInventory().CreateInInventory("Hook");
			TentObject.GetInventory().CreateInInventory("Worm");
			TentObject.GetInventory().CreateInInventory("Worm");
			TentObject.GetInventory().CreateInInventory("Worm");
			TentObject.GetInventory().CreateInInventory("Worm");
			TentObject.GetInventory().CreateInInventory("Worm");
			TentObject.GetInventory().CreateInInventory("Worm");
			TentObject.GetInventory().CreateInInventory("MetalWire");
			TentObject.GetInventory().CreateInInventory("Netting");
			
			//Insert mission crate into mission objects list
			m_MissionObjects.Insert( TentObject );

			EntityAI wood, stones;
			
			//fireplace
			FireplaceBase MissionCampfire = FireplaceBase.Cast( GetGame().CreateObject( "Fireplace", m_MissionPosition, ECE_PLACE_ON_SURFACE ) );
			MissionCampfire.SetPosition( m_MissionPosition + "3 0 2" );
			MissionCampfire.Synchronize();
			wood = MissionCampfire.GetInventory().CreateAttachment("Firewood");
			ItemBase.Cast( wood ).SetQuantity(6);
			MissionCampfire.GetInventory().CreateAttachment("WoodenStick");
			MissionCampfire.GetInventory().CreateAttachment("Rag");

			for (int ij = 0; ij < 16; ij++)
			{
				MissionCampfire.GetInventory().CreateAttachment("Stone");
			}

			MissionCampfire.SetStoneCircleState(true);
			MissionCampfire.PlaceOnSurface();
			MissionCampfire.StartFire(true);
			m_MissionObjects.Insert(MissionCampfire);

		}
		else
		{				
			if ( MissionBuilding )
			{
				//Cleanup position before Hunting Backpack
				GetGame().GetObjectsAtPosition( m_MissionPosition, 0.5, m_ObjectList, m_ObjectCargoList );
				for ( int k = 0; k < m_ObjectList.Count(); k++ )
				{
					Object FoundObjectFin = m_ObjectList.Get(k);
					if ( FoundObjectFin.IsItemBase() )
					GetGame().ObjectDelete( FoundObjectFin );
				}
				
				//Spawn container
				GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnContainer, 1000 );
								
				Print("[SMM] Survivor Mission Extension "+ m_selectedMission +" :: "+ m_MissionName +" ...mission extended!");
			}
			else Print("[SMM] Extended MissionBuilding couldn't be found in "+ m_MissionDescription[3] +" ... Mission rejected!");
		}
	}
	
	void SpawnAIs()
	{	
		// TO ADD BANDITS TO THIS TEMPLATE YOU WILL NEED TO ADD THE FIREPLACE OBJECT THEY ARE TO SPAWN AT IE PLACE HOLDER IN THE AISETTINGS.C FILE FOR EXPANSION AI
		// IN THIS CASE I AM USING "FP_FireBarrelGrill"
	}
	
	override void ObjDespawn()
	{	
		//Despawn all mission objects at mission timeout except those retains until next mission
		for ( int i = 0; i < m_MissionObjects.Count(); i++ )
		{
			//Delete Object clientside first
			if ( m_MissionObjects.Get(i).GetType() == "FBF_FireplaceIgnited" ) GetGame().RemoteObjectDelete( m_MissionObjects.Get(i) );
			
			//Delete Object serverside
			GetGame().ObjectDelete( m_MissionObjects.Get(i) );
		}
	}
	
	override void ExtendMission()
	{

		//Set new mission messages
		m_MissionMessage1 = "All right survivor, you lived! I dont know how but you did! It seems "+ SurvivorName +" wasnt carrying the item after all. ";
		m_MissionMessage2 = "Instead make a fishing pole and catch me "+ FishCount1 +" "+ FishType1 +" then bring them to,\n** "+ m_MissionSecondaryLoc +" School laboratory class room **\n(left wing, top floor) this is the normal drop off area. I left a Seachest on the labor desk.";
		m_MissionMessage3 = "Put atleast "+ FishCount1 +". Be careful, You may be the groups only hope now. Good luck!";
		
		//init Messenger for new messages
		m_MsgNum = 1;					//skip msg 0, begin with msg 1
		m_MsgChkTime = m_MissionTime;
		MsgCutoff = false;
		
		//increase mission MissionTimeout
		m_MissionTimeout = m_MissionTime + ExtendedTimout;  
		
		//deployment settings & init for extended mission
		m_MissionZoneOuterRadius = 120.0;
		m_MissionZoneInnerRadius = 2.0;
		
		//Get building position from building type and location
		if ( EventsWorldData.GetBuildingsAtLoc("Land_City_School", m_MissionDescription[3], ExtendedPosList ))
		{
			//Get MissionPosition for MissionBuilding from secondary mission
			m_MissionPosition = ExtendedPosList.GetRandomElement();						
			GetGame().GetObjectsAtPosition( m_MissionPosition, 1.0 , m_ObjectList , m_ObjectCargoList );
			for ( int m=0; m < m_ObjectList.Count(); m++ )
			{ 
				Object FoundObject = m_ObjectList.Get(m);
				if ( FoundObject.GetType() == "Land_City_School" )
				{	
					MissionBuilding = FoundObject;			 
										
					//new MissionPosition is rewards spawnpoint
					m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(0) );
					break;
				}	
			}
		}
		else Print("[SMM] Can't find MissionBuilding City School in "+ m_MissionDescription[3] +"!");
	}
	
	override void MissionFinal()
	{		
		
		// DO NOTHING HERE!!!! 
	}
	
	override void PlayerChecks(PlayerBase player)
	{
		// DO NOTHING IF IT EXTENDS
		if ( m_MissionExtended ) {
			// Intentionally left blank
		}
		
		//Check if container gets taken from player
		if ( MissionSettings.Opt_DenyObjTakeaway && !m_MissionExtended )
		{
			if ( m_MissionObjects[0] && m_MissionObjects[0].ClassName() == "SeaChest" )
			{
				if ( player.GetInventory().HasEntityInInventory( EntityAI.Cast( m_MissionObjects[0] ) ) && !m_ContainerWasTaken )
				{
					m_ContainerWasTaken = true;
					Print("[SMM] Mission object container was taken by a player ...and will be deleted.");
					GetGame().ObjectDelete( m_MissionObjects[0] );
				}
			}
		}
		
		//Check if container has desired amount of Items collected at primary mission position IF SO THEN SPAWN REWARD
		if ( MissionObject != null && MissionObject.ClassName() == "SeaChest" && !m_MissionExtended )
		{
			int CargoCount = MissionObject.GetInventory().CountInventory();
			int LastCount = 0;
			int FoundFishObjects = 0;
			bool FoundFishObject = false;
			
			if ( CargoCount != LastCount )
			{
				if ( CargoCount >= FishCount1 && FoundFishObjects <= FishCount1 )
				{	
					CargoBase CargoItems1 = MissionObject.GetInventory().GetCargo();		
					
					for ( int i = 0; i < CargoCount; i++ )
					{
						EntityAI CargoItem = CargoItems1.GetItem(i);
						if ( CargoItem != null && CargoItem.GetType() == FishType1 ) FoundFishObjects++;
						else continue;					
						
						//When requested amount of Items is present, despawn MissionObject & spawn rewards
						if ( FoundFishObjects >= FishCount1 )
						{
							if (player.GetIdentity())
							{
								Print("[SMM] Player with SteamID64: "+ player.GetIdentity().GetPlainId() +" has successfully stored the requested amount of "+ FishCount1 +" MissionObjects in the container.");
							}
							else
							{
								Print("[SMM] Player identity is null. Unable to log SteamID64.");
							}
							//delete container first
							GetGame().ObjectDelete( MissionObject );
							//call rewards spawn one second later 
							GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( this.SpawnRewards, 1000 );
							m_RewardsSpawned = true;
							m_MsgNum = -1;
							m_MsgChkTime = m_MissionTime + MsgDlyFinish;
							break;
						} 
					}
					LastCount = CargoCount;
				}
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
		int factionType = 0;    // RANDOM will randomize unless 1-10 then DEFAULT_FACTION set at Top
		int loadoutType = 9;    // RANDOM will randomize unless 1-14 then DEFAULT_FACTION set at Top
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
		CreateCircularPatrolRoute(group, m_MissionPosition, 25); // 25m radius

		// Verify and Set Formation
		Print(string.Format("[SMM] Spawned patrol: %1/%2 AI (Leader: %3) Loadout:%4 Faction:%5", spawnedCount, squadSize, leader.GetType(), loadout, faction.GetName()));
		group.SetFormation(CreateFormation(formationType));
		
	}

	#endif
	#endif

	override bool DeployMission()
	{	//When first player enters the mission zone (primary/secondary)
		//Get MissionBuilding at mission position
		if ( !m_MissionExtended && !MissionBuilding )
		{
			if ( EventsWorldData.GetBuildingsAtLoc("Land_City_School", m_MissionDescription[3], ExtendedPosList ))
			{		
				//Get MissionPosition for MissionBuilding from secondary mission
				m_MissionPosition = ExtendedPosList.GetRandomElement();						
				GetGame().GetObjectsAtPosition( m_MissionPosition, 1.0 , m_ObjectList , m_ObjectCargoList );
				for ( int m=0; m < m_ObjectList.Count(); m++ )
				{ 
					Object FoundObject = m_ObjectList.Get(m);
					if ( FoundObject.GetType() == "Land_City_School" )
					{	
						MissionBuilding = FoundObject;			 
						Print("[SMM] MissionBuilding extended is "+ MissionBuilding.GetType() +" at "+ m_MissionDescription[3] +" @ "+ MissionBuilding.GetPosition() );
											
						//new MissionPosition is rewards spawnpoint
						m_MissionPosition = MissionBuilding.ModelToWorld( Spawnpoints.Get(0) );
						break;
					}	
				}
			}
			else Print("[SMM] Can't find MissionBuilding City School in "+ m_MissionDescription[3] +"!");	
		}
		
					
		if ( (m_MissionPosition && m_MissionPosition != "0 0 0" && m_MissionExtended) || (MissionBuilding && !m_MissionExtended) )
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
			Print("[SMM] Mission position was rejected or MissionBuilding doesn't exist!");
			if ( MissionSettings.DebugMode && !MissionBuilding && !m_MissionExtended ) Print("[SMM] MissionBuilding is NULL!");
			return false;
		}
	}

}

