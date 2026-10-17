# March state-pointer map (CX2GUUser m_NowStateData / m_FutureStateData / m_StateList)

Source: X2Lib/ working tree (read only). Korean bytes shown as '?'. Function attribution of insert/ref lines = nearest preceding `Class::Func(` at column 0 (heuristic; hand-verified on samples).

## 1. Container and struct

- X2GUUser.h:3955-3957: `UserUnitStateData m_FutureStateData; UserUnitStateData m_NowStateData; UserUnitStateDataMap m_StateList;` (all by value, members of CX2GUUser)
- X2GUUser.h:1117/1119: `typedef map<USHORT, UserUnitStateData> UserUnitStateDataMap;` (flag SERV_IRUHADEV_JUNE_USER_STATE_ID16 on; else map<UCHAR,...>). std::map is node based: insert never moves elements; only erase/clear invalidates a pointer.
- X2GUUser.h:1880: `const UserUnitStateDataMap& GetStateList() const` -- NO callers anywhere in X2Lib / X2 (grep).
- X2GUNPC.h:3595 `map<char,NPCStateData> m_StateList` and X2Pet.h:916 `map<char,PetStateData> m_StateList` are different types/containers, out of scope.
- Only CX2GUUser-derived classes (Elsword, Arme, Lire, Raven, Eve, Chung, Ara, EL, Add) touch UserUnitStateData; X2ComboAndSkillChainSystem.cpp:98 calls Ara's InitAquiredActiveSkillState.

