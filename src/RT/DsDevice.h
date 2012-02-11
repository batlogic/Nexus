#ifndef _DSDEVICE_H
#define _DSDEVICE_H


struct DsDevice {
    LPGUID id[2];

    bool validId[2];

    bool found;

    std::string name;

    inline DsDevice() : found(false) { validId[0] = false; validId[1] = false; };

};
#endif
