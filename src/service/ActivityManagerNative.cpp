#define LOG_TAG "ActivityManagerNative"
#include <cutils/log.h>
#include <binder/IServiceManager.h>

#include <service/IActivityManager.h>

namespace cdroid {
enum {
    TRANSACTION_attachApplication = (android::IBinder::FIRST_CALL_TRANSACTION + 0),
    TRANSACTION_startActivity,
    TRANSACTION_startService,
    TRANSACTION_bindService,
    TRANSACTION_publishService,
    TRANSACTION_registerReceiver,
    TRANSACTION_broadcastIntent,
};

class BpActivityManager: public BpInterface<IActivityManager>
{
public:
    BpActivityManager(const sp<IBinder>& impl)
        : BpInterface<IActivityManager>(impl)
    {
    }
    virtual void attachApplication(sp<IBinder> appThread)
    {
        Parcel _data;
        Parcel _reply;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(appThread);
        remote()->transact(TRANSACTION_attachApplication, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
        }
        else {
            // ERROR
        }
    }

    virtual int startActivity(sp<IBinder> caller, sp<IBinder> resultTo, sp<Intent> intent, int requestCode)
    {
        Parcel _data;
        Parcel _reply;
        int    _result = -1;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(caller);
        _data.writeStrongBinder(resultTo);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        _data.writeInt32(requestCode);
        remote()->transact(TRANSACTION_startActivity, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            _result = -1;
        }
        return _result;
    }

    virtual int startService(sp<IBinder> caller, sp<Intent> intent)
    {
        Parcel _data;
        Parcel _reply;
        int    _result = -1;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(caller);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        remote()->transact(TRANSACTION_startService, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            _result = -1;
        }
        return _result;
    }
    virtual int bindService(sp<IBinder> caller, sp<IBinder> token, sp<Intent> intent, sp<IBinder> connection, int flags)
    {
        Parcel _data;
        Parcel _reply;
        int    _result = -1;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(caller);
        _data.writeStrongBinder(token);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        _data.writeStrongBinder(connection);
        _data.writeInt32(flags);
        remote()->transact(TRANSACTION_bindService, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            _result = -1;
        }
        return _result;
    }
    virtual int publishService(sp<IBinder> token, sp<Intent> intent, sp<IBinder> service)
    {
        Parcel _data;
        Parcel _reply;
        int    _result = -1;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(token);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        _data.writeStrongBinder(service);
        remote()->transact(TRANSACTION_publishService, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            _result = -1;
        }
        return _result;
    }

