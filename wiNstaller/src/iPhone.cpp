#include "iPhone.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <windows.h>

#include "crc.h"
#include "pem.h"
#include "activationrecord.h"

using namespace std;

const char *IPhoneComm::rwFstab = "/dev/disk0s1 / hfs rw 0 1\n/dev/disk0s2 /private/var hfs rw,noexec 0 2";
const char *IPhoneComm::modifiedServicesPlist = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n <key>com.apple.afc2</key>\n     <dict>\n                <key>Label</key>\n              <string>com.apple.afc2</string>\n               <key>ProgramArguments</key>\n   <array>\n                        <string>/usr/libexec/afcd</string>\n                    <string>--lockdown</string>\n                   <string>-d</string>\n                   <string>/</string>\n             </array>\n      </dict>\n       <key>com.apple.afc</key>\n      <dict>\n                <key>Label</key>\n              <string>com.apple.afc</string>\n         <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/libexec/afcd</string>\n                    <string>--lockdown</string>\n   </array>\n       </dict>\n       <key>com.apple.crashreportcopy</key>\n  <dict>\n                <key>InstanceLimit</key>\n              <integer>1</integer>\n          <key>Label</key>\n               <string>com.apple.crashreportcopy</string>\n            <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/libexec/CrashReportCopyAgent</string>\n                     <string>--lockdown</string>\n                   <string>--oneshot</string>\n            </array>\n      </dict>\n       <key>com.apple.mobile.debug_image_mount</key>\n  <dict>\n                <key>Label</key>\n              <string>com.apple.mobile.debug_image_mount</string>\n           <key>ProgramArguments</key>\n            <array>\n                       <string>/usr/libexec/debug_image_mount</string>\n               </array>\n      </dict>\n       <key>com.apple.mobile.notification_proxy</key>\n <dict>\n                <key>Label</key>\n              <string>com.apple.mobile.notification_proxy</string>\n          <key>ProgramArguments</key>\n   <array>\n                        <string>/usr/libexec/notification_proxy</string>\n              </array>\n      </dict>\n       <key>com.apple.mobile.software_update</key>\n   <dict>\n         <key>Label</key>\n              <string>com.apple.mobile.software_update</string>\n             <key>ProgramArguments</key>\n           <array>\n               <string>/usr/libexec/software_update</string>\n          </array>\n      </dict>\n       <key>com.apple.mobile.system_profiler</key>\n   <dict>\n                <key>Label</key>\n               <string>com.apple.mobile.system_profiler</string>\n             <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/sbin/system_profiler</string>\n             </array>\n      </dict>\n       <key>com.apple.mobilebackup</key>\n     <dict>\n                <key>InstanceLimit</key>\n              <integer>1</integer>\n           <key>Label</key>\n              <string>com.apple.mobilebackup</string>\n               <key>ProgramArguments</key>\n           <array>\n               <string>/usr/libexec/BackupAgent</string>\n                      <string>--lockdown</string>\n           </array>\n      </dict>\n       <key>com.apple.mobilesync</key>\n       <dict>\n         <key>InstanceLimit</key>\n              <integer>1</integer>\n          <key>Label</key>\n              <string>com.apple.mobilesync</string>\n         <key>ProgramArguments</key>\n            <array>\n                       <string>/usr/libexec/SyncAgent</string>\n                       <string>--lockdown</string>\n                   <string>--oneshot</string>\n                     <string>-v</string>\n           </array>\n      </dict>\n       <key>com.apple.purpletestr</key>\n      <dict>\n                <key>Label</key>\n               <string>com.apple.purpletestr</string>\n                <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/libexec/PurpleTestr</string>\n                      <string>--lockdown</string>\n                   <string>--oneshot</string>\n            </array>\n      </dict>\n       <key>com.apple.screenshotr</key>\n       <dict>\n                <key>Label</key>\n              <string>com.apple.screenshotr</string>\n                <key>ProgramArguments</key>\n   <array>\n                        <string>/usr/libexec/ScreenShotr</string>\n                     <string>--lockdown</string>\n                   <string>--oneshot</string>\n    </array>\n       </dict>\n       <key>com.apple.syslog_relay</key>\n     <dict>\n                <key>Label</key>\n              <string>com.apple.syslog_relay</string>\n       <key>ProgramArguments</key>\n            <array>\n                       <string>/usr/libexec/syslog_relay</string>\n                    <string>--lockdown</string>\n           </array>\n       </dict>\n</dict>\n</plist>";
const char *IPhoneComm::roFstab = "/dev/disk0s1 / hfs ro 0 1\n/dev/disk0s2 /private/var hfs rw,noexec 0 2";
const char *IPhoneComm::originalServicesPlist = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<dict>\n <key>com.apple.afc</key>\n      <dict>\n                <key>Label</key>\n              <string>com.apple.afc</string>\n         <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/libexec/afcd</string>\n                    <string>--lockdown</string>\n   </array>\n       </dict>\n       <key>com.apple.crashreportcopy</key>\n  <dict>\n                <key>InstanceLimit</key>\n              <integer>1</integer>\n          <key>Label</key>\n               <string>com.apple.crashreportcopy</string>\n            <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/libexec/CrashReportCopyAgent</string>\n                     <string>--lockdown</string>\n                   <string>--oneshot</string>\n            </array>\n      </dict>\n       <key>com.apple.mobile.debug_image_mount</key>\n  <dict>\n                <key>Label</key>\n              <string>com.apple.mobile.debug_image_mount</string>\n           <key>ProgramArguments</key>\n            <array>\n                       <string>/usr/libexec/debug_image_mount</string>\n               </array>\n      </dict>\n       <key>com.apple.mobile.notification_proxy</key>\n <dict>\n                <key>Label</key>\n              <string>com.apple.mobile.notification_proxy</string>\n          <key>ProgramArguments</key>\n   <array>\n                        <string>/usr/libexec/notification_proxy</string>\n              </array>\n      </dict>\n       <key>com.apple.mobile.software_update</key>\n   <dict>\n         <key>Label</key>\n              <string>com.apple.mobile.software_update</string>\n             <key>ProgramArguments</key>\n           <array>\n               <string>/usr/libexec/software_update</string>\n          </array>\n      </dict>\n       <key>com.apple.mobile.system_profiler</key>\n   <dict>\n                <key>Label</key>\n               <string>com.apple.mobile.system_profiler</string>\n             <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/sbin/system_profiler</string>\n             </array>\n      </dict>\n       <key>com.apple.mobilebackup</key>\n     <dict>\n                <key>InstanceLimit</key>\n              <integer>1</integer>\n           <key>Label</key>\n              <string>com.apple.mobilebackup</string>\n               <key>ProgramArguments</key>\n           <array>\n               <string>/usr/libexec/BackupAgent</string>\n                      <string>--lockdown</string>\n           </array>\n      </dict>\n       <key>com.apple.mobilesync</key>\n       <dict>\n         <key>InstanceLimit</key>\n              <integer>1</integer>\n          <key>Label</key>\n              <string>com.apple.mobilesync</string>\n         <key>ProgramArguments</key>\n            <array>\n                       <string>/usr/libexec/SyncAgent</string>\n                       <string>--lockdown</string>\n                   <string>--oneshot</string>\n                     <string>-v</string>\n           </array>\n      </dict>\n       <key>com.apple.purpletestr</key>\n      <dict>\n                <key>Label</key>\n               <string>com.apple.purpletestr</string>\n                <key>ProgramArguments</key>\n           <array>\n                       <string>/usr/libexec/PurpleTestr</string>\n                      <string>--lockdown</string>\n                   <string>--oneshot</string>\n            </array>\n      </dict>\n       <key>com.apple.screenshotr</key>\n       <dict>\n                <key>Label</key>\n              <string>com.apple.screenshotr</string>\n                <key>ProgramArguments</key>\n   <array>\n                        <string>/usr/libexec/ScreenShotr</string>\n                     <string>--lockdown</string>\n                   <string>--oneshot</string>\n    </array>\n       </dict>\n       <key>com.apple.syslog_relay</key>\n     <dict>\n                <key>Label</key>\n              <string>com.apple.syslog_relay</string>\n       <key>ProgramArguments</key>\n            <array>\n                       <string>/usr/libexec/syslog_relay</string>\n                    <string>--lockdown</string>\n           </array>\n       </dict>\n</dict>\n</plist>";
        
