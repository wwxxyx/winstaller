#ifndef _IPHONE_H_
#define _IPHONE_H_

#include <string>
#include "MobileDevice.h"

using namespace std;

struct am_device;
struct am_device_notification_callback_info;
struct am_recovery_device;

class IPhoneComm
{
    public:
        enum Notifications
        {
            NotifyConnected,
            NotifyDisconnected,
            NotifyJailbreakFailed,
            NotifyJailbreakSucceeded,
            NotifyReturnToJailFailed,
            NotifyReturnToJailSucceeded,
            NotifyApplicationInstallFailed,
            NotifyApplicationInstallSucceeded,
            NotifyActivateFailed,
            NotifyActivateSucceeded
        };

        enum LogLevel
        {
            None,
            Info,
            Verbose
        };
        
    public:
        ~IPhoneComm();
    
        static IPhoneComm* Instance();
        
        bool initialize();
        bool activate();
        bool deactivate();
        void setNotificationCallback( void (*notificationCallback)( enum IPhoneComm::Notifications notification ) );
        bool installApplication( string applicationPath );
        bool jailbreak();
        bool returnToJail();
        bool sendFileToDevice( string sourcePath, string destinationPath );
        bool sendFileToDevice( const unsigned char* data, int len, string destinationPath );
        bool setRamdiskAndKernelCache( string ramdiskPath, string kernelCachePath );
        
        bool isConnected();
        bool isJailbroken();
        bool isActivated();
        
    protected:
        IPhoneComm();
        IPhoneComm(const IPhoneComm&);
		IPhoneComm& operator= (const IPhoneComm&);
 
    private:
        enum DeviceState
        {
            Idle,
            EnteringRecovery,
            EnteringRestore,
            ExitingRestore,
            ReturnToJail
        };

        struct iTunesDllPatch
        {
            unsigned long performRestoreOffset;
            
            unsigned long sendCommandOffset;
            unsigned long sendFileOffset;
            unsigned long socketForPortOffset;
            unsigned long performOperationOffset;
            
            unsigned long sendCommandCrc;
            unsigned long sendFileCrc;
            unsigned long socketForPortCrc;
            unsigned long performOperationsCrc;
        };
        
        static IPhoneComm* instance;
    
        am_device *iPhone;
        am_recovery_device *iPhoneRecovery;
        
        afc_connection *afcConnection;
    
        enum DeviceState state;
        
        bool connected;
        bool jailbroken;
        bool activated;
    
        string ramdiskPath;
        string kernelCachePath;
        string applicationPath;
        string applicationName;
        
        struct iTunesDllPatch patches[2];      
        
        void (*notificationCallback)(enum IPhoneComm::Notifications notification);
        	
        bool matchFunctionCrc( FILE *f, unsigned long offset, unsigned long crc );
        bool loadItunesDLL();
        void resetConnection();
        bool establishConnection();
        bool enterRecovery( bool noRestore );
        void exitRecovery();
        void enterRestoreMode();
        void doRestoreMode();
        void doFinalStep();
        
        bool createDirectoryOnDevice(const char *dir);
        void putDirRecursively(const char * remoteBasePath, const char *basePath, bool exeOnly);
        void transferApplicationFakingExesRecursively( struct am_restore_device *rdev, const char* destBasePath, const char* srcBasePath, const char* localBasePath );

        void sendNotification(enum IPhoneComm::Notifications notification);

        CFStringRef StringtoCFString(string input);
        string CFStringtoString(CFStringRef input);
        
        void handleRestoreNotification(am_recovery_device *rdev);
        void handleDeviceNotification(am_device_notification_callback_info *info); 
        void handleRecoveryDisconnect(struct am_recovery_device *rdev);
        void handleRecoveryConnect(struct am_recovery_device *rdev);
        
        static void _handleDeviceNotification(am_device_notification_callback_info *info); 
        static void _handleRecoveryDisconnect(struct am_recovery_device *rdev);
        static void _handleRecoveryConnect(struct am_recovery_device *rdev);
        
        void * VsendCommandToDevice;
        void * VsendFileToDevice;
        void * VsocketForPort;
        void * VperformOperation;
    
        unsigned int sendCommandToDevice(am_recovery_device *rdev, CFStringRef cfs); 
        unsigned int sendFileToDevice(am_recovery_device * rdev, CFStringRef fname);
        unsigned int socketForPort(struct am_restore_device * rdev, unsigned int port);
        unsigned int performOperation(am_restore_device * rdev, CFDictionaryRef cfdr);
        
        static const char *rwFstab;
        static const char *modifiedServicesPlist;
        static const char *roFstab;
        static const char *originalServicesPlist;
        

        
};


#endif 

