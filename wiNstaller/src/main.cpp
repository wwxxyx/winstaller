#include <windows.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <conio.h>
#include "iPhone.h"

#include <curl.h>
#include <ZipArchive.h>

enum Action
{
    Jailbreak,
    ReturnToJail,
    Activate,
    Reactivate,
    Deactivate,
    Install   
};

IPhoneComm *iPhone;

string cwd;
string ramdisk;
string kernelcache;
string applicationPath;

enum Action action;
bool inProcess;
bool done;

void killItunes()
{
    OSVERSIONINFO OSVersion;
    OSVersion.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
    if(!GetVersionEx(&OSVersion)) return ;

    HANDLE hProcess;
    
    if(OSVersion.dwPlatformId==VER_PLATFORM_WIN32_NT)// NT based OS
    {
        HINSTANCE hiPSAPI = LoadLibrary("PSAPI.DLL");
        if( hiPSAPI == NULL ) return ;
        
        // PSAPI Function Pointers.
        typedef BOOL (WINAPI *LPENUMPROCESSES)(DWORD *,DWORD,DWORD *);
        typedef BOOL (WINAPI *LPENUMPROCESSMODULES)( HANDLE,HMODULE *,DWORD,LPDWORD);
        typedef DWORD (WINAPI *LPGETMODULEFILENAMEEX)( HANDLE,HMODULE,LPTSTR,DWORD);
        
        LPENUMPROCESSES lpfEnumProcesses = (LPENUMPROCESSES)GetProcAddress(hiPSAPI,"EnumProcesses");
        LPENUMPROCESSMODULES lpfEnumProcessModules = (LPENUMPROCESSMODULES)GetProcAddress(hiPSAPI,"EnumProcessModules");
        LPGETMODULEFILENAMEEX lpfGetModuleFileNameEx = (LPGETMODULEFILENAMEEX)GetProcAddress(hiPSAPI,"GetModuleFileNameExA");
        
        if(!lpfEnumProcesses||!lpfEnumProcessModules||!lpfGetModuleFileNameEx)
        {
                //problem loading pointers to functions
            FreeLibrary(hiPSAPI);
            return;
        }
        
        //TODO: add loop to increase to max dwNeeded, set at 400 for test
        DWORD ProcessIDList[400],dwCb=400,dwCbNeeded,cbRet;
        
        if(!lpfEnumProcesses(ProcessIDList,dwCb,&dwCbNeeded))
        {
                //enumprocesses failed
            FreeLibrary(hiPSAPI);
            return;
        }

        HMODULE hModule[50];
        DWORD i=0;
        char ModuleFileName[256];

        while(i<dwCbNeeded/sizeof(DWORD))
        {
            hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ|PROCESS_TERMINATE,0,ProcessIDList[i++]);
            if(hProcess)
            {
                if(lpfEnumProcessModules(hProcess,hModule,50,&cbRet))
                {
                    if(lpfGetModuleFileNameEx(hProcess,hModule[0],ModuleFileName,256))
                    {
                        if(strstr(ModuleFileName,"iTunes"))
                        {
                            cout << "Terminating " << ModuleFileName << endl;
                            TerminateProcess(hProcess,0);
                        }
                    }
                }
                }
                cbRet=GetLastError();
            CloseHandle(hProcess);
        }
        FreeLibrary(hiPSAPI);
    }
    else if(OSVersion.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)// Win 9x 
    {
        HINSTANCE hiKernel32 = LoadLibrary("Kernel32.DLL");
        if(hiKernel32 == NULL) return;
        
        // ToolHelp32 Function Pointers.
        typedef HANDLE (WINAPI *LPCREATETOOLHELP32SNAPSHOT)(DWORD,DWORD);
        typedef BOOL (WINAPI *LPPROCESS32FIRST)(HANDLE,LPPROCESSENTRY32);
        typedef BOOL (WINAPI *LPPROCESS32NEXT)(HANDLE,LPPROCESSENTRY32);
        
        LPCREATETOOLHELP32SNAPSHOT lpfCreateToolhelp32Snapshot = (LPCREATETOOLHELP32SNAPSHOT)GetProcAddress(hiKernel32,"CreateToolhelp32Snapshot");
        LPPROCESS32FIRST lpfProcess32First = (LPPROCESS32FIRST)GetProcAddress(hiKernel32,"Process32First");
        LPPROCESS32NEXT lpfProcess32Next = (LPPROCESS32NEXT)GetProcAddress(hiKernel32,"Process32Next");
        
        if(!lpfCreateToolhelp32Snapshot||!lpfProcess32First||!lpfProcess32Next)
        {
            FreeLibrary(hiKernel32);
            return;
        }
        
        HANDLE hProcessList = lpfCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
        if(!hProcessList)
        {
            FreeLibrary(hiKernel32);
            return;    // cannot take the snapshot
        }
        
        PROCESSENTRY32 Process;
        Process.dwSize = sizeof(PROCESSENTRY32);
        if(!lpfProcess32First(hProcessList,&Process))
        {
            CloseHandle(hProcessList);
            FreeLibrary(hiKernel32);
            return;
        }
        do
        {
            if(strstr(Process.szExeFile,"iTunes"))
            {
                cout << "Killing " << Process.szExeFile << endl;
                hProcess = OpenProcess(PROCESS_TERMINATE,0,Process.th32ProcessID);
                if(!hProcess) break;// cannot open the process                
                TerminateProcess(hProcess,0);
                CloseHandle(hProcess);
            }
        }
        while (lpfProcess32Next(hProcessList,&Process));
        
        CloseHandle(hProcessList);
        FreeLibrary(hiKernel32);
    }   
}

