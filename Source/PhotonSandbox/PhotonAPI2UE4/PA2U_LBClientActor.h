#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PA2U_LoadBalancingListener.h"

#include "PA2U_LBClientActor.generated.h"

UCLASS()
class PA2U_LBClientActor : public AActor, public PA2U_BaseView
{
    GENERATED_BODY()

    public:
        PA2U_LBClientActor(){}
        ~PA2U_LBClientActor(){}

        UFUNCTION(BlueprintImplementableEvent, Category = "PhotonLB")
        void initPlayers();

    private:
        void BeginPlay();
        void Tick(float DeltaSeconds);
};