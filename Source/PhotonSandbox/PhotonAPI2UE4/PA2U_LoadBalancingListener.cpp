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
		const Object* obj = eventContent.getValue("1");
		if (!obj) obj = eventContent.getValue((nByte)1);
		if (!obj) obj = eventContent.getValue(1);
		if (!obj) obj = eventContent.getValue(1.0);
		if (obj && obj->getDimensions() == 1 && obj->getSizes()[0] == 2)
		{
			double x = 0; double y = 0; double z = 0;

			if(obj->getType() == TypeCode::DOUBLE)
			{
				double* data = ((ValueObject<double*>*)obj)->getDataCopy();
				x = (double)data[0];
				y = (double)data[1];
				z = (double)data[2];
			}
			else if(obj->getType() == TypeCode::INTEGER)
			{
				int* data = ((ValueObject<int*>*)obj)->getDataCopy();
				x = (double)data[0];
				y = (double)data[1];
				z = (double)data[2];
			}
			else if(obj->getType() == TypeCode::BYTE)
			{
				nByte* data = ((ValueObject<nByte*>*)obj)->getDataCopy();
				x = (double)data[0];
				y = (double)data[1];
				z = (double)data[2];
			}
			else if(obj->getType() == TypeCode::OBJECT)
			{
				Object* data = ((ValueObject<Object*>*)obj)->getDataCopy();
				if(data[0].getType() == TypeCode::INTEGER)
				{
					x = (double)((ValueObject<int>*)(data + 0))->getDataCopy();
					y = (double)((ValueObject<int>*)(data + 1))->getDataCopy();
					z = (double)((ValueObject<int>*)(data + 2))->getDataCopy();
				}
				else
				{
					x = ((ValueObject<double>*)(data + 0))->getDataCopy();
					y = ((ValueObject<double>*)(data + 1))->getDataCopy();
					z = ((ValueObject<double>*)(data + 2))->getDataCopy();
				}
				MemoryManagement::deallocateArray(data);
			}
#ifdef __UNREAL__
			mpView->changePlayerTransform(playerNr, x, y, z);
#else
			mpView->changePlayerTransform(playerNr, x, y, z);
#endif
		}
		else Console::get().writeLine(L"Bad position event.");
	}
}

void PA2U_LoadBalancingListener::connectReturn(int errorCode, const JString& errorString, const JString& region, const JString& cluster)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"connected to cluster " + cluster + L" of region " + region);
		mpLbc->opJoinRandomRoom();
	}
	else
		Console::get().writeLine(JString(L"Warn: connect failed ") + errorCode + L" " + errorString);
}

void PA2U_LoadBalancingListener::disconnectReturn(void)
{
	updateState();
	Console::get().writeLine(L"disconnected");
	mpView->initPlayers();
}

void PA2U_LoadBalancingListener::createRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"room has been created");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"Warn: opCreateRoom() failed: " + errorString);
}

void PA2U_LoadBalancingListener::joinOrCreateRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"room has been entered");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"Warn: opJoinOrCreateRoom() failed: " + errorString);
}

void PA2U_LoadBalancingListener::joinRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"game room has been successfully joined");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"opJoinRoom() failed: " + errorString);
}

void PA2U_LoadBalancingListener::joinRandomRoomReturn(int localPlayerNr, const Hashtable& gameProperties, const Hashtable& playerProperties, int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::NO_MATCH_FOUND)
		createRoom();
	else if(errorCode == ErrorCode::OK)
	{
		Console::get().writeLine(L"game room has been successfully joined");
		afterRoomJoined(localPlayerNr);
	}
	else
		Console::get().writeLine(L"opJoinRandomRoom() failed: " + errorString);
}

void PA2U_LoadBalancingListener::leaveRoomReturn(int errorCode, const JString& errorString)
{
	updateState();
	if(errorCode == ErrorCode::OK)
		Console::get().writeLine(L"game room has been successfully left");
	else
		Console::get().writeLine(L"opLeaveRoom() failed: " + errorString);
	mpView->initPlayers();
}

void PA2U_LoadBalancingListener::gotQueuedReturn(void)
{
	updateState();
}

void PA2U_LoadBalancingListener::joinLobbyReturn(void)
{
	Console::get().writeLine(L"joined lobby");
	updateState();
}

void PA2U_LoadBalancingListener::leaveLobbyReturn(void)
{
	Console::get().writeLine(L"left lobby");
	updateState();
}

void PA2U_LoadBalancingListener::onLobbyStatsUpdate(const JVector<LobbyStatsResponse>& res)
{
	Console::get().writeLine(L"===================== lobby stats update");
	for(unsigned int i=0; i<res.getSize(); ++i)
		Console::get().writeLine(res[i].toString());

	// lobby stats request test
	JVector<LobbyStatsRequest> ls;
	ls.addElement(LobbyStatsRequest());
	ls.addElement(LobbyStatsRequest(L"AAA"));
	ls.addElement(LobbyStatsRequest(L"BBB"));
	ls.addElement(LobbyStatsRequest(L"CCC", LobbyType::DEFAULT));
	ls.addElement(LobbyStatsRequest(L"AAA", LobbyType::SQL_LOBBY));
	mpLBC->opLobbyStats(ls);
}