    virtual int registerReceiver(sp<IBinder> caller, sp<IBinder> receiver, sp<IntentFilter> filter)
    {
        Parcel _data;
        Parcel _reply;
        int    _result = -1;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(caller);
        _data.writeStrongBinder(receiver);
        filter->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        remote()->transact(TRANSACTION_registerReceiver, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            _result = -1;
        }
        return _result;
    }
    virtual int broadcastIntent(sp<IBinder> caller, sp<Intent> intent, sp<IBinder> resultTo, sp<Bundle> map, bool serialized, bool sticky)
    {
        Parcel _data;
        Parcel _reply;
        int    _result = -1;

        _data.writeInterfaceToken(this->getInterfaceDescriptor());
        _data.writeStrongBinder(caller);
        intent->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        _data.writeStrongBinder(resultTo);
        map->writeToParcel(&_data, android::Parcelable::PARCELABLE_WRITE_RETURN_VALUE);
        if(serialized) {
            _data.writeInt32(1);
        } else {
            _data.writeInt32(0);
        }
        if(sticky) {
            _data.writeInt32(1);
        } else {
            _data.writeInt32(0);
        }
        remote()->transact(TRANSACTION_broadcastIntent, _data, &_reply, 0);
        _reply.readExceptionCode();
        if ((0!=_reply.readInt32())) {
            _result = _reply.readInt32();
        }
        else {
            _result = -1;
        }
        return _result;
    }
};

IMPLEMENT_META_INTERFACE(ActivityManager, "com::cdroid::service::IActivityManager");
sp<IActivityManager> ActivityManagerNative::sProxy;
Mutex ActivityManagerNative::mSingletonMutex;

sp<IActivityManager> ActivityManagerNative::getDefault()
{
    if(!sProxy.get()) {
        AutoMutex _l(mSingletonMutex);
        if(!sProxy.get()) {
            sp<IServiceManager> sm = android::defaultServiceManager();
            sp<IBinder> const service = sm->checkService(String16("activity"));
            sProxy = IActivityManager::asInterface(service);
            assert(sProxy.get() != NULL);
        }
    }
    return sProxy;
}


int BnActivityManager::onTransact(uint32_t code, const Parcel& data, Parcel* reply,uint32_t flags)
{
    switch(code) {
        case TRANSACTION_attachApplication:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0 = data.readStrongBinder();
                attachApplication(_arg0);
                reply->writeInt32(1);
                return true;
            }
            break;
        case TRANSACTION_startActivity:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0;
                sp<IBinder> _arg1;
                sp<Intent> _arg2;
                int       _arg3;
                _arg0 = data.readStrongBinder();
                _arg1 = data.readStrongBinder();
                _arg2 = _arg2->createFromParcel(data);
                _arg3 = data.readInt32();
                int _result = startActivity(_arg0, _arg1, _arg2, _arg3);
                reply->writeInt32(1);
                reply->writeInt32(_result);
                return true;
            }
        case TRANSACTION_startService:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0;
                sp<Intent> _arg1;
                _arg0 = data.readStrongBinder();
                _arg1 = _arg1->createFromParcel(data);
                int _result = startService(_arg0, _arg1);
                reply->writeInt32(1);
                reply->writeInt32(_result);
                return true;
            }
        case TRANSACTION_bindService:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0;
                sp<IBinder> _arg1;
                sp<Intent> _arg2;
                sp<IBinder> _arg3;
                int         _arg4;
                _arg0 = data.readStrongBinder();
                _arg1 = data.readStrongBinder();
                _arg2 = _arg2->createFromParcel(data);
                _arg3 = data.readStrongBinder();
                _arg4 = data.readInt32();
                int _result = bindService(_arg0, _arg1, _arg2, _arg3, _arg4);
                reply->writeInt32(1);
                reply->writeInt32(_result);
                return true;
            }
        case TRANSACTION_publishService:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0;
                sp<Intent> _arg1;
                sp<IBinder> _arg2;
                _arg0 = data.readStrongBinder();
                _arg1 = _arg1->createFromParcel(data);
                _arg2 = data.readStrongBinder();
                int _result = publishService(_arg0, _arg1, _arg2);
                reply->writeInt32(1);
                reply->writeInt32(_result);
                return true;
            }
        case TRANSACTION_registerReceiver:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0;
                sp<IBinder> _arg1;
                sp<IntentFilter> _arg2;
                _arg0 = data.readStrongBinder();
                _arg1 = data.readStrongBinder();
                _arg2 = _arg2->createFromParcel(data);
                int _result = registerReceiver(_arg0, _arg1, _arg2);
                reply->writeInt32(1);
                reply->writeInt32(_result);
                return true;
            }
        case TRANSACTION_broadcastIntent:
            {
                CHECK_INTERFACE(IActivityManager, data, reply);
                sp<IBinder> _arg0;
                sp<Intent> _arg1;
                sp<IBinder> _arg2;
                sp<Bundle> _arg3;
                bool _arg4;
                bool _arg5;
                _arg0 = data.readStrongBinder();
                _arg1 = _arg1->createFromParcel(data);
                _arg2 = data.readStrongBinder();
                _arg3 = _arg3->createFromParcel(data);
                if(1 == data.readInt32()) {
                    _arg4 = true;
                } else {
                    _arg4 = true;
                }

                if(1 == data.readInt32()) {
                    _arg5 = true;
                } else {
                    _arg5 = true;
                }
                int _result = broadcastIntent(_arg0, _arg1, _arg2, _arg3, _arg4, _arg5);
                reply->writeInt32(1);
                reply->writeInt32(_result);
                return true;
            }
    }

    return BBinder::onTransact(code, data, reply, flags);
}

};