IPhoneComm* IPhoneComm::instance = 0;

IPhoneComm* IPhoneComm::Instance()
{
    if(0 == instance)
    {
        instance = new IPhoneComm();      
    }   
    return instance;
}

IPhoneComm::IPhoneComm() :  iPhone(0),
                            iPhoneRecovery(0),
                            afcConnection(0),
                            state(Idle),
                            connected(false),
                            jailbroken(false),
                            activated(false),
                            notificationCallback(0),
                            VsendCommandToDevice(0),
                            VsendFileToDevice(0),
                            VsocketForPort(0),
                            VperformOperation(0)
    
{
    // 7.3 ish
    patches[0].performRestoreOffset = 0x10009F30;
    
    patches[0].sendCommandOffset = 0x10009290;
    patches[0].sendFileOffset = 0x10009410;
    patches[0].socketForPortOffset = 0x10012830;
    patches[0].performOperationOffset = 0x100129C0;
    
    patches[0].sendCommandCrc = 2855836820UL;
    patches[0].sendFileCrc = 3614846356UL;
    patches[0].socketForPortCrc = 527188315UL;
    patches[0].performOperationsCrc = 1889757874UL;
    
    // 7.4 ish
    patches[1].performRestoreOffset = 0x10008FD0;
    
    patches[1].sendCommandOffset = 0x10008170;
    patches[1].sendFileOffset = 0x100082F0;
    patches[1].socketForPortOffset = 0x10012F90;
    patches[1].performOperationOffset = 0x10014040;
    
    patches[1].sendCommandCrc = 0xD7762D94;
    patches[1].sendFileCrc = 0xB6826CDE;
    patches[1].socketForPortCrc = 0xB9EEE3A6;
    patches[1].performOperationsCrc = 0xDD1428F0;    

}

IPhoneComm::~IPhoneComm()
{

}

bool IPhoneComm::matchFunctionCrc( FILE *f, unsigned long offset, unsigned long crc )
{
    bool match = false;
    unsigned int calcCrc = 0;
    unsigned char buffer[64];
    fseek(f, SEEK_SET, (offset - 0x10000000));
    int read = fread(buffer, 1, 64, f); 
    if(read == 64)
    {
        calcCrc = cyg_crc32_accumulate(calcCrc, buffer, read); 
        
        if(calcCrc == crc)
        {
            match = true;
        }
    }
    return match;
}

