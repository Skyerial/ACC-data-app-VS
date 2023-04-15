#include "SharedFileOut.h"
#include "sessionData.h"

void SessionData::setSessionData(SPageFileStatic* pfs) {
    track = pfs->track;
    carModel = pfs->carModel;
}

void SessionData::setBestLap() {

}

void SessionData::print() {
    printf("%ls\n%ls\n%d\n", track, carModel, bestLap);
}