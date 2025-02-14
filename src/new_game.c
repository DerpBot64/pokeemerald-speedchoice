#include "global.h"
#include "new_game.h"
#include "random.h"
#include "pokemon.h"
#include "roamer.h"
#include "pokemon_size_record.h"
#include "script.h"
#include "lottery_corner.h"
#include "play_time.h"
#include "mauville_old_man.h"
#include "match_call.h"
#include "lilycove_lady.h"
#include "load_save.h"
#include "pokeblock.h"
#include "dewford_trend.h"
#include "berry.h"
#include "rtc.h"
#include "easy_chat.h"
#include "event_data.h"
#include "money.h"
#include "trainer_hill.h"
#include "tv.h"
#include "coins.h"
#include "text.h"
#include "overworld.h"
#include "mail.h"
#include "battle_records.h"
#include "item.h"
#include "pokedex.h"
#include "apprentice.h"
#include "frontier_util.h"
#include "constants/maps.h"
#include "pokedex.h"
#include "save.h"
#include "link_rfu.h"
#include "main.h"
#include "contest.h"
#include "item_menu.h"
#include "pokemon_storage_system.h"
#include "pokemon_jump.h"
#include "decoration_inventory.h"
#include "secret_base.h"
#include "player_pc.h"
#include "field_specials.h"
#include "berry_powder.h"
#include "mevent.h"
#include "union_room_chat.h"
#include "speedchoice.h"
#include "constants/items.h"
#include "done_button.h"
#include "constants/moves.h"
#include "malloc.h"
#include "pokedex.h"

extern const u8 EventScript_ResetAllMapFlags[];

// this file's functions
static void ClearFrontierRecord(void);
static void WarpToTruck(void);
static void ResetMiniGamesResults(void);

// EWRAM vars
EWRAM_DATA bool8 gDifferentSaveFile = FALSE;
EWRAM_DATA bool8 gEnableContestDebugging = FALSE;

// const rom data
static const struct ContestWinner sContestWinnerPicDummy =
{
    .monName = _(""),
    .trainerName = _("")
};

// code
void SetTrainerId(u32 trainerId, u8 *dst)
{
    dst[0] = trainerId;
    dst[1] = trainerId >> 8;
    dst[2] = trainerId >> 16;
    dst[3] = trainerId >> 24;
}

u32 GetTrainerId(u8 *trainerId)
{
    return (trainerId[3] << 24) | (trainerId[2] << 16) | (trainerId[1] << 8) | (trainerId[0]);
}

void CopyTrainerId(u8 *dst, u8 *src)
{
    s32 i;
    for (i = 0; i < TRAINER_ID_LENGTH; i++)
        dst[i] = src[i];
}

static void InitPlayerTrainerId(void)
{
    u32 trainerId = (Random() << 0x16) | GetGeneratedTrainerIdLower();
    SetTrainerId(trainerId, gSaveBlock2Ptr->playerTrainerId);
}

// L=A isnt set here for some reason.

/*
OPTIONs should be
- Text Speed INST
- Battle Scene OFF
- Battle Style OFF
- Sound MONO
- Button Mode L=A
- Frame TYPE1
by default
*/
static void SetDefaultOptions(void)
{
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_INST; // INSTANT TEXT
    gSaveBlock2Ptr->optionsWindowFrameType = 0;
    gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
    gSaveBlock2Ptr->optionsBattleSceneOff = TRUE;
    gSaveBlock2Ptr->optionsButtonMode = OPTIONS_BUTTON_MODE_L_EQUALS_A;
    gSaveBlock2Ptr->regionMapZoom = FALSE;
    gSaveBlock2Ptr->optionsNickname = TRUE;
}

static void ClearPokedexFlags(void)
{
    gUnusedPokedexU8 = 0;
    memset(&gSaveBlock2Ptr->pokedex.owned, 0, sizeof(gSaveBlock2Ptr->pokedex.owned));
    memset(&gSaveBlock2Ptr->pokedex.seen, 0, sizeof(gSaveBlock2Ptr->pokedex.seen));
}

