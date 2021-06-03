#pragma once

#include "LoadBalancing-cpp/inc/Client.h"
#include "PA2U_BaseView.h"

struct LocalPlayerBase
{
    LocalPlayerBase();
    float x;
    float y;
    float z;
};

class PA2U_LoadBalancingListener : public ExitGames::LoadBalancing::PA2U_LoadBalancingListener
{
    public:
        PA2U_LoadBalancingListener(PA2U_BaseView *pView){}
        ~PA2U_LoadBalancingListener(){}

        void setLoadBalancingClient(ExitGames::LoadBalancing::Client* pLBC);
        void connect(const ExitGames::Common::JString& userName);
        void service();
        void createRoom();
        bool setLocalPlayerTransform(float x, float y, float z);
        void moveLocalPlayerTransform();

    private:
        // 共通実装になりそうなのでとりあえずサンプルの宣言をそのまま引用 (Mojatto, 2021/06/03)

        //From Common::BaseListener

        // receive and print out debug out here
        virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);

        //From LoadBalancing::LoadBalancingListener

        // implement your error-handling here
        virtual void connectionErrorReturn(int errorCode);
        virtual void clientErrorReturn(int errorCode);
        virtual void warningReturn(int warningCode);
        virtual void serverErrorReturn(int errorCode);

        // events, triggered by certain operations of all players in the same room
        virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
        virtual void leaveRoomEventAction(int playerNr, bool isInactive);
        virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);

        // callbacks for operations on PhotonLoadBalancing server
        virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& region, const ExitGames::Common::JString& cluster);
        virtual void disconnectReturn(void);
        virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
        virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
        virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
        virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
        virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
        virtual void gotQueuedReturn(void);
        virtual void joinLobbyReturn(void);
        virtual void leaveLobbyReturn(void);
        virtual void onRoomListUpdate(void);
        virtual void onLobbyStatsUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats);
        virtual void onLobbyStatsResponse(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats);
        virtual void onRoomPropertiesChange(const ExitGames::Common::Hashtable& changes);

        void updateState();
        void afterRoomJoined(int localPlayerNumber);
        
        ExitGames::LoadBalancing::Client* mpLBC;
        PA2U_BaseView*                    mpView;
        LocalPlayerBase                   mLocalPlayerBase;
        ExitGames::Common::JString        mMap;
        
        int  mLocalPlayerNumber;
};