void extractRestoreFiles( string filepath, bool known )
{
    cout << "Extracting restore files" << endl;
    CZipArchive zip;
    ZIP_INDEX_TYPE index;
    
    try
    {
        zip.Open(_T(filepath.c_str()));
    
        cout << "Extracting ramdisk" << endl;
        if( known )
        {
            index = zip.FindFile(_T("694-5259-38.dmg"));
            zip.ExtractFile(index, _T(cwd.c_str()));
            ramdisk = (cwd + "/694-5259-38.dmg");
        }
        else
        {
            CZipIndexesArray indexes;
            zip.FindMatches(_T("*.dmg"), indexes, false);	 
            for (ZIP_ARRAY_SIZE_TYPE i = 0; i < indexes.GetCount(); i++)
            {
                ZIP_INDEX_TYPE ind = indexes[i];        
                if(zip[ind]->m_uUncomprSize > (10*1024*1024) && zip[ind]->m_uUncomprSize < (20*1024*1024) )
                {
                    zip.ExtractFile(ind, _T(cwd.c_str()));   
                    FILE *f = fopen((cwd + "/" + zip[ind]->GetFileName()).c_str(), "rb");
            		if(!f)
            		{
                        continue; 
                    }
                    else
                    {
                        unsigned char magic[4];
                        fread(magic, 1, 4, f);
                        if( magic[0] == 0x38 &&
                            magic[1] == 0x39 &&
                            magic[2] == 0x30 &&
                            magic[3] == 0x30 )
                        {
                            ramdisk = cwd + "/" + zip[ind]->GetFileName();
                            fclose(f);
                            break;
                        }  
                        fclose(f);  
                    }            
                }
            }
        }			
        
        cout << "Extracting kernelcache" << endl; 
        if(known)
        {  
            index = zip.FindFile(_T("kernelcache.restore.release.s5l8900xrb"));
            zip.ExtractFile(index, _T(cwd.c_str()));
            kernelcache = (cwd + "/kernelcache.restore.release.s5l8900xrb");
        }
        else
        {
            CZipIndexesArray indexes;
            zip.FindMatches (_T("*kernelcache*"), indexes, false);	
            if(indexes.GetCount() > 0)
            { 
                zip.ExtractFile(indexes[0], _T(cwd.c_str())); 
                kernelcache = cwd + "/" + zip[indexes[0]]->GetFileName();
            }
        }
        zip.Close();
    }
    catch(...)
    {
        cout << "Error extracting restore file" << endl;   
    }

}

void downloadRestoreArchive()
{
    CURL * curl = curl_easy_init(); 
    if(!curl)
    {
        cout << "Unable to initialize curl" << endl;    
        return;
    }
    
    FILE * target = fopen((cwd + "/iPhone1,1_1.0_1A543a_Restore.ipsw").c_str(), "wb");
    if(!target)
    {
        cout << "Unable to create file for download" << endl;
        return;
    }
      
    cout << "Downloading restore files from apple" << endl;
     
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &fwrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, target);
    curl_easy_setopt(curl, CURLOPT_URL, "http://appldnld.apple.com.edgesuite.net/content.info.apple.com/iPhone/061-3538.20070629.B7vXa/iPhone1,1_1.0_1A543a_Restore.ipsw");
    CURLcode res = curl_easy_perform( curl );
    curl_easy_cleanup(curl);
    
    fclose(target);
    if(res)
    {
        cout << "Restore download failed" << endl;   
    }
    else
    {
        cout << "Restore download succeeded" << endl;
        extractRestoreFiles(cwd + "/iPhone1,1_1.0_1A543a_Restore.ipsw", true);
    }
}


