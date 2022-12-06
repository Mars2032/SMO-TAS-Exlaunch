#pragma once

#include "game/GameData/GameDataHolder.h"

namespace SaveDataAccessFunction {

    void startSaveDataInitSync(GameDataHolder*);

    void startSaveDataReadSync(GameDataHolder*);

    void startSaveDataWriteSync(GameDataHolder*);
}
