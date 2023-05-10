#include "SharedFileOut.h"
#include "sessionData.h"

void SessionData::SetSessionData(SPageFileStatic* pfs, SPageFileGraphic* pfg) {
    session = pfg->session;
    track = pfs->track;
    carModel = pfs->carModel;
}

// This function is slightly useless still
void SessionData::SetBestLap() {
    return;
}

void SessionData::Print() {
    printf("%ls\n%ls\n%d\n", track, carModel, bestLap);
}