bool findLocalRestoreFiles()
{
    DIR* dir;			
    struct dirent* entry;	
    struct stat dir_stat;
	string restoreFile;
	
    dir = opendir(cwd.c_str());
    if (dir) 
    {
        while ((entry = readdir(dir))) 
        {
            if (stat((cwd + "/" +  entry->d_name).c_str(), &dir_stat) == -1) 
    		{
    			continue;
            }
		    
		    if (S_ISDIR(dir_stat.st_mode)) 
		    {
                continue;
            }
            
            if(strstr(entry->d_name, ".ipsw") && strstr(entry->d_name, "iPhone") && !strstr(entry->d_name, "Update"))
            {
                restoreFile = cwd + "/" + entry->d_name;
            }
            
            if(ramdisk.empty())
            {
                if(strstr(entry->d_name, ".dmg"))
                {
                    FILE *f = fopen(entry->d_name, "rb");
            		if(!f)
            		{
                        cout << "File " << entry->d_name << " doesn't seem to exist" << endl;
                        continue; 
                    }
                    else
                    {
                        unsigned char magic[4];
                        fread(magic, 1, 4, f);
                        if( magic[0] == 0x38 &&
                            magic[1] == 0x39 &&
                            magic[2] == 0x30 &&
                            magic[3] == 0x30 )
                        {
                            ramdisk = entry->d_name;
                        }  
                        fclose(f);  
                    }
                }
            }
            
            if(kernelcache.empty())
            {
                if(strstr(entry->d_name, "kernelcache"))
                {
                    kernelcache = entry->d_name;
                }
            }
        }
    }
    
    if((ramdisk.empty() || kernelcache.empty()) && !restoreFile.empty())
    {
        extractRestoreFiles( restoreFile, false );
    }
    
    return (!ramdisk.empty() && !kernelcache.empty());
}

bool findItunesRestoreFiles()
{		
    TCHAR szPath[MAX_PATH];
    if(SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, szPath))
    {
        cout << "Could not find application data folder" << endl;
        return false;   
    }
        
    string iTunesUpdateFolder(szPath);
    iTunesUpdateFolder += "/Apple Computer/iTunes/iPhone Software Updates";
    
    DIR* dir;	
    struct dirent* entry;	
    struct stat dir_stat;
	
    dir = opendir(iTunesUpdateFolder.c_str());
    if (dir) 
    {
        while ((entry = readdir(dir))) 
        {
            if (stat((iTunesUpdateFolder + "/" +  entry->d_name).c_str(), &dir_stat) == -1) 
    		{
    			continue;
            }
		    
		    if (S_ISDIR(dir_stat.st_mode)) 
		    {
                continue;
            }
            
            if(strstr(entry->d_name, ".ipsw") && strstr(entry->d_name, "iPhone") && !strstr(entry->d_name, "Update"))
            {
                extractRestoreFiles( iTunesUpdateFolder + "/" +  entry->d_name, false );
                break;
            }
        }
    }
    
    return (!ramdisk.empty() && !kernelcache.empty());
}

void notificationCallback( enum IPhoneComm::Notifications notification)
{
    switch(notification)
    {
        case IPhoneComm::NotifyConnected:
        {
            cout << "Notified of connection" << endl;
            if(!inProcess)
            {
                inProcess = true;
                if(action == Install)
                {
                    iPhone->installApplication(applicationPath);
                }
                else if(action == Activate)
                {
                    if(iPhone->isActivated())
                    {
                        cout << "Phone is already activated" << endl;
                        done = true;                       
                    }
                    else if(!iPhone->activate())
                    { 
                        cout << "Activate failed" << endl;
                        done = true;
                    }
                }
                else if(action == Reactivate)
                {
                    if(!iPhone->activate())
                    {
                        cout << "Activate failed" << endl;
                        done = true;
                    }
                }
                else if(action == Deactivate)
                {
                    if(!iPhone->isActivated())
                    {
                        cout << "Phone is already deactivated" << endl;
                        done = true;
                    }

                    iPhone->deactivate();
                    done = true;
                }
                else if(action == Jailbreak)
                {
                    if(iPhone->isJailbroken())
                    {
                        cout << "Phone is already jailbroken" << endl;
                        done = true;
                    }
                    else if(!iPhone->jailbreak())
                    {
                        cout << "Jailbreak failed" << endl;
                        done = true;
                    } 
                }
                else if(action == ReturnToJail)
                {
                    if(!iPhone->isJailbroken())
                    {
                        cout << "Phone is already in jail" << endl;
                        done = true;
                    }
                    else if(!iPhone->returnToJail())
                    {
                        cout << "Return to jail failed" << endl;
                        done = true;
                    } 
                }
                else
                {
                    done = true;
                }
            }
            break;
        }  
        case IPhoneComm::NotifyJailbreakFailed:
        case IPhoneComm::NotifyReturnToJailFailed:
        case IPhoneComm::NotifyActivateFailed:
        case IPhoneComm::NotifyApplicationInstallFailed:
        {
            done = true;
            break;
        }
        case IPhoneComm::NotifyReturnToJailSucceeded:
        {
            if(action == ReturnToJail)
            {
                done = true;
            }
            break;
        }
        case IPhoneComm::NotifyJailbreakSucceeded:
        {
            if(action == Jailbreak ||
               action == Activate || 
               action == Reactivate )
            {
                done = true;
            }
            break;
        }
        case IPhoneComm::NotifyActivateSucceeded:
        {
            if(action == Activate || action == Reactivate)
            {
                done = true;
            }
            break;
        }
        case IPhoneComm::NotifyApplicationInstallSucceeded:
        {
            if(action == Install)
            {
                done = true;
            }
            break;
        }
        default:
        {
            break;    
        }
    }
}

