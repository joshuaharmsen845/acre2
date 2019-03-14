#pragma once

#include "compat.h"
#include "Types.h"
#include "Macros.h"
#include "Log.h"
#include "IRpcFunction.h"

#include "IServer.h"
#include "Engine.h"

#include "TextMessage.h"

RPC_FUNCTION(setSoundSystemMasterOverride) {

    int status;

    status = vMessage->getParameterAsInt(0);

    if (status == 1) {
        CEngine::getInstance()->setSoundSystemOverride(TRUE);
    } else {
        CEngine::getInstance()->setSoundSystemOverride(FALSE);
    }


    return acre_result_ok;
}
DECLARE_MEMBER(char *, Name);
};