bool IPhoneComm::loadItunesDLL()
{
    HMODULE hGetProcIDDLL = GetModuleHandle("iTunesMobileDevice.dll");
    if (!hGetProcIDDLL) 
    {
        cout << "Cannot load local iTunesMobileDevice.dll" << endl; 
        return false;
    }
    
    char dllFile[MAX_PATH];
    memset(dllFile, 0, MAX_PATH);
    GetModuleFileName( hGetProcIDDLL, dllFile, MAX_PATH );

    FILE *dll = fopen(dllFile, "rb");
    if(!dll)
    {
        cout << "Cannot open dll file" << endl;
        return false;
    }

    bool found = false;
    for(int i = 0; i< 2; i++)
    {
        struct iTunesDllPatch patch = patches[i];

        if( matchFunctionCrc( dll, patch.sendCommandOffset, patch.sendCommandCrc ) &&
            matchFunctionCrc( dll, patch.sendFileOffset, patch.sendFileCrc ) &&
            matchFunctionCrc( dll, patch.socketForPortOffset, patch.socketForPortCrc ) &&
            matchFunctionCrc( dll, patch.performOperationOffset, patch.performOperationsCrc ) )
        {
            VsendCommandToDevice=((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore") -patch.performRestoreOffset+patch.sendCommandOffset));
            VsendFileToDevice=((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore") -patch.performRestoreOffset+patch.sendFileOffset));
            VsocketForPort=((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore") -patch.performRestoreOffset+patch.socketForPortOffset));
            VperformOperation=((void *)((char *)GetProcAddress(hGetProcIDDLL, "AMRestorePerformRecoveryModeRestore") -patch.performRestoreOffset+patch.performOperationOffset));                        

            found = true;
            break;
        }
    }
    fclose(dll);
    return found;
}

bool IPhoneComm::initialize()
{
    if(!loadItunesDLL())
    {
        cout << "This version of iTunesMobileDevice.dll is not supported" << endl;
        return false;
    }
    
    am_device_notification *devNotification;
    if( AMDeviceNotificationSubscribe(IPhoneComm::_handleDeviceNotification, 0, 0, 0, &devNotification) ||
        AMRestoreRegisterForDeviceNotifications(NULL, IPhoneComm::_handleRecoveryConnect, NULL, IPhoneComm::_handleRecoveryDisconnect, 0, NULL))
    {
        cout << "Unable to subscribe to notifications" << endl; 
        return false;  
    }

    return true;
}

void IPhoneComm::setNotificationCallback( void (*notificationCallback)(enum IPhoneComm::Notifications notification) )
{
    this->notificationCallback = notificationCallback;
}

void IPhoneComm::sendNotification( enum IPhoneComm::Notifications notification )
{
    if(notificationCallback != 0)
    {
        (*notificationCallback)(notification); 
    }
}

bool IPhoneComm::setRamdiskAndKernelCache( string ramdiskPath, string kernelCachePath )
{
    bool result = false;
    struct stat fileStat;

    if(stat(ramdiskPath.c_str(), &fileStat))
    {
        cout << "Cannot find ramdisk at " << ramdiskPath << endl;
        goto done;
    }
    
    if(stat(kernelCachePath.c_str(), &fileStat))
    {
        cout << "Cannot find ramdisk at " << kernelCachePath << endl;
        goto done;
    }
    
    this->ramdiskPath = ramdiskPath;
    this->kernelCachePath = kernelCachePath;   
    
done:
    return result;
}

void IPhoneComm::resetConnection()
{
    if(afcConnection)
    {
        AFCConnectionClose(afcConnection);
        afcConnection = 0;
    }
    
    if(iPhone)
    {
		AMDeviceDisconnect(iPhone);
		AMDeviceStopSession(iPhone);
        iPhone=0;
    }
    
    iPhoneRecovery = 0;
    connected = false;
    jailbroken = false;
    activated = false;
}

bool IPhoneComm::establishConnection()
{
    bool result = false;
    afc_connection *hAFC;
    CFStringRef activation;
    
	if(AMDeviceConnect(iPhone)) 
	{ 
    	cout << "Could not connect to iPhone" << endl;
        goto done;
    }
    
    if(!AMDeviceIsPaired(iPhone))
    { 
    	cout << "Could not pair with iPhone... Please connect the phone while iTunes is running one time." << endl; 
    	goto done;
    }
    
    if(AMDeviceValidatePairing(iPhone)) 
    { 
    	cout << "Pairing not valid... Please connect the phone while iTunes is running one time." << endl; 
    	goto done;
    }
    
    if(AMDeviceStartSession(iPhone))  
    { 
    	cout << "Unable to start session" << endl; 
    	goto done;
    }
 
 	cout << "Checking for jailbreak" << endl;  
 	

    if ( !AMDeviceStartService(iPhone, CFSTR("com.apple.afc2"), &hAFC, NULL) ) 
    {
        cout << "iPhone is jailbroken" << endl; 
        jailbroken = true;
    }
	else if ( AMDeviceStartService(iPhone, CFSTR("com.apple.afc"), &hAFC, NULL) ) 
    {
		cout << "Unable to start any AFC service" << endl; 
    	goto done;
	} 

	if (AFCConnectionOpen(hAFC, 0, &afcConnection)) 
    {
		cout << "Unable to create AFC connection" << endl; 
    	goto done;
	}
	
    cout << "Checking for activation" << endl; 
    activation = AMDeviceCopyValue(iPhone, 0, CFSTR("ActivationState"));

	if ( activation != NULL &&
        (CFStringCompare(activation, CFSTR("FactoryActivated"), 0) == kCFCompareEqualTo) ||
		(CFStringCompare(activation, CFSTR("Activated"), 0) == kCFCompareEqualTo) ) 
    {
        cout << "iPhone is activated" << endl; 
		activated = true;
	}

	connected = true;
	result = true;
	
done:
    return result;
}

bool IPhoneComm::enterRecovery( bool noRestore )
{
    cout << "Entering recovery mode" << endl;
    if(!noRestore)
    {
        state = EnteringRecovery;
    }
    if (AMDeviceEnterRecovery(iPhone)) 
    {
        cout << "Unable to enter recovery mode" << endl;
        return false;
	}
	return true;
}

void IPhoneComm::exitRecovery()
{
	cout << "Exiting recovery mode" << endl;
	
	sendCommandToDevice(iPhoneRecovery, CFSTR("setenv auto-boot true"));
	sendCommandToDevice(iPhoneRecovery, CFSTR("setenv boot-args"));
	sendCommandToDevice(iPhoneRecovery, CFSTR("saveenv"));
	sendCommandToDevice(iPhoneRecovery, CFSTR("fsboot"));
}

void IPhoneComm::enterRestoreMode()
{
    cout << "Entering restore mode" << endl;
    state = EnteringRestore;
    
    if( iPhoneRecovery == 0)
    {
        cout << "Must be in recovery mode to enter restore" << endl;
        sendNotification(NotifyJailbreakFailed);
        return;   
    }
    
    cout << "Sending ramdisk" << endl; 
	if (sendFileToDevice(iPhoneRecovery, StringtoCFString(ramdiskPath))) 
    {
		cout << "Error sending ramdisk file" << endl;
	}
 
    cout << "Issuing ramdisk command" << endl; 
 	if (sendCommandToDevice(iPhoneRecovery, CFSTR("ramdisk"))) 
    {
		cout << "Error sending ramdisk command" << endl;
	}
	
    cout << "Sending kernelcache" << endl; 
 	if (sendFileToDevice(iPhoneRecovery, StringtoCFString(kernelCachePath))) 
    {
		cout << "Error sending kernel cache file" << endl;
	} 
    
	sendCommandToDevice(iPhoneRecovery, CFSTR("setenv boot-args rd=md0 -progress")); 
	sendCommandToDevice(iPhoneRecovery, CFSTR("setenv auto-boot false"));
	sendCommandToDevice(iPhoneRecovery, CFSTR("setpicture 0"));
	sendCommandToDevice(iPhoneRecovery, CFSTR("bgcolor 0 0 0"));  
	
	cout << "Booting to restore mode" << endl; 
	if (sendCommandToDevice(iPhoneRecovery, CFSTR("bootx"))) 
	{
		cout << "Error booting to restore mode" << endl;
		sendNotification(NotifyJailbreakFailed);
		return;
	}	
}

void IPhoneComm::doRestoreMode()
{
    cout << "Performing restore mode activities" << endl;
    struct am_restore_device *restoreDevice = AMRestoreModeDeviceCreate(0, AMDeviceGetConnectionID(iPhone), 0);
    restoreDevice->port = socketForPort(restoreDevice, 0xf27e);

    Sleep(5000);
 
    cout << "Filesystem check 1" << endl;   
    CFMutableDictionaryRef request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
															   &kCFTypeDictionaryKeyCallBacks,
															   &kCFTypeDictionaryValueCallBacks);

	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("FilesystemCheck"));
	CFDictionarySetValue(request, CFSTR("DeviceName"), CFSTR("/dev/disk0s1"));
	performOperation(restoreDevice, request);

    cout << "Filesystem check 2" << endl;   
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks, 
                                        &kCFTypeDictionaryValueCallBacks);

	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("FilesystemCheck"));
	CFDictionarySetValue(request, CFSTR("DeviceName"), CFSTR("/dev/disk0s2"));
	performOperation(restoreDevice, request); 
  
    cout << "Mount /dev/disk0s1" << endl;     
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Mount"));
	CFDictionarySetValue(request, CFSTR("DeviceName"), CFSTR("/dev/disk0s1"));
	CFDictionarySetValue(request, CFSTR("MountPoint"), CFSTR("/mnt1"));
	performOperation(restoreDevice, request);

    cout << "Mount /dev/disk0s2" << endl;  
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Mount"));
	CFDictionarySetValue(request, CFSTR("DeviceName"), CFSTR("/dev/disk0s2"));
	CFDictionarySetValue(request, CFSTR("MountPoint"), CFSTR("/mnt2"));
	performOperation(restoreDevice, request);     

    cout << "Copy fstab" << endl;          
    request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Ditto"));
	CFDictionarySetValue(request, CFSTR("SourcePath"), CFSTR("/mnt2/root/Media/fstab"));
	CFDictionarySetValue(request, CFSTR("DestinationPath"), CFSTR("/mnt1/etc/fstab"));
	performOperation(restoreDevice, request);

    cout << "Copy Services.plist" << endl;    
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Ditto"));
	CFDictionarySetValue(request, CFSTR("SourcePath"), CFSTR("/mnt2/root/Media/Services.plist"));
	CFDictionarySetValue(request, CFSTR("DestinationPath"), CFSTR("/mnt1/System/Library/Lockdown/Services.plist"));
	performOperation(restoreDevice, request);                      

    if(!applicationPath.empty())
    {
    	transferApplicationFakingExesRecursively(restoreDevice, ("/mnt1/Applications/" + applicationName + "/").c_str(), ("/mnt2/root/Media/" + applicationName + "/").c_str(), applicationPath.c_str());
    
    	cout << "Remove temp application dir" << endl; 
    	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                            &kCFTypeDictionaryKeyCallBacks,
    										&kCFTypeDictionaryValueCallBacks);
    
    	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("RemovePath"));
    	CFDictionarySetValue(request, CFSTR("Path"), CFStringCreateWithCString( kCFAllocatorDefault, ("/mnt2/root/Media/" + applicationName).c_str(), kCFStringEncodingUTF8 ));
    	performOperation(restoreDevice, request);
    }

    cout << "Remove temp fstab" << endl; 
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);

	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("RemovePath"));
	CFDictionarySetValue(request, CFSTR("Path"), CFSTR("/mnt2/root/Media/fstab"));
	performOperation(restoreDevice, request);

    cout << "Remove temp Services.plist" << endl; 
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("RemovePath"));
	CFDictionarySetValue(request, CFSTR("Path"), CFSTR("/mnt2/root/Media/Services.plist"));
	performOperation(restoreDevice, request);

    cout << "Unmount /mnt2" << endl; 
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Unmount"));
	CFDictionarySetValue(request, CFSTR("MountPoint"), CFSTR("/mnt2"));
	performOperation(restoreDevice, request);

    cout << "Unmount /mnt1" << endl; 
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);

	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Unmount"));
	CFDictionarySetValue(request, CFSTR("MountPoint"), CFSTR("/mnt1"));
	performOperation(restoreDevice, request);

    cout << "Exit restore mode" << endl; 
	state = ExitingRestore;
	request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                        &kCFTypeDictionaryKeyCallBacks,
										&kCFTypeDictionaryValueCallBacks);
	
	CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Goodbye"));
	performOperation(restoreDevice, request);
}