void PA2U_LoadBalancingListener::onLobbyStatsResponse(const ExitGames::Common::JVector<LobbyStatsResponse>& res)
{
	Console::get().writeLine(L"===================== lobby stats response");
	for(unsigned int i=0; i<res.getSize(); ++i)
		Console::get().writeLine(res[i].toString());
}

void PA2U_LoadBalancingListener::onRoomListUpdate()
{
	const JVector<Room*>& rooms = mpLbc->getRoomList();
	JVector<JString> names(rooms.getSize());
	for(unsigned int i=0; i<rooms.getSize(); ++i)
		names.addElement(rooms[i]->getName());
	mpView->updateRoomList(names);
}

void PA2U_LoadBalancingListener::onRoomPropertiesChange(const ExitGames::Common::Hashtable& changes)
{
	if(updateGridSize(changes))
		mpView->setupScene(mGridSize);
}

void PA2U_LoadBalancingListener::updateState()
{
	int state = mpLbc->getState();
	mpView->updateState(state, PeerStatesStr[state], state == PeerStates::Joined ? mpLBC->getCurrentlyJoinedRoom().getName() : JString());
}

bool PA2U_LoadBalancingListener::updateGridSize(const ExitGames::Common::Hashtable& props)
{
	if(props.contains(L"s"))
	{
		const Object* v = props.getValue(L"s");
		switch(v->getType())
		{
		case TypeCode::INTEGER:
			mGridSize = ((ValueObject<int>*)v)->getDataCopy();
			return true;
		case TypeCode::DOUBLE:
			mGridSize = (int)((ValueObject<double>*)v)->getDataCopy();
			return true;
		}
	}
	return false;
}

void PA2U_LoadBalancingListener::afterRoomJoined(int localPlayerNr)
{
	Console::get().writeLine(JString(L"afterRoomJoined: localPlayerNr=") + localPlayerNr);
	this->mLocalPlayerNr = localPlayerNr;
	MutableRoom& myRoom = mpLbc->getCurrentlyJoinedRoom();
	Hashtable props = myRoom.getCustomProperties();
	updateGridSize(props);
	if(props.contains(L"m"))
		mMap = ((ValueObject<JString>*)props.getValue(L"m"))->getDataCopy();

	mpView->initPlayers();
	mpView->setupScene(mGridSize);

	const JVector<Player*>& players = myRoom.getPlayers();
	for(unsigned int i=0; i<players.getSize(); ++i)
	{
		const Player* p = players[i];
		mpView->addPlayer(p->getNumber(), p->getName(), p->getNumber() == mpLBC->getLocalPlayer().getNumber());
	}
	mpView->changePlayerColor(localPlayerNr, mLocalPlayer.color);
	raiseColorEvent();
}

void PA2U_LoadBalancingListener::createRoom()
{
	JString name = 
#ifdef __UNREAL__
		JString(L"UE-")
#else
		JString(L"native-")
#endif
		 + (rand()%100);
	Hashtable props;
	props.put(L"s", mGridSize);
	props.put(L"m", mMap);
	mpLBC->opCreateRoom(name, RoomOptions().setCustomRoomProperties(props));
	Console::get().writeLine(L"Creating room " + name);
}

void PA2U_LoadBalancingListener::service(void)
{
	unsigned long t = GETTIMEMS();
	if((t - mLocalPlayer.lastUpdateTime) > PLAYER_UPDATE_INTERVAL_MS)
	{
		mLocalPlayer.lastUpdateTime = t;
		if(mpLBC->getState() == PeerStates::Joined)
			moveLocalPlayer();
	}
}

void PA2U_LoadBalancingListener::moveLocalPlayerTransform(void)
{
	double x = mLocalPlayerBase.x;
	double y = mLocalPlayerBase.y;
	double z = mLocalPlayerBase.z;
	setLocalPlayerTransform(x, y, z);
}

bool PA2U_LoadBalancingListener::setLocalPlayerTransform(double x, double y, double z)
{
	mLocalPlayerBase.x = x; mLocalPlayerBase.y = y; mLocalPlayerBase.z = z;
	Hashtable data;

#ifdef __UNREAL__
	nByte coords[] = {static_cast<nByte<(mLocalPlayer.x), static_cast<nByte<(mLocalPlayer.y), static_cast<nByte<(mLocalPlayer.z)};
#else
	nByte coords[] = {static_cast<nByte<(mLocalPlayer.x), static_cast<nByte<(mLocalPlayer.y), static_cast<nByte<(mLocalPlayer.z)};
#endif
	data.put((nByte)1, coords, 3);

	mpLBC->opRaiseEvent(false, data, 3, RaiseEventOptions().setInterestGroup(0));
	mpView->changePlayerTransform(mLocalPlayerNumber, mLocalPlayerBase.x, mLocalPlayerBase.y, mLocalPlayerBase.z);
	return true;
}