### UserUnitStateData members (X2GUUser.h:1016-1112)
1. `USHORT stateID` (UCHAR if flag off) 2. `char m_SPLevel` 3. `float m_fPowerRate` (#ifdef NEW_SKILL_TREE, live) 4. `CX2SkillTree::SKILL_ID m_eSkillID` 5. `bool m_bHyperState`
6-14. nine GUStateCBFunc: StateInit, StateStartFuture, StateStart, OnFrameMoveFuture, OnFrameMove, OnCameraMove, OnEventProcess, StateEndFuture, StateEnd
15. `CX2ComboAndSkillChainInfoPtr m_ptrComboAndSkillChainInfo` (shared_ptr)
- virtual void Init() (:1055-1080) resets all (stateID=GUSI_NONE, m_SPLevel=0, m_fPowerRate=1, m_eSkillID=SI_NONE, callbacks empty, hyper=false, ptr.reset()). Ctor calls Init(). Copy ctor `{ *this = t; }` :1084.
- operator= (:1085-1111) copies all 15 members one by one. Struct has a vtable (virtual Init).

## 2. Whole-struct writes of m_NowStateData / m_FutureStateData (the by-value copies)

- X2GUUser.cpp:6697: `m_FutureStateData			= m_StateList[m_FrameDataFuture.syncData.nowState];`
- X2GUUser.cpp:6761: `m_NowStateData			= m_StateList[m_FrameDataNow.syncData.nowState];`
- X2GUUser.cpp:6762: `m_NowStateData.stateID	= m_FrameDataNow.syncData.nowState;`
- X2GUUser.cpp:1865: `m_FutureStateData.Init();`
- X2GUUser.cpp:1866: `m_NowStateData.Init();`
  - :6697 in DoStateEndStartFuture (starts :6670); :6761 in DoStateEndStart (starts :6718); :6762 re-stamps stateID after the Now copy only (Future's equivalent line is commented out at :6698). :1865-1866 are Init() of both (ctor region).
- No other whole assignment (`= *it`, `= m_StateList[..]`) exists for the two members. Passed whole by const ref only: SetCustomAnimName( m_FutureStateData, animName ) X2GUUser.cpp:15126; SetCustomAnimName( m_NowStateData, animName ) :15819 (virtual X2GUUser.h:3285 `IN const UserUnitStateData&`; only override CX2GUAdd X2GUAdd.cpp:8891).

## 3. Member reads/writes of m_NowStateData / m_FutureStateData (non-comment lines)

Counts by member (reads+writes, comments excluded):
- m_FutureStateData.Init: 1
- m_FutureStateData.OnEventProcess: 4
- m_FutureStateData.OnFrameMoveFuture: 2
- m_FutureStateData.StateEndFuture: 2
- m_FutureStateData.StateStartFuture: 2
- m_FutureStateData.m_eSkillID: 2
- m_FutureStateData.m_ptrComboAndSkillChainInfo: 4
- m_FutureStateData.stateID: 8
- m_NowStateData.Init: 1
- m_NowStateData.OnCameraMove: 2
- m_NowStateData.OnFrameMove: 2
- m_NowStateData.StateEnd: 2
- m_NowStateData.StateStart: 2
- m_NowStateData.m_SPLevel: 3
- m_NowStateData.m_bHyperState: 2
- m_NowStateData.m_eSkillID: 5
- m_NowStateData.m_fPowerRate: 12
- m_NowStateData.stateID: 72

Counts by file: X2GUAdd.cpp: Now 0/Future 1, X2GUAra.cpp: Now 7/Future 4, X2GUArme_VioletMage.cpp: Now 2/Future 0, X2GUChung.cpp: Now 11/Future 0, X2GUElsword_SwordMan.cpp: Now 15/Future 0, X2GULire_ElvenRanger.cpp: Now 8/Future 0, X2GURaven.cpp: Now 7/Future 0, X2GUUser.cpp: Now 50/Future 18, X2GUUser.h: Now 3/Future 2
(no accesses in X2GUEL.cpp, X2GuEve.cpp, nor outside X2Lib; X2GUUser.cpp has 72)

WRITES to a member (excluding Init()):
- X2GUChung.cpp:13134: `m_NowStateData.m_fPowerRate					*= GetDamageRelByCannonBallMastery();`
- X2GUChung.cpp:13376: `m_NowStateData.m_fPowerRate *= 1.2f;`
- X2GUElsword_SwordMan.cpp:13201: `m_NowStateData.m_fPowerRate *= 1.1f;`
- X2GUElsword_SwordMan.cpp:13206: `m_NowStateData.m_fPowerRate *= 4.0f;`
- X2GULire_ElvenRanger.cpp:19170: `m_NowStateData.m_fPowerRate *= m_sFatalityData.GetPowerRate();`
- X2GURaven.cpp:14718: `m_NowStateData.m_fPowerRate = m_fAttackPowerRateShadowStepThrust;`
- X2GURaven.cpp:20882: `m_NowStateData.m_fPowerRate = m_fShadowBusterPowerRate;`
- X2GURaven.cpp:22920: `m_NowStateData.m_fPowerRate = m_fAttackPowerRateShadowStepPunisher;`
- X2GUUser.cpp:6762: `m_NowStateData.stateID	= m_FrameDataNow.syncData.nowState;`
- X2GUUser.cpp:6778: `m_NowStateData.m_fPowerRate *= fJuneBuffSkillDamageRate;`
- X2GUUser.cpp:6779: `m_NowStateData.m_fPowerRate *= m_fSkillDamageRateBySlotType;							/// @3203c801: 98`
- Init() calls: X2GUUser.cpp:1865 (m_FutureStateData.Init()), :1866 (m_NowStateData.Init()).
- Indirect write via the shared_ptr member (target shared with the m_StateList entry because operator= copies the ptr): X2GUAra.cpp:3450 `m_FutureStateData.m_ptrComboAndSkillChainInfo->SetChaining( true )`, :3451 ResetChainingFromNext(), :3452 passes the ptr to m_ComboAndSkillChainSystem.ResetOtherListChaining.
- Read helpers: X2GUUser.h:1808 `GetNowStateID() const { return m_NowStateData.stateID; }`, :1809 GetFutureStateID, :1810 GetNowStateSkillID, :1812 GetFutureStateSkillID, :1815 IsHyperState; X2GUUser.cpp:27601 GetPowerRate() returns m_NowStateData.m_fPowerRate. X2GameUnit.h:2453 `virtual int GetNowStateID() const = NULL`.
- m_NowStateData.stateID is read ~72 times, mostly `m_LuaManager.BeginTableByReference( m_NowStateData.stateID )` in class StateStart/FrameMove callbacks plus switch/compare.
- BEHAVIOUR WARNING: because Now is a COPY, `m_NowStateData.m_fPowerRate *= x` changes only the running copy. With a pointer into m_StateList those writes would compound permanently into the container entry on every re-entry of the state. Sites: X2GUUser.cpp:6778-6779 (DoStateEndStart June buff factors), Chung :13134 (CSI_SI_A_CHUNG_RELOAD_CANNON_Start) and :13376 (CSI_SI_SA_CHUNG_BRUTAL_SWING_FrameMove, inside an EventTimerOneshot), Elsword :13201/:13206 (ESSI_SI_A_ES_KICK_Start), Lire :19170 (LESI_SA_RTR_FATALITY_StateStart), Raven :14718/:20882/:22920 (assign `=` in *_Start callbacks; assignment is idempotent, `*=` is not).

## 4. References / pointers to the container or its elements

Pointers: only X2GUUser.cpp:5707 `UserUnitStateData* pData = &(iter->second);` (InitState loop, local). Iterators: :5704 local; :33939 `UserUnitStateDataMap::iterator itState = m_StateList.find(...)` local (JuneUpdateCommonRandomState). No member pointer/iterator/reference to a state element or to m_NowStateData/m_FutureStateData exists (`= &m_NowStateData`: none).
Returned reference: X2GUUser.cpp:40439 GetPressedSkillState returns `const UserUnitStateData&` = m_StateList[...] at :40445 :40449 :40456 :40460; sole caller X2GUAra.cpp:3689 `const UserUnitStateData& stateDataPressed = GetPressedSkillState(...)` (local).
Local ref binds `UserUnitStateData& x = m_StateList[ id ]` (operator[] inserts a default entry if the key is absent), all in functions that run MID-GAME (sec 5):
- X2GUUser.cpp SetEquippedSkillLevelCommon: 4 binds, lines 6316, 6322, 6334, 6340
- X2GUAdd.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 618, 623
- X2GUAra.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 1897, 1902
- X2GUAra.cpp InitAquiredActiveSkillState: 35 binds, lines 16238, 16245, 16258, 16265, 16274, 16281, 16295, 16302, 16309, 16316, 16329, 16336, 16350, 16357, 16366, 16373, 16380, 16387, 16394, 16415, 16422, 16432, 16439, 16448, 16455, 16462, 16471, 16478, 16487, 16494, 16503, 16510, 16519, 16526, 16532
- X2GUArme_VioletMage.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 2848, 2853
- X2GUArme_VioletMage.cpp SetEquippedSkillLevel: 23 binds, lines 4112, 4117, 4154, 4159, 4180, 4185, 4190, 4195, 4217, 4237, 4244, 4265, 4272, 4296, 4303, 4310, 4317, 4324, 4331, 4338, 4345, 4352, 4359
- X2GUChung.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 3190, 3195
- X2GUChung.cpp SetEquippedSkillLevelStateData: 2 binds, lines 5157, 5162
- X2GUEL.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 666, 671
- X2GUElsword_SwordMan.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 2892, 2897
- X2GUElsword_SwordMan.cpp SetEquippedSkillLevel: 32 binds, lines 4142, 4147, 4152, 4157, 4179, 4184, 4191, 4196, 4205, 4210, 4217, 4222, 4247, 4252, 4260, 4265, 4290, 4296, 4303, 4309, 4316, 4322, 4329, 4335, 4360, 4368, 4375, 4383, 4390, 4398, 4405, 4413
- X2GuEve.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 2159, 2164
- X2GuEve.cpp SetEquippedSkillLevel: 16 binds, lines 3239, 3244, 3252, 3257, 3264, 3269, 3276, 3281, 3289, 3294, 3301, 3306, 3330, 3335, 3342, 3347
- X2GULire_ElvenRanger.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 3092, 3097
- X2GULire_ElvenRanger.cpp SetEquippedSkillLevel: 19 binds, lines 4499, 4504, 4530, 4541, 4591, 4596, 4603, 4608, 4633, 4646, 4673, 4681, 4705, 4712, 4774, 4781, 4807, 4814, 4838
- X2GURaven.cpp SetEquippedSkillFuncBySkillSlotIndex: 2 binds, lines 2651, 2656
- X2GURaven.cpp SetEquippedSkillLevel: 73 binds, lines 4163, 4168, 4194, 4199, 4206, 4211, 4244, 4249, 4256, 4261, 4287, 4294, 4301, 4308, 4335, 4341, 4346, 4352, 4374, 4380, 4385, 4391, 4421, 4426, 4431, 4436, 4458, 4463, 4487, 4492, 4497, 4502, 4507, 4512, 4536, 4541, 4546, 4551, 4556, 4561, 4566, 4571, 4576, 4581, 4586, 4591, 4613, 4618, 4623, 4628, 4650, 4655, 4660, 4665, 4695, 4700, 4705, 4710, 4737, 4742, 4747, 4752, 4776, 4781, 4786, 4791, 4799, 4804, 4827, 4832, 4837, 4846, 4851
  total ref binds: 222
- By-ref args: `SetSkillLevelStateData( pSkillTemplet, m_StateList[eStateID] )` X2GUAdd.cpp:2455, X2GUAra.cpp:2985, X2GUEL.cpp:2974, X2GULire_ElvenRanger.cpp:22951, X2GURaven.cpp:22985; Ara `ASI_*_CreateState( UserUnitStateData& stateData_, id )` helpers get refs of m_StateList[..] (X2GUAra.cpp InitAquiredActiveSkillState :16224-16534) and of locals (51 ref params declared in X2GUAra.h).
- In-place Init() of live container entries: every class's SetEquippedSkillFuncBySkillSlotIndex does `normalStateData.Init(); normalStateData.stateID = ...; hyperStateData.Init(); ...` on m_StateList entries (e.g. X2GUChung.cpp:3190-3200). With a pointer model a running state (Now/Future pointing at it) re-initialised by ChangeEquippedSkillState would have its callbacks/stateID/power rate wiped under it. With the by-value copy that was harmless.

## 5. Inserts / in-place edits of m_StateList

Mid-game writers:
- CX2GUUser::ChangeEquippedSkillState( int slotId, int skillId ) X2GUUser.cpp:6152 (callers X2SkillTree.cpp:1795/1800/1808/1813/3976/3977) -> overload ( UnitData&, SKILL_ID, slot, bSlotB ) :6189, which calls CalcEquippedSkillLevelPlusAndMPCostBySlotID (:6192), `SetEquippedSkillFuncBySkillSlotIndex( eSkillId_, iSlotIndex_, bSlotB_ )` (:6195; pure virtual X2GUUser.h:3411), `SetEquippedSkillLevelCommon( kUnitData_, bSlotB_ )` (:6198), `SetEquippedSkillLevel( eSkillId_, false )` (:6201; virtual per class). Also X2Room.cpp:1670/1679 call the 4-arg overload.
- SetEquippedSkillFuncBySkillSlotIndex overrides (Init() + overwrite of the slot's normal/hyper entry, then `m_StateList[stateData.stateID] = stateData` for sub-states): X2GUAdd.cpp:612 (inserts :777-1070), X2GUAra.cpp:1890 (:1979-2657), X2GUArme_VioletMage.cpp:2842 (:3214-4005), X2GUChung.cpp:3184 (:3284-4779), X2GUEL.cpp:660 (:769-1496), X2GUElsword_SwordMan.cpp:2886 (:3026-3950), X2GuEve.cpp:2153 (:2651-3070), X2GULire_ElvenRanger.cpp:3086 (:3391-4470), X2GURaven.cpp:2645 (:2983-4121). Init-time caller: X2GUUser.cpp:38726 SetEquippedSkillFunc( kUnitData_, bSlotB_ ) (loop :38734) from InitEquippedActiveSkillState.
- InitEquippedActiveSkillState( bool bOnlySkillLevel ) X2GUUser.cpp:5914: SetEquippedSkillFunc x2 (skipped when bOnlySkillLevel), m_ComboAndSkillChainSystem.InitSkillStateChained( this ) (X2ComboAndSkillChainSystem.cpp:91 -> Ara-only virtual InitAquiredActiveSkillState X2GUAra.cpp:16224: 35 ref binds + SetSkillLevelStateData + *_CreateState into m_StateList), SetEquippedSkillLevelCommon x2, SetEquippedSkillLevel( SI_NONE, true ). Class overrides: Arme X2GUArme_VioletMage.cpp:2509 (calls base). Called at init from each class InitState (X2GUAdd.cpp:358, X2GUAra.cpp:346, X2GUArme_VioletMage.cpp:1615, X2GUChung.cpp:882).
- Reached MID-GAME via CX2GUUser::UpdatePassiveAndActiveSkillState X2GUUser.cpp:32367 -> `InitEquippedActiveSkillState( true )` :32371 (no SetEquippedSkillFunc, but still in-place writes of m_SPLevel / m_fPowerRate / m_eSkillID by SetEquippedSkillLevelCommon, per-class SetEquippedSkillLevel, and for Ara the InitAquiredActiveSkillState CreateState rewrites). Callers: X2BuffBehaviorTemplet.cpp:4031/4068/4078, X2Game.cpp:1907, X2PetManager.cpp:2254/2417/2802/3389, X2PremiumBuffManager.cpp:607, X2Room.cpp:1687/2550, X2StateDungeonGame.cpp:3756, X2StateMenu.cpp:4620/4753/4856, X2UIInventory.cpp:15421, X2GUUser.cpp:42343, X2GUUser.h:2968.
- SetEquippedSkillLevelCommon X2GUUser.cpp:6285-6350: in-place writes of m_SPLevel / m_fPowerRate / m_eSkillID on m_StateList[normalStateID] and [hyperStateID] (:6316-6340; live branch UPGRADE_SKILL_SYSTEM_2013, plus the June socket factor `*= fJuneGroupPowerRate`).
- Per-class SetEquippedSkillLevel / SetEquippedSkillLevelStateData / SetSkillLevelStateData (in-place m_SPLevel/m_fPowerRate/m_eSkillID): Add X2GUAdd.cpp:1146/2452, Ara :2700/2982/2989, Arme :4090, Chung :4838/5150, EL :2977, Elsword (binds :4142-4413), Eve (binds :3239-3347), Lire (:4499-4838; SetSkillLevelStateData :22954), Raven (:4163-4851; :22987).
Insert sites by function (`m_StateList[ x ] = state`; counts and line ranges):
- X2GUUser.cpp JuneInitWaitStateChange: 1 inserts, lines 22111..22111
- X2GUUser.cpp JuneInitBoundingStates: 3 inserts, lines 30614..30616
- X2GUAdd.cpp InitState: 3 inserts, lines 343..355
- X2GUAdd.cpp SetEquippedSkillFuncBySkillSlotIndex: 10 inserts, lines 777..1070
- X2GUAdd.cpp InitStateCommon: 100 inserts, lines 1560..2311
- X2GUAdd.cpp InitStateByUnitClass: 17 inserts, lines 2332..2443
- X2GUAra.cpp InitState: 3 inserts, lines 330..342
- X2GUAra.cpp InitStateCommon: 98 inserts, lines 369..1318
- X2GUAra.cpp InitStateByUnitClass: 20 inserts, lines 1345..1501
- X2GUAra.cpp SetEquippedSkillFuncBySkillSlotIndex: 35 inserts, lines 1979..2657
- X2GUArme_VioletMage.cpp InitState: 45 inserts, lines 1032..1606
- X2GUArme_VioletMage.cpp InitStateCommon: 90 inserts, lines 1648..2472
- X2GUArme_VioletMage.cpp SetEquippedSkillFuncBySkillSlotIndex: 24 inserts, lines 3214..4005
- X2GUChung.cpp InitState: 3 inserts, lines 860..876
- X2GUChung.cpp InitStateCommon: 101 inserts, lines 901..1775
- X2GUChung.cpp InitStateByUnitClass: 106 inserts, lines 1796..2670
- X2GUChung.cpp SetEquippedSkillFuncBySkillSlotIndex: 73 inserts, lines 3284..4779
- X2GUEL.cpp InitState: 3 inserts, lines 292..304
- X2GUEL.cpp SetEquippedSkillFuncBySkillSlotIndex: 26 inserts, lines 769..1496
- X2GUEL.cpp InitStateCommon: 96 inserts, lines 2015..2740
- X2GUEL.cpp InitStateByUnitClass: 25 inserts, lines 2763..2962
- X2GUElsword_SwordMan.cpp InitState: 34 inserts, lines 920..1308
- X2GUElsword_SwordMan.cpp InitStateCommon: 116 inserts, lines 1337..2463
- X2GUElsword_SwordMan.cpp SetEquippedSkillFuncBySkillSlotIndex: 17 inserts, lines 3026..3950
- X2GuEve.cpp InitStateCommon: 95 inserts, lines 684..1547
- X2GuEve.cpp InitState: 32 inserts, lines 1683..2122
- X2GuEve.cpp SetEquippedSkillFuncBySkillSlotIndex: 7 inserts, lines 2651..3070
- X2GULire_ElvenRanger.cpp InitStateCommon: 76 inserts, lines 938..1626
- X2GULire_ElvenRanger.cpp InitState: 83 inserts, lines 1741..2668
- X2GULire_ElvenRanger.cpp SetEquippedSkillFuncBySkillSlotIndex: 17 inserts, lines 3391..4470
- X2GURaven.cpp InitState: 28 inserts, lines 735..1090
- X2GURaven.cpp InitStateCommon: 90 inserts, lines 1129..1909
- X2GURaven.cpp InitStateSwordTaker: 6 inserts, lines 1927..1967
- X2GURaven.cpp InitStateOverTaker: 7 inserts, lines 1981..2030
- X2GURaven.cpp InitStateWeaponTaker: 9 inserts, lines 2046..2110
- X2GURaven.cpp InitPassiveSkillState: 8 inserts, lines 2179..2557
- X2GURaven.cpp SetEquippedSkillFuncBySkillSlotIndex: 56 inserts, lines 2983..4121
  Functions named SetEquippedSkillFuncBySkillSlotIndex are mid-game; InitState / InitStateCommon / InitStateByUnitClass / InitState<Weapon> / InitPassiveSkillState (Raven) are init-time. X2GUUser.cpp inserts: JuneInitWaitStateChange :22111, JuneInitEventStates :22249 (loop over local array, called from InitStateCommonRandom ~:33898/:34017), JuneInitBoundingStates :30614-30616 -- all init-time.
- No erase; `m_StateList.clear()` only in the destructor X2GUUser.cpp:1552 (~CX2GUUser at :1331).
- Other container reads: X2GUUser.cpp:22825 `m_StateList.end()==m_StateList.find(iState)` (JuneStartEventKit), :33940 find, :34003 `m_StateList.end() != m_StateList.find( USI_WAIT_STATE_CHANGE )`, :5705 InitState loop.

## 6. State-id accessors and named functions

- GetNowStateID / GetFutureStateID / GetNowStateSkillID / GetFutureStateSkillID / IsHyperState: X2GUUser.h:1808-1815 (inline). m_FrameDataNow.syncData.nowState / m_FrameDataFuture.syncData.nowState hold the id; DoStateEndStart(Future) copy the container entry for that id (Now additionally re-stamps stateID).
- ChangeEquippedSkillState: X2GUUser.cpp:6152 and :6189 (sec 5); CX2User::ChangeEquippedSkillState X2User.cpp:230 is separate.
- SetEquippedSkillFuncBySkillSlotIndex: pure virtual X2GUUser.h:3411; defined per class (sec 5). Each: Init() both slot entries in place, set stateID / hyper flag, then switch(skill) assigning callbacks and inserting sub-states.