bool IPhoneComm::createDirectoryOnDevice(const char *dir)
{
	if (!connected || afcConnection == 0) {
		cout << "Can't create a directory when no phone is connected." << endl;
		return false;
	}

	if (AFCDirectoryCreate(afcConnection, (char*)dir)) 
    {
		cout << "Error creating directory " << dir << endl;
		return false;
	}
	
	cout << "Directory " << dir << " successfully created on phone." << endl;
	return true;
}

bool IPhoneComm::sendFileToDevice( string sourcePath, string destinationPath )
{
    bool result = false;
    unsigned char* data = 0;
    struct stat fileStat;
    fstream sourceFile;
    
    if (stat(sourcePath.c_str(), &fileStat))
    {
        cout << "Unable to stat file " << sourcePath << " to determine length";
        goto done;
    }

    data = new unsigned char[fileStat.st_size];
    
    sourceFile.open(sourcePath.c_str(), ios::in | ios::binary);
    sourceFile.read((char *)data, fileStat.st_size);
    if(!sourceFile)
    {
        cout << "Unable to read from source file " << sourcePath;
        goto done;
    }
    
    result = sendFileToDevice( data, fileStat.st_size, destinationPath );
    
done:
    delete[] data;   
    return result;
}

bool IPhoneComm::sendFileToDevice( const unsigned char* data, int len, string destinationPath )
{
    bool result = false;
    afc_file_ref fileRef;
    
    if(!connected || afcConnection == 0)
    {
    	cout << "Not connected to iPhone" << endl;
    	goto done;        
    }
    
 	if(AFCFileRefOpen(afcConnection, destinationPath.c_str(), 3, 0, &fileRef))
 	{
		cout << "Unable to send open afc connection for remote file " << destinationPath << endl;
		goto done;
	}

	if(AFCFileRefWrite(afcConnection, fileRef, data, len))
    {
		cout << "Unable to send data for file" << endl;
		goto done;
	}

    if(AFCFileRefClose(afcConnection, fileRef))
    {
        cout << "Error closing afc file ref" << endl;   
    }
    
    result = true;
    
done:
    
    return result;  
}

