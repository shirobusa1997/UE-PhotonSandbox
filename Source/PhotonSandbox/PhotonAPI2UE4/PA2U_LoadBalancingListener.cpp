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

void PA2U_LoadBalancingListener::setLoadBalancingClient(ExitGames::LoadBalancing::Client* pLBC)
{
    this->mpLBC = pLBC;
    return;
}

void PA2U_LoadBalancingListener::connect(const JString& userName)
{
    mpLBC->connect(AuthenticationValues().setUserID(JString() + GETITEMS()), userName);
    return;
}