void ClearAllContestWinnerPics(void)
{
    s32 i;

    ClearContestWinnerPicsInContestHall();
    for (i = 8; i < 13; i++)
        gSaveBlock1Ptr->contestWinners[i] = sContestWinnerPicDummy;
}

static void ClearFrontierRecord(void)
{
    CpuFill32(0, &gSaveBlock2Ptr->frontier, sizeof(gSaveBlock2Ptr->frontier));

    gSaveBlock2Ptr->frontier.opponentNames[0][0] = EOS;
    gSaveBlock2Ptr->frontier.opponentNames[1][0] = EOS;
}

static void WarpToTruck(void)
{
    SetWarpDestination(MAP_GROUP(INSIDE_OF_TRUCK), MAP_NUM(INSIDE_OF_TRUCK), -1, -1, -1);
    WarpIntoMap();
    sInIntro = FALSE;
    sInSubMenu = FALSE;
    sInBattle = FALSE;
    sInField = TRUE;
}

void Sav2_ClearSetDefault(void)
{
    ClearSav2();
    SetDefaultOptions();
}

void ResetMenuAndMonGlobals(void)
{
    gDifferentSaveFile = 0;
    ResetPokedexScrollPositions();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetBagScrollPositions();
    ResetPokeblockScrollPositions();
}

void NewGameResetDoneButtonStats(){

	//excludes timers, those are done on hitting the go button
	gSaveBlock1Ptr->doneButtonStats.saveCount = 0;
	gSaveBlock1Ptr->doneButtonStats.reloadCount = 0;
	gSaveBlock1Ptr->doneButtonStats.clockResetCount = 0;
	gSaveBlock1Ptr->doneButtonStats.stepCount = 0;
	gSaveBlock1Ptr->doneButtonStats.stepCountWalk = 0;
	gSaveBlock1Ptr->doneButtonStats.stepCountSurf = 0;
	gSaveBlock1Ptr->doneButtonStats.stepCountBike = 0;
	gSaveBlock1Ptr->doneButtonStats.stepCountRun = 0;
	gSaveBlock1Ptr->doneButtonStats.bonks = 0;
	gSaveBlock1Ptr->doneButtonStats.totalDamageDealt = 0;
	gSaveBlock1Ptr->doneButtonStats.actualDamageDealt = 0;
	gSaveBlock1Ptr->doneButtonStats.totalDamageTaken = 0;
	gSaveBlock1Ptr->doneButtonStats.actualDamageTaken = 0;
	gSaveBlock1Ptr->doneButtonStats.ownMovesHit = 0;
	gSaveBlock1Ptr->doneButtonStats.ownMovesMissed = 0;
	gSaveBlock1Ptr->doneButtonStats.enemyMovesHit = 0;
	gSaveBlock1Ptr->doneButtonStats.enemyMovesMissed = 0;
	//save block 2
	gSaveBlock2Ptr->doneButtonStats.ownMovesSE = 0;
	gSaveBlock2Ptr->doneButtonStats.ownMovesNVE = 0;
	gSaveBlock2Ptr->doneButtonStats.enemyMovesSE = 0;
	gSaveBlock2Ptr->doneButtonStats.enemyMovesNVE = 0;
	gSaveBlock2Ptr->doneButtonStats.critsDealt = 0;
	gSaveBlock2Ptr->doneButtonStats.OHKOsDealt = 0;
	gSaveBlock2Ptr->doneButtonStats.critsTaken = 0;
	gSaveBlock2Ptr->doneButtonStats.OHKOsTaken = 0;
	gSaveBlock2Ptr->doneButtonStats.playerHPHealed = 0;
	gSaveBlock2Ptr->doneButtonStats.enemyHPHealed = 0;
	gSaveBlock2Ptr->doneButtonStats.playerPokemonFainted = 0;
	gSaveBlock2Ptr->doneButtonStats.enemyPokemonFainted = 0;
	gSaveBlock2Ptr->doneButtonStats.expGained = 0;
	gSaveBlock2Ptr->doneButtonStats.switchouts = 0;
	gSaveBlock2Ptr->doneButtonStats.battles = 0;
	gSaveBlock2Ptr->doneButtonStats.trainerBattles = 0;
	gSaveBlock2Ptr->doneButtonStats.wildBattles = 0;
	gSaveBlock2Ptr->doneButtonStats.battlesFled = 0;
	gSaveBlock2Ptr->doneButtonStats.failedRuns = 0;
	gSaveBlock2Ptr->doneButtonStats.moneyMade = 0;
	gSaveBlock2Ptr->doneButtonStats.moneySpent = 0;
	gSaveBlock2Ptr->doneButtonStats.moneyLost = 0;
	gSaveBlock2Ptr->doneButtonStats.itemsPickedUp = 0;
	gSaveBlock2Ptr->doneButtonStats.itemsBought = 0;
	gSaveBlock2Ptr->doneButtonStats.itemsSold = 0;
	gSaveBlock2Ptr->doneButtonStats.movesLearnt = 0;
	gSaveBlock2Ptr->doneButtonStats.ballsThrown = 0;
	gSaveBlock2Ptr->doneButtonStats.pokemonCaughtInBalls = 0;
	gSaveBlock2Ptr->doneButtonStats.evosAttempted = 0;
	gSaveBlock2Ptr->doneButtonStats.evosCompleted = 0;
	gSaveBlock2Ptr->doneButtonStats.evosCancelled = 0;
}

