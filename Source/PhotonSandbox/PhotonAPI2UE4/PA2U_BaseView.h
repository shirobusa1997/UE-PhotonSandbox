#pragma once

#include "Common-cpp/inc/Common.h"

class PA2U_BaseView
{
    public:
        virtual void updateState(int state, const ExitGames::Common::JString& stateStr, const ExitGames::JString& joinedRoomName);
        virtual void initPlayers(void);
        virtual void addPlayer(int playerNumber, const ExitGames::Common::JString& playerName, bool isLocal);
        virtual void removePlayer(int playerNumber);
        virtual void changePlayerTransform(int playerNumber, float Pos_x, float Pos_y, float Pos_z);
        virtual void setupScene();
        virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames);
};