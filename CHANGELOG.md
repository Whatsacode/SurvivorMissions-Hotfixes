# 🚀 Hotfix Change Log
## Version 0.85 v9 – Hotfixes & Features  
📅 **Published:** 2025-09-07  

### 🧩 Module  
- *No changes.*

### 🛠 Server Files  
- *No changes.*

- **Mission Templates**
  - Fixed Typo In All Mission Rewards - "'AK_Suppressor" to "AK_Suppressor"
  - Fixed Typo In Camp Mission Script

---


## Version 0.85 v8 – Hotfixes & Features  
📅 **Published:** 2025-08-25  

### 🧩 Module  
- *No changes.*

### 🛠 Server Files  
- *No changes.*

- **Mission Templates**
  - Updated BearHunt Mission Turn-In Hunting Bag into a Mountain Bag.
  - Updated All Reward Containers to be Seachest this will allow all Rewards to Spawn.
  - Updated All Player Check Functions to Follow Bool Settings to disable players from taking Seachest.

---

## Version 0.85 v7 – Hotfixes & Features  
📅 **Published:** 2025-08-14  

### 🧩 Module  
- *No changes.*

### 🛠 Server Files  
- **Chernarus EWD:**
- Added 2 Racecar Missions

- **Mission Templates**
  - Racecarmission Added

---

## Version 0.85 v6 – Hotfixes & Features  
📅 **Published:** 2025-08-14  

### 🧩 Module  
- *No changes.*

### 🛠 Server Files  
- **Chernarus EWD:**
- *No changes.*

- **Mission Templates**
  - Fixed Mission's Not Deploying #ifdef misplacement.

---

## Version 0.85 v5 – Hotfixes & Features  
📅 **Published:** 2025-06-10  

### 🧩 Module  
- *No changes.*

### 🛠 Server Files  
- **Chernarus EWD:**
  - Added `#ifdef` before Escort Missions:  
    If Expansion AI is not installed, mission will not load.

- **EscortBot Mission** *(Requires Expansion AI)*  
  - Updated code base for compatibility with new Expansion AI.
  - Added:
    - PVEZ PVP Zone Function
    - Scheduled Restart Timer Check  
  - 👤 Credit: **Xellose**

---

## Version 0.85 v4 – Hotfixes & Features  
📅 **Published:** 2025-06-01  

### 🧩 Module  
- Added overrides to clean-up logs.

### 🛠 Server Files  
- **EWD - Chernarus:**

#### 🎣 FishBandit Mission  
- Updated:
  - Fireplace
  - PVEZ PVP Zone Function
  - Expansion AI Patrol Spawn (enabled by default; 25m radius)
  - Scheduled Restart Timer Check
  - Added Dead Bodies to Event  
- 👤 Credit: **Xellose**

#### 🚂 TrainWreck Mission  
- Added:
  - PVEZ PVP Zone Function
  - Expansion AI Patrol Spawn
  - Scheduled Restart Timer Check
  - Fix: Infected moving through train wagons  
- 👤 Credit: **Tarkules**

#### 🚩 CaptureFlag Mission  
- Added:
  - PVEZ PVP Zone Function
  - Expansion AI Patrol Spawn (enabled by default; 700m intersecting patrol)
  - Scheduled Restart Timer Check
  - Fixed Fireplace Rock Circle (no longer floating)  
- 👤 Credit: **Funkdoc**

#### 🚚 Convoy Mission  
- Removed:
  - Code relating to mods other than Expansion AI.
- Added:
  - PVEZ PVP Zone Function
  - Expansion AI Patrol Spawn (friendly patrol; enabled by default)
  - Scheduled Restart Timer Check
  - Dynamic mission text depending on Expansion AI presence
  - Convoy now uses DayZ cars (vanilla-compatible)  
- 👤 Credit: **Cygnus**

---

## Version 0.85 v3 – Hotfixes & Features  
📅 **Published:** 2025-05-26  

### 🧩 Module  
- Added:
  - PVEZ PVP Zone Functionality
  - Expansion AI Patrol Spawn Functionality

---

## Version 0.85 v2 – Hotfixes & Features  
📅 **Published:** 2023-11-10  

### 🧩 Module  
- Added:
  - Tent Check Function (from Discord)  
  - 👤 Credit: **Vandest**

---

## Version 0.85 v2 – Hotfixes & Features  
📅 **Published:** 2022-03-08  

### 🧩 Module  
- Added:
  - Territory Check Function (from Discord)  
    - 👤 Credit: **Gramps**
  - Scheduled Restart Timer Check  
    *(Default: 4-hour restart, set in `missionsettings.c`)*

### 🛠 Server Files  
- Fixed:
  - SparkPlug issue in Ganja Mission
- Added:
  - Territory Check to EWD
  - Restart Check to each mission
