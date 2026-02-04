#ifndef __GAME_GAME_H__
#define __GAME_GAME_H__

#include "application.h"


class Game : public Application {
public:
    bool OnInit() override;
    void OnCleanup() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    bool OnProcessEvents() override;
};


#endif//__GAME_GAME_H__