void IPhoneComm::handleDeviceNotification(am_device_notification_callback_info *info)
{
    resetConnection();
    if(info->msg==ADNCI_MSG_CONNECTED) 
    {
        cout << "iPhone connected" << endl;
        iPhone=info->dev;
        
        if( state != EnteringRestore)
        {
            if(establishConnection())
            {
                sendNotification(NotifyConnected);
            }
        }
        
        if( state == EnteringRestore)
        {
            doRestoreMode();   
        }
        else if( state == ExitingRestore )
        {
            doFinalStep();   
        }

    }
    else if(info->msg==ADNCI_MSG_DISCONNECTED)
    {
        cout << "iPhone disconnected" << endl;
        if(state != EnteringRecovery &&
           state != ExitingRestore )
        {
            sendNotification(NotifyDisconnected);
            state = Idle;   
        }
    }
    else
    {
        cout << "iPhone ?: " << info->msg << endl; 
    }
}

void IPhoneComm::handleRecoveryDisconnect(struct am_recovery_device *rdev)
{
    cout << "iPhone disconnected from recovery mode" << endl;
    resetConnection();
    if(state != EnteringRestore &&
       state != ExitingRestore )
    {
        state = Idle;   
    }
}

void IPhoneComm::handleRecoveryConnect(struct am_recovery_device *rdev)
{
    cout << "iPhone connected in recovery mode" << endl;
    resetConnection();
    iPhoneRecovery = rdev;
    
    if( state == EnteringRecovery )
    {
        enterRestoreMode();
    }
    else
    {
        exitRecovery();
    }
}