void NewGameInitData(void)
{
    if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_CORRUPT)
        RtcReset();

    //done at the end of the naming screen in birch intro normally,
    //skipped due to speedchoice changes so added here.
    SeedRngAndSetTrainerId();

    gDifferentSaveFile = 1;
    gSaveBlock2Ptr->encryptionKey = 0;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetPokedex();
    ClearFrontierRecord();
    ClearSav1();
    ClearMailData();
    gSaveBlock2Ptr->specialSaveWarpFlags = 0;
    gSaveBlock2Ptr->gcnLinkFlags = 0;
    InitPlayerTrainerId();
    PlayTimeCounter_Reset();
    ClearPokedexFlags();
    InitEventData();
    ClearTVShowData();
    ResetGabbyAndTy();
    ClearSecretBases();
    ClearBerryTrees();
    SetMoney(&gSaveBlock1Ptr->money, 3000);
    SetCoins(0);
    ResetLinkContestBoolean();
    ResetGameStats();
    ClearAllContestWinnerPics();
    ClearPlayerLinkBattleRecords();
    InitSeedotSizeRecord();
    InitLotadSizeRecord();
    gPlayerPartyCount = 0;
    ZeroPlayerPartyMons();
    ResetPokemonStorageSystem();
    ClearRoamerData();
    ClearRoamerLocationData();
    gSaveBlock1Ptr->registeredItem = 0;
    ClearBag();
    NewGameInitPCItems();
    ClearPokeblocks();
    ClearDecorationInventories();
    InitEasyChatPhrases();
    SetMauvilleOldMan();
    InitDewfordTrend();
    ResetFanClub();
    ResetLotteryCorner();
    WarpToTruck();
    ScriptContext2_RunNewScript(EventScript_ResetAllMapFlags);
    ResetMiniGamesResults();
    InitUnionRoomChatRegisteredTexts();
    InitLilycoveLady();
    ResetAllApprenticeData();
    ClearRankingHallRecords();
    InitMatchCallCounters();
    sub_801AFD8();
    WipeTrainerNameRecords();
    ResetTrainerHillResults();
    ResetContestLinkResults();

    NewGameResetDoneButtonStats();

    //if(CheckSpeedchoiceOption(EARLY_BIKE, EARLY_BIKE_YES) == TRUE) {
        AddBagItem(ITEM_MACH_BIKE, 1);
        FlagSet(FLAG_RECEIVED_BIKE); // put the flag here for making sure you got the bicycle
    //}
	// ADD DONE BUTTON
	AddBagItem(ITEM_DONE_BUTTON, 1);


