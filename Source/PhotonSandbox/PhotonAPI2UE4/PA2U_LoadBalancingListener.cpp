// ライブラリ参照
#include "PA2U_LoadBalancingListener.h"

#ifdef __UNREAL__
    #include "PhotonSandbox.h"
#endif

#include <iostream>
#include <stdlib.h>

#ifdef _EG_ANDROID_PLATFORM 
    #include <stdarg.h>
    #include <andorid/log.h>
#endif

#include "PA2U_LoadBalancingListener.h"
#include "PA2U_DefaultConstants.h"

// 名前空間の使用宣言
using namespace ExitGames::Common;
using namespace ExitGames::LoadBalancing;

// PeerStates
const JString PeerStatesStr[] = {
	L"Uninitialized",
	L"PeerCreated",
	L"ConnectingToNameserver",
	L"ConnectedToNameserver",
	L"DisconnectingFromNameserver",
	L"Connecting",
	L"Connected",
	L"WaitingForCustomAuthenticationNextStepCall",
	L"Authenticated",
	L"JoinedLobby",
	L"DisconnectingFromMasterserver",
	L"ConnectingToGameserver",
	L"ConnectedToGameserver",
	L"AuthenticatedOnGameServer",
	L"Joining",
	L"Joined",
	L"Leaving",
	L"Left",
	L"DisconnectingFromGameserver",
	L"ConnectingToMasterserver",
	L"ConnectedComingFromGameserver",
	L"AuthenticatedComingFromGameserver",
	L"Disconnecting",
	L"Disconnected"
};

class PeerStatesStrChecker
{
    public:
        PeerStatesStrChecker()
        {
            DEBUG_ASSERT(sizeof(PeerStatesStr) / sizeof(JString) == PeerStates::Disconnected + 1);
        }
} checker;

LocalPlayerBase::LocalPlayerBase() : x(0.0f), y(0.0f), z(0.0f)
{}

PA2U_LoadBalancingListener::PA2U_LoadBalancingListener(PA2U_BaseView* pView) : mpLBC(NULL), mpView(pView), mLocalPlayerNumber(0), mMap(DEFAULT_MAP)
{}

PA2U_LoadBalancingListener::~PA2U_LoadBalancingListener() { delete mpView; }

void PA2U_LoadBalancingListener::setLoadBalancingClient(ExitGames::LoadBalancing::Clienlt* pLBC)
{
    this->mpLBC = pLBC;
    return;
}

void PA2U_LoadBalancingListener::connect(const JString& userName)
{
    mpLBC->connect(AuthenticationValues().setUserID(JString() + GETITEMS()), userName);
    return;
}

void PA2U_LoadBalancingListener::debugReturn(int debugLevel, const JString& string)
{
	Console::get().debugReturn(debugLevel, string);
}

void PA2U_LoadBalancingListener::connectionErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"connection failed with error ") + errorCode);

	updateState();
}

void PA2U_LoadBalancingListener::clientErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"received error ") + errorCode + L" from client");

	updateState();
}

void PA2U_LoadBalancingListener::warningReturn(int warningCode)
{
	Console::get().writeLine(JString(L"received warning ") + warningCode + " from client");
}

void PA2U_LoadBalancingListener::serverErrorReturn(int errorCode)
{
	Console::get().writeLine(JString(L"received error ") + errorCode + " from server");

	updateState();
}

void PA2U_LoadBalancingListener::joinRoomEventAction(int playerNr, const JVector<int>& playernrs, const Player& player)
{
	// デバッグ出力
	Console::get().writeLine(JString("player ") + playerNr + L" " + player.getName() + L" has joined the game.");

	mpView->addPlayer(playerNr, player.getName(), player.getNumber() == mpLBC->getLocalPlayer().getNumber());
}

void PA2U_LoadBalancingListener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	if(isInactive)
		Console::get().writeLine(JString(L"player ") + playerNr + L" has suspended the game");
	else
	{
		Console::get().writeLine(JString(L"player ") + playerNr + L" has abandoned the game");

		mpView->removePlayer(playerNr);
	}
}

void PA2U_LoadBalancingListener::customEventAction(int playerNr, nByte eventCode, const Object& eventContentObj)
{
	ExitGames::Common::Hashtable eventContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();

	// イベントコードによる処理分岐
	// 2 : プレイヤーの位置移動
	if (eventCode == 2)
	{	
		
	}
}