bool IPhoneComm::deactivate()
{
    cout << "Deactivating" << endl;
    
    if(!connected)
    {
        cout << "Not connected to iPhone" << endl; 
        return false;
    }
    
    if ( AMDeviceDeactivate(iPhone)) 
    {    
        cout << "Deactivate failed" << endl;
        return false;
    }
    
    activated = false;
    
    cout << "Deactivate succeeded" << endl;
    return true;
}

bool IPhoneComm::activate()
{
    bool result = false;
    CFStringRef deviceid;
    CFStringRef imei;
    CFStringRef iccid;
    string deviceid_str;
    string imei_str;
    string iccid_str;
    EVP_PKEY * pKey = 0;
    CFDictionaryRef activation_record;
    
    if(!connected || afcConnection == 0)
    {
        cout << "Not connected to iPhone" << endl; 
        goto done;
    }

    if(!jailbroken)
    {
        result = jailbreak();
        goto done;
    }
 
    cout << "Activating!" << endl;
   
    deviceid = AMDeviceCopyDeviceIdentifier(iPhone);
	if (deviceid == NULL) 
    {
		cout << "Could not retrieve the DeviceID" << endl;
		goto done;
	}
	imei = AMDeviceCopyValue(iPhone, 0, CFSTR("InternationalMobileEquipmentIdentity"));
	if (imei == NULL) {
		cout <<  "Could not retrieve the IMEI" << endl;
		goto done;
	}
	iccid = AMDeviceCopyValue(iPhone, 0, CFSTR("IntegratedCircuitCardIdentity"));
	if (iccid == NULL) {
		cout << "Could not retrieve the ICCID (make sure the SIM is inserted)." << endl;
		goto done;
	}
		
	// Convert to STL strings
	deviceid_str = CFStringtoString( deviceid );
	imei_str = CFStringtoString( imei );
	iccid_str = CFStringtoString( iccid );

	cout << "DeviceID: " << deviceid_str << endl;
	cout << "IMEI: " << imei_str << endl;
	cout << "ICCID: " << iccid_str << endl;
		
	pKey = LoadPEM("resources/iPhoneActivation_private.pem");
    activation_record = GenerateActivationRecord(pKey, deviceid_str, imei_str, iccid_str);
	if (activation_record == NULL) 
	{
		cout << "Could not generate activation record." << endl;
		goto done;
	}

    cout << "Sending public key" << endl; 
    if(!sendFileToDevice( "resources/iPhoneActivation.pem", "/System/Library/Lockdown/iPhoneActivation.pem" ))
    {
        cout << "Error sending public key" << endl;
        goto done;
    }
		
	if(!deactivate()) 
    {    
        goto done;
    }
    
    activated = false;
    
	if ( AMDeviceActivate(iPhone, activation_record)) 
    { 
        cout << "Activation failed" << endl;
        CFRelease(activation_record);
        goto done;
    }  
        
	CFRelease(activation_record);
	activated = true;
    result = true;
    
    cout << "Activation succeeded" << endl;
    sendNotification(NotifyActivateSucceeded);
    
done:
    return result;
}

bool IPhoneComm::installApplication( string applicationPath )
{
    this->applicationPath = applicationPath + "\\";
 
    string::size_type idx = applicationPath.find_last_of("\\");
    if (idx != string::npos)
    {
        this->applicationName = applicationPath.substr(idx+1);
    }
    
    return jailbreak();
}

bool IPhoneComm::returnToJail()
{
    if(!connected || afcConnection == 0)
    {
        cout << "Not connected to iPhone" << endl; 
        return false;  
    }
    
    if(!jailbroken)
    {
        return true;
    }
    
    cout << "Sending fstab" << endl; 
    if(!sendFileToDevice( reinterpret_cast<const unsigned char*>(roFstab), strlen(roFstab), "/etc/fstab" ))
    {
        cout << "Error sending fstab" << endl;
    }
    
    cout << "Sending Services.plist" << endl; 
    if(!sendFileToDevice( reinterpret_cast<const unsigned char*>(originalServicesPlist), strlen(originalServicesPlist), "/System/Library/Lockdown/Services.plist" ))
    {
        cout << "Error sending Services.plist" << endl;
    }

    jailbroken = false;
    
    sendNotification(NotifyReturnToJailSucceeded);
    
    return true;
}

