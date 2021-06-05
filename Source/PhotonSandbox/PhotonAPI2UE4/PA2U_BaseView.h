#pragma once

#include "Common-cpp/inc/Common.h"

class PA2U_BaseView
{
    public:
        virtual void updateState(int state, const ExitGames::Common::JString& stateStr, const ExitGames::JString& joinedRoomName);
        virtual void initPlayers(void);
        virtual void addPlayer(int playerNumber, const ExitGames::Common::JString& playerName, bool isLocal);
        virtual void removePlayer(int playerNumber);
        virtual void changePlayerTransform(int playerNumber, double Pos_x, double Pos_y, double Pos_z);
        virtual void setupScene();
        virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames);
};