#if DEVMODE
    {
		u16 i;
		u8 ev = 252;
		u8 ev2 = 6;
		struct Pokemon *mon = (void*) AllocZeroed(sizeof(struct Pokemon));



        if (mon != NULL)
        {
            u32 pid;
            u32 otid = T2_READ_32(gSaveBlock2Ptr->playerTrainerId);
            do
			{
            	pid = Random32();
			} while (NATURE_RASH != GetNatureFromPersonality(pid));
            CreateMon(mon, SPECIES_MEWTWO, 100, 31, TRUE, pid, OT_ID_PLAYER_ID, 0);
            SetMonMoveSlot(mon, MOVE_PSYCHIC, 0);
			SetMonMoveSlot(mon, MOVE_FLY, 1);
			SetMonMoveSlot(mon, MOVE_SURF, 2);
			SetMonMoveSlot(mon, MOVE_THUNDERBOLT, 3);
			SetMonData(mon, MON_DATA_ATK_EV, &ev);
			SetMonData(mon, MON_DATA_SPATK_EV, &ev);
			SetMonData(mon, MON_DATA_SPEED_EV, &ev2);
			CalculateMonStats(mon);
            GiveMonToPlayer(mon);
            Free(mon);
        }
        FlagSet(FLAG_VISITED_PETALBURG_CITY);
        FlagSet(FLAG_VISITED_LITTLEROOT_TOWN);
        FlagSet(FLAG_VISITED_OLDALE_TOWN);
        FlagSet(FLAG_VISITED_DEWFORD_TOWN);
        FlagSet(FLAG_VISITED_LAVARIDGE_TOWN);
        FlagSet(FLAG_VISITED_FALLARBOR_TOWN);
        FlagSet(FLAG_VISITED_VERDANTURF_TOWN);
        FlagSet(FLAG_VISITED_PACIFIDLOG_TOWN);
        FlagSet(FLAG_VISITED_SLATEPORT_CITY);
        FlagSet(FLAG_VISITED_MAUVILLE_CITY);
        FlagSet(FLAG_VISITED_RUSTBORO_CITY);
        FlagSet(FLAG_VISITED_FORTREE_CITY);
        FlagSet(FLAG_VISITED_LILYCOVE_CITY);
        FlagSet(FLAG_VISITED_MOSSDEEP_CITY);
        FlagSet(FLAG_VISITED_SOOTOPOLIS_CITY);
        FlagSet(FLAG_VISITED_EVER_GRANDE_CITY);

        FlagSet(FLAG_BADGE01_GET);
        FlagSet(FLAG_BADGE02_GET);
        FlagSet(FLAG_BADGE03_GET);
        FlagSet(FLAG_BADGE04_GET);
        FlagSet(FLAG_BADGE05_GET);
        FlagSet(FLAG_BADGE06_GET);
        FlagSet(FLAG_BADGE07_GET);
        FlagSet(FLAG_BADGE08_GET);
        FlagSet(FLAG_LANDMARK_POKEMON_LEAGUE);

		SetMoney(&gSaveBlock1Ptr->money, 999999);

		AddBagItem(ITEM_PP_MAX,99);
		AddBagItem(ITEM_MAX_ELIXIR,99);

		for(i = 0; i <50; i++){
			AddBagItem(ITEM_TM01+i,99);
		}

		for(i = 0; i <8; i++){
			AddBagItem(ITEM_HM01+i,1);
		}

		for(i = 1; i <387; i++){
			GetSetPokedexFlag(i, FLAG_SET_SEEN);
		}

    }
#endif //DEVMODE

}

static void ResetMiniGamesResults(void)
{
    CpuFill16(0, &gSaveBlock2Ptr->berryCrush, sizeof(struct BerryCrush));
    SetBerryPowder(&gSaveBlock2Ptr->berryCrush.berryPowderAmount, 0);
    ResetPokeJumpResults();
    CpuFill16(0, &gSaveBlock2Ptr->berryPick, sizeof(struct BerryPickingResults));
}