void usage()
{
    cout << "Usage: wiNstaller activate" << endl;
    cout << "       wiNstaller reactivate" << endl;
    cout << "       wiNstaller deactivate" << endl;
    cout << "       wiNstaller jailbreak" << endl;
    cout << "       wiNstaller returntojail" << endl;
    cout << "       wiNstaller install <path to app>" << endl;
    exit(0); 
}

int main(int argc, char * * argv)
{
    if(argc < 2)
    {
        usage();
    }
    
    if(!stricmp(argv[1], "activate"))
    {
        action = Activate;
    }
    else if(!stricmp(argv[1], "reactivate"))
    {
        action = Reactivate;
    }
    else if(!stricmp(argv[1], "deactivate"))
    {
        action = Deactivate;
    }
    else if(!stricmp(argv[1], "jailbreak"))
    {
        action = Jailbreak;
    }
    else if(!stricmp(argv[1], "returntojail"))
    {
        action = ReturnToJail;
    }
    else if(!stricmp(argv[1], "install"))
    {
        if(argc < 3)
        {
            usage();
        }
        
        action = Install;
        
        string app(argv[2]);
        if(app.find_last_of('/') == (app.size() - 1) ||
            app.find_last_of('\\') == (app.size() - 1))
        {
            app = app.substr( 0, app.size() - 1);
        }
    
        FILE * file = fopen( (app + "/" + "Info.plist").c_str(), "rb" );
        if(!file)
        {
            cout << "The directory specified does not seem to be an iPhone app!" << endl;
            exit(0);   
        }
        else
        {
            fclose(file);   
        }
        
        file = fopen( (app + "/" + "icon.png").c_str(), "rb" );
        if(!file)
        {
            cout << "The directory specified does not seem to be an iPhone app!" << endl;
            exit(0);   
        }
    	else
    	{
            fclose(file);
        }  
        
        applicationPath = app;      
    }
    else
    {
        usage();   
    }
   
	char cwdPath[MAX_PATH];
	memset(cwdPath, 0, MAX_PATH);
	getcwd(cwdPath, MAX_PATH);
	cwd = string(cwdPath);

    cout << "Looking for restore files" << endl;
    if(!findLocalRestoreFiles() && !findItunesRestoreFiles())
    {
        curl_global_init(CURL_GLOBAL_ALL);
        downloadRestoreArchive();
    }

    if(ramdisk.empty() || kernelcache.empty())
    {
        cout << "Could not obtain any restore files!" << endl;   
        exit(0);
    }

    cout << "Ramdisk: " << ramdisk << endl;
    cout << "Kernelcache: " << kernelcache << endl;
    
    killItunes();
    
    done = false;   
    inProcess = false;
    iPhone = IPhoneComm::Instance();
    iPhone->setRamdiskAndKernelCache(ramdisk,kernelcache);
    iPhone->setNotificationCallback(&notificationCallback);
    
    if(!iPhone->initialize())
    {
        cout << "Error initializing communications with iPhone." << endl;
        exit(0);   
    }
   
    if(!iPhone->isConnected())
    {
        cout << "Please connect the iPhone" << endl;
    }
    

    while(!done)
    {
        Sleep(500);   
    }
    
    cout << "Press any key to exit..." << endl;
    getch();
	return 0;
}