bool IPhoneComm::jailbreak()
{
    if(!connected || afcConnection == 0)
    {
        cout << "Not connected to iPhone" << endl; 
        return false;  
    }
    
    cout << "Jailbreaking..." << endl; 
      
    cout << "Sending temp fstab" << endl; 
    if(!sendFileToDevice( reinterpret_cast<const unsigned char*>(rwFstab), strlen(rwFstab), "fstab" ))
    {
        cout << "Error sending rw fstab" << endl;
        return false;
    }
  
    cout << "Sending temp Services.plist" << endl; 
    if(!sendFileToDevice( reinterpret_cast<const unsigned char*>(modifiedServicesPlist), strlen(modifiedServicesPlist), "Services.plist" ))
    {
        cout << "Error sending modified Services.plist" << endl;
        return false;
    }  
   
    if(!applicationPath.empty())
    {
        cout << "Sending temp application files" << endl;
        this->putDirRecursively((jailbroken ? ("/var/root/Media/" + this->applicationName + "/").c_str() : (this->applicationName + "/").c_str()), this->applicationPath.c_str(), false);		
    }
    
    return enterRecovery( false );

}

void IPhoneComm::doFinalStep()
{
    cout << "Performing final steps" << endl;
 
    if(!applicationPath.empty())
    {
        cout << "Sending libarmfp.dylib" << endl;
        if(!sendFileToDevice( "resources/libarmfp.dylib", "/usr/lib/libarmfp.dylib" ))
        {
            cout << "Error sending libarmfp.dylib" << endl;
        }

        cout << "Sending executables" << endl;	
    	this->putDirRecursively(("/Applications/" + applicationName + "/").c_str(), applicationPath.c_str(), true);
    }
    
    if(!activated)
    {
        activate();
    }
    
    if(!applicationPath.empty())
    {
        cout << "Install complete!" << endl;
        sendNotification(NotifyApplicationInstallSucceeded);
    }
    else
    {
        cout << "Jailbreak complete!" << endl;
        sendNotification(NotifyJailbreakSucceeded);        
    }
	
    state = Idle;
}

void IPhoneComm::putDirRecursively(const char * remoteBasePath, const char *basePath, bool exeOnly)
{
    DIR* dir;			
	char entryPath[256];
	char remotePath[256];
	
    struct dirent* entry;	
    struct stat dir_stat;
	
    /* open the directory for reading */
    dir = opendir(basePath);
    if (!dir) {
		cout << "Cannot read directory " << basePath << endl;
		return;
    }

	// Make sure the dir exists remotely.
	createDirectoryOnDevice(remoteBasePath);

    while ((entry = readdir(dir))) 
	{
        /* check if the given entry is a directory. */
		memset(entryPath, 0, 256);
		strncpy(entryPath, basePath, 255);
		strcat(entryPath, entry->d_name);
		
		memset(remotePath, 0, 256);	
		strncpy(remotePath, remoteBasePath, 255);
		strcat(remotePath, entry->d_name);
		
        if (stat(entryPath, &dir_stat) == -1) 
		{
			cout << "Cannot stat entry " << entry->d_name << endl;
			continue;
        }
		
		/* skip the "." and ".." entries, to avoid loops. */
		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;
			
        if (S_ISDIR(dir_stat.st_mode)) 
		{
			strcat(entryPath,"/");
			strcat(remotePath, "/");
			putDirRecursively(remotePath, entryPath, exeOnly);
	        continue;
		}
		
		bool isExe = false;
		FILE *f = fopen(entryPath, "rb");
		if(!f)
		{
            cout << "File " << entryPath << " doesn't seem to exist" << endl;
            continue; 
        }
        else
        {
            unsigned char magic[4];
            fread(magic, 1, 4, f);
            if( magic[0] == 0xCE &&
                magic[1] == 0xFA &&
                magic[2] == 0xED &&
                magic[3] == 0xFE )
            {
                isExe = true;
            }  
            fclose(f);  
        }
        
		if(!exeOnly || isExe)
		{
			cout << "Writing file " << entryPath << " to " << remotePath << endl;
			if (!sendFileToDevice(entryPath, remotePath))
			{
				cout << "Error writing file " << entryPath << " to phone" << endl;
				return;
			}
		}	
    }
}

