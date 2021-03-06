#ifndef _SERVICE_ACTIVESERVICES_H
#define _SERVICE_ACTIVESERVICES_H

#include <runtime/Intent.h>
#include <runtime/IServiceConnection.h>
#include "ServiceRecord.h"
#include "ProcessRecord.h"

namespace cdroid {

class ActivityManagerService;

class ActiveServices : public RefBase
{
public:
    ActiveServices(sp<ActivityManagerService> ams);

    int attachApplicationLocked(sp<ProcessRecord> app);
    int startServiceLocked(sp<IApplicationThread> caller, sp<Intent> intent, int pid, int uid);
    int bindServiceLocked(sp<IApplicationThread> caller, sp<IBinder> token, sp<Intent> intent, sp<IServiceConnection> connection, int pid, int uid, int flags);
    int publishServiceLocked(sp<IBinder> token, sp<Intent> intent, sp<IBinder> service);

protected:

    sp<ServiceRecord> retrieveServiceLocked(sp<Intent> intent);
    int requestServiceBindingsLocked(sp<ServiceRecord> r);
    int requestServiceBindingsLocked(sp<ServiceRecord> r, sp<IntentBindRecord> i, bool rebind);

private:
    int startServiceLocked(sp<ServiceRecord> r);
    int realStartServiceLocked(sp<ServiceRecord> r, sp<ProcessRecord> app);

private:
    sp<ActivityManagerService> mAMS;

    Vector<sp<ServiceRecord> > mServices;
    sp<PackageManager>          mPM;
};



};

#endif
