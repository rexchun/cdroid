#define LOG_TAG "ContextImpl"
#include <cutils/log.h>

#include <runtime/ContextImpl.h>
#include <runtime/ActivityThread.h>

#include <service/IActivityManager.h>

namespace cdroid {

ContextImpl::ContextImpl()
{
    mServiceConnectionMgr = new ServiceConnectionManager();
    mReceiverMgr = new ReceiverManager();
}

ContextImpl* ContextImpl::createSystemContext(ActivityThread *mainThread)
{
    ContextImpl *context = new ContextImpl;
    context->init(NULL, NULL,mainThread, NULL);
    return context;
}

void ContextImpl::init(ActivityManifest* amf, sp<IBinder> token, sp<ActivityThread> thread, sp<Looper> cmdLooper)
{
    mActivityMF = amf;
    mToken = token;
    mThread= thread;

    mCmdLooper = cmdLooper;
}

void ContextImpl::init(ServiceManifest* smf, sp<IBinder> token, sp<ActivityThread> thread)
{
    mServiceMF = smf;
    mToken = token;
    mThread= thread;
}

void ContextImpl::setOuterContext(sp<Context> context)
{
    mOuterContext = context;
}

sp<Context> ContextImpl::getOuterContext()
{
    return mOuterContext;
}

sp<Handler> ContextImpl::getCmdHandler()
{
    if(mCmdHandler == NULL) {
        AutoMutex _l(mCmdMutex);
        if(mCmdHandler == NULL) {
            mCmdHandler = new H(this, mCmdLooper);
        }
    }

    return mCmdHandler;
}

int ContextImpl::execInternalCommand(String8 cmd)
{
    sp<Text> txt = new Text(cmd);
    sp<Message> msg = new Message(H::ON_CMD, txt);

    getCmdHandler()->sendMessage(msg);

    return 0;
}

int ContextImpl::startActivity(sp<Intent> intent)
{
    //ALOGI("startActivity %s", intent->getAction().string());

    ActivityManagerNative::getDefault()->startActivity(mThread->getApplicationThread(), mToken, intent, -1);

    return 0;
}

int ContextImpl::startService(sp<Intent> intent)
{
    //ALOGI("startService %s", intent->getAction().string());
    ActivityManagerNative::getDefault()->startService(mThread->getApplicationThread(), intent);
    return 0;
}

int ContextImpl::bindService(sp<Intent> intent, sp<ServiceConnection> conn)
{
    sp<IServiceConnection> sd = mServiceConnectionMgr->getServiceDispatcher(conn, getOuterContext(), mThread->getHandler(), 0);
    ActivityManagerNative::getDefault()->bindService(mThread->getApplicationThread(), mToken, intent, sd->asBinder(), 0);
}

class ContextTestServiceConnection : public ServiceConnection {
public:
    virtual void onServiceConnected(sp<ComponentName> name, sp<IBinder> service)
    {
        ALOGI("ContextTestServiceConnection onServiceConnection %s", name->getName().string());
    }
    virtual void onServiceDisconnected(sp<ComponentName> name)
    {
        ALOGI("ContextTestServiceConnection onServiceDisconnected %s", name->getName().string());
    }
};

int ContextImpl::registerReceiver(sp<BroadcastReceiver> receiver, sp<IntentFilter> filter)
{
    sp<Handler> mainHandler = mThread->getHandler();
    sp<IIntentReceiver> rd = mReceiverMgr->getReceiverDispatcher(receiver, getOuterContext(), mThread->getHandler());
    ActivityManagerNative::getDefault()->registerReceiver(mThread->getApplicationThread(), rd->asBinder(), filter);

}

int ContextImpl::sendBroadcast(sp<Intent> intent)
{
    ActivityManagerNative::getDefault()->broadcastIntent(mThread->getApplicationThread(), intent, NULL, NULL, false, false);

}

class ContextTestBroadcastReceiver : public BroadcastReceiver {
public:
    virtual void onReceive(sp<Context> context, sp<Intent> intent) {
        ALOGI("ContextTestBroadcastReceiver onReceive %s", intent->getAction().string());
    }
};

int ContextImpl::handleInternalCommand(String8 cmd)
{
    //ALOGI("handleInternalcommand %s", cmd.string());
    const char *space = " ";
    int pos = cmd.find(space);
    //ALOGI("handleInternalcommand pos %d", pos);
    switch(pos) {
        case 16:
            {
                sp<IntentFilter> inf = new IntentFilter(String8(cmd.string() + pos + 1));
                sp<ContextTestBroadcastReceiver> r = new ContextTestBroadcastReceiver();
                return registerReceiver(r, inf);
            }
            break;
        case 13:
            {
                sp<Intent> intent = new Intent(String8(cmd.string() + pos + 1));
                return startActivity(intent);
            }
            break;
        case 12:
            {
                sp<Intent> intent = new Intent(String8(cmd.string() + pos + 1));
                return startService(intent);
            }
            break;
        case 11:
            {
                sp<ContextTestServiceConnection> testconn = new ContextTestServiceConnection();

                sp<Intent> intent = new Intent(String8(cmd.string() + pos + 1));
                return bindService(intent, testconn);
            }
            break;
        case 9:
            {
                sp<Intent> intent = new Intent(String8(cmd.string() + pos + 1));
                return sendBroadcast(intent);
            }
            break;
    }

    ALOGI("handleInternalcommand ommit unknow command %s", cmd.string());
    
    return -1;
}

void ContextImpl::H::handleMessage(const sp<Message>& message)
{
    switch(message->what) {
        case ON_CMD:
            {
                sp<Text> txt = reinterpret_cast<Text*>(message->obj.get());
                mContext->handleInternalCommand(txt->getString());
            }
            break;
    }
}

ServiceDispatcher::ServiceDispatcher(sp<ServiceConnection> conn, sp<Context> context, sp<Handler> mainHandler)
    :mConnection(conn),
    mContext(context),
    mMainThreadHandler(mainHandler)
{
    mIServiceConnection = new InnerServiceConnection(conn, mainHandler);
}

sp<IServiceConnection> ServiceDispatcher::getIServiceConnection()
{
    return mIServiceConnection;
}

ServiceDispatcher::InnerServiceConnection::InnerServiceConnection(sp<ServiceConnection> conn, sp<Handler> handler)
    : mConnection(conn),
    mMainThreadHandler(handler)
{
}


void ServiceDispatcher::InnerServiceConnection::connected(sp<ComponentName> name, sp<IBinder> service)
{
    mMainThreadHandler->post(new RunConnection(mConnection, name, service, 0));
}

ServiceConnectionManager::ServiceConnectionManager()
{
}

sp<IServiceConnection> ServiceConnectionManager::getServiceDispatcher(sp<ServiceConnection> conn, sp<Context> context, sp<Handler> handler, int flags)
{
    AutoMutex _l(mMutex);

    map<sp<Context>, map<sp<ServiceConnection>, sp<ServiceDispatcher> >* >::iterator it = mServices.find(context);
    map<sp<ServiceConnection>, sp<ServiceDispatcher> >* csdmap = NULL;

    map<sp<ServiceConnection>, sp<ServiceDispatcher> >::iterator it2;
    sp<ServiceDispatcher> sd;

    if(it != mServices.end()) {
        csdmap = it->second;
        it2 = csdmap->find(conn);
        sd = it2->second;
    }

    if(sd == NULL) {
        sd = new ServiceDispatcher(conn, context, handler);
        if(csdmap == NULL) {
            csdmap = new map<sp<ServiceConnection>, sp<ServiceDispatcher> >();
            mServices.insert(pair<sp<Context>,map<sp<ServiceConnection>,sp<ServiceDispatcher> >* >(context, csdmap));
        }
        csdmap->insert(pair<sp<ServiceConnection>,sp<ServiceDispatcher> >(conn, sd));
    }
    else {
    }

    return sd->getIServiceConnection();
}

ReceiverDispatcher::ReceiverDispatcher(sp<BroadcastReceiver> receiver, sp<Context> context, sp<Handler> mainHandler)
    : mReceiver(receiver),
    mContext(context),
    mMainHandler(mainHandler)
{
    mIntentReceiver = new InnerIntentReceiver(receiver, context, mainHandler);
}

sp<IIntentReceiver> ReceiverDispatcher::getIIntentReceiver()
{
    return mIntentReceiver;
}

ReceiverDispatcher::InnerIntentReceiver::InnerIntentReceiver(sp<BroadcastReceiver> receiver, sp<Context> context, sp<Handler> mainHandler)
    : mReceiver(receiver),
    mContext(context),
    mMainThreadHandler(mainHandler)
{
}

void ReceiverDispatcher::InnerIntentReceiver::performReceive(sp<Intent> intent, sp<Bundle> extra, bool ordered, bool sticky)
{
    ALOGI("TODO post Receive");
}

ReceiverManager::ReceiverManager()
{
}

sp<IIntentReceiver> ReceiverManager::getReceiverDispatcher(sp<BroadcastReceiver> receiver, sp<Context> context, sp<Handler> handler)
{
    AutoMutex _l(mMutex);
    map<sp<Context>, map<sp<BroadcastReceiver>, sp<ReceiverDispatcher> >* >::iterator it = mReceivers.find(context);
    map<sp<BroadcastReceiver>, sp<ReceiverDispatcher> >* rdmap = NULL;

    map<sp<BroadcastReceiver>, sp<ReceiverDispatcher> >::iterator it2;
    sp<ReceiverDispatcher> rd;

    if(it != mReceivers.end()) {
        rdmap = it->second;
        it2 = rdmap->find(receiver);
        rd = it2->second;
    }

    if(rd == NULL) {
        rd = new ReceiverDispatcher(receiver, context, handler);
        if(rdmap == NULL) {
            rdmap = new map<sp<BroadcastReceiver>, sp<ReceiverDispatcher> >();
            mReceivers.insert(pair<sp<Context>,map<sp<BroadcastReceiver>,sp<ReceiverDispatcher> >* >(context, rdmap));
        }
        rdmap->insert(pair<sp<BroadcastReceiver>,sp<ReceiverDispatcher> >(receiver, rd));
    }
    else {
    }

    return rd->getIIntentReceiver();
}

};