void IPhoneComm::transferApplicationFakingExesRecursively( struct am_restore_device *rdev, const char* destBasePath, const char* srcBasePath, const char* localBasePath )
{
	DIR* dir;			
	char localPath[256];
	char srcPath[256];
	char destPath[256];
	
    struct dirent* entry;	/* pointer to one directory entry.   */
    struct stat dir_stat;       /* used by stat().                   */
	
    /* open the directory for reading */
    dir = opendir(localBasePath);
    if (!dir) 
    {
		cout << "Cannot read directory " << localBasePath;
		return;
    }

    while ((entry = readdir(dir))) 
	{
        /* check if the given entry is a directory. */
		memset(localPath, 0, 256);
		strncpy(localPath, localBasePath, 255);
		strcat(localPath, entry->d_name);
		
		memset(srcPath, 0, 256);	
		strncpy(srcPath, srcBasePath, 255);
		strcat(srcPath, entry->d_name);

		memset(destPath, 0, 256);	
		strncpy(destPath, destBasePath, 255);
		strcat(destPath, entry->d_name);
		
        if (stat(localPath, &dir_stat) == -1) 
		{
			cout << "Cannot stat entry " << entry->d_name << endl;
			continue;
        }
		
		/* skip the "." and ".." entries, to avoid loops. */
		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;
			
        if (S_ISDIR(dir_stat.st_mode)) 
		{
			strcat(localPath,"/");
			strcat(destPath, "/");
			strcat(srcPath, "/");
			transferApplicationFakingExesRecursively(rdev, destPath, srcPath, localPath);
	        continue;
		}
		
	    bool isExe = false;
		FILE *f = fopen(localPath, "rb");
		if(!f)
		{
            cout << "File " << localPath << " doesn't seem to exist" << endl;
            continue; 
        }
        else
        {
            unsigned char magic[4];
            fread(magic, 1, 4, f);
            if( magic[0] == 0xCE &&
                magic[1] == 0xFA &&
                magic[2] == 0xED &&
                magic[3] == 0xFE )
            {
                isExe = true;
            }  
            fclose(f);  
        }
        
		const char *source = (isExe ? "/bin/cat" : srcPath);
		cout << "Copying file " << source << " to " << destPath << endl;

		CFMutableDictionaryRef request = CFDictionaryCreateMutable(   kCFAllocatorDefault, 0, 
                                                                    &kCFTypeDictionaryKeyCallBacks,
										                            &kCFTypeDictionaryValueCallBacks);

		CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("Ditto"));
		CFDictionarySetValue(request, CFSTR("SourcePath"), CFStringCreateWithCString( kCFAllocatorDefault, source, kCFStringEncodingUTF8 ));
		CFDictionarySetValue(request, CFSTR("DestinationPath"),  CFStringCreateWithCString( kCFAllocatorDefault, destPath, kCFStringEncodingUTF8 ));
		
		performOperation(rdev, request);

		request = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, 
                                            &kCFTypeDictionaryKeyCallBacks,
                                            &kCFTypeDictionaryValueCallBacks);


		CFDictionarySetValue(request, CFSTR("Operation"), CFSTR("RemovePath"));
		CFDictionarySetValue(request, CFSTR("Path"), CFStringCreateWithCString( kCFAllocatorDefault, srcPath, kCFStringEncodingUTF8 ));
		performOperation(rdev, request);

    }

}

bool IPhoneComm::isConnected()
{
    return connected;   
}

bool IPhoneComm::isJailbroken()
{
    return jailbroken;   
}

bool IPhoneComm::isActivated()
{
    return activated;
}

CFStringRef IPhoneComm::StringtoCFString(string input)
{
   return CFStringCreateWithCString(NULL, input.c_str(), kCFStringEncodingMacRoman);
}

string IPhoneComm::CFStringtoString(CFStringRef input)
{
    char buffer[4096];
	CFStringGetCString(input, buffer, 4096, kCFStringEncodingASCII);
	string deviceid_str(buffer);
	CFRelease(input);
	return deviceid_str;
}

void IPhoneComm::_handleDeviceNotification(am_device_notification_callback_info *info)
{
    instance->handleDeviceNotification(info);
}

void IPhoneComm::_handleRecoveryDisconnect(struct am_recovery_device *rdev)
{
    instance->handleRecoveryDisconnect(rdev);
}

void IPhoneComm::_handleRecoveryConnect(struct am_recovery_device *rdev)
{
    instance->handleRecoveryConnect(rdev);
}

unsigned int IPhoneComm::socketForPort(struct am_restore_device * rdev, unsigned int port) 
{
    int retval = 0;
	asm("push %3\n\t"
        "push %2\n\t"
        "call *%0\n\t"
        "movl %%eax, %1\n\t"
        "addl $0x8, %%esp"
		:
        :"m"(VsocketForPort), "m"(retval), "m"(rdev), "m"(port)
	:); //"%esi"
    return retval;        
}

unsigned int IPhoneComm::performOperation(am_restore_device * rdev, CFDictionaryRef cfdr) 
{
    int retval = 0;
	asm("movl %2, %%esi\n\t"
        "movl %3, %%ebx\n\t"
        "call *%0\n\t"
        "movl %%eax, %1"
		:
        :"m"(VperformOperation), "m"(retval), "m"(rdev), "m"(cfdr)
	:); //"%esi"
    return retval;      
}

unsigned int IPhoneComm::sendFileToDevice(am_recovery_device * rdev, CFStringRef fname)
{
	int retval = 0;
	asm("push %3\n\t"
        "movl %2, %%ecx\n\t"
        "call *%0\n\t"
        "addl $0x4, %%esp\n\t"
        "movl %%eax, %1"
		:
        :"m"(VsendFileToDevice), "m"(retval), "m"(rdev), "m"(fname)
	:); //"%esi"
    return retval;         
}

unsigned int IPhoneComm::sendCommandToDevice(am_recovery_device *rdev, CFStringRef cfs) 
{
	int retval = 0;
	asm("movl %3, %%esi\n\t"
        "push %1\n\t"
        "call *%0\n\t"
        "movl %%eax, %2"
		:
        :"m"(VsendCommandToDevice),  "m"(cfs), "m"(retval), "m"(rdev)
	:); //"%esi"
	return retval;
}

