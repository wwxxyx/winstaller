#include <cstdlib>
#include <iostream>
#include <string>
#include <windows.h>
#include <dirent.h>


using namespace std;

void launch(int argc, char *argv[])
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    string commandLine("wiNstallerapp");
    for(int i=1;i<argc;i++)
    {
        commandLine += " ";
        bool hasSpaces = ( strstr(argv[i], " ") ? true : false );
        if(hasSpaces)
        {
            commandLine += "\"";
        }
        commandLine += argv[i];
        if(hasSpaces)
        {
            commandLine += "\"";
        }
    }
    
    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
        const_cast<char *>(commandLine.c_str()),        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        cout << "Unable to start winstaller" << endl;
    }
    else
    {
        WaitForSingleObject( pi.hProcess, INFINITE );
        
        // Close process and thread handles. 
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread ); 
    }
}

int main(int argc, char *argv[])
{
    bool gotDll = false;
    
    FILE* temp = fopen("iTunesMobileDevice.dll", "rb");
    if(temp)
    {
        gotDll = true;
        fclose(temp);
    }
    else
    {
        FILE* dll = fopen("C:\\Program Files\\Common Files\\Apple\\Mobile Device Support\\bin\\iTunesMobileDevice.dll", "rb");
        if(dll)
        {
            char cwdPath[MAX_PATH];
        	memset(cwdPath, 0, MAX_PATH);
        	getcwd(cwdPath, MAX_PATH);
        	string cwd(cwdPath);
            FILE* localDll = fopen((cwd + "/iTunesMobileDevice.dll").c_str(), "wb");
            if( localDll )
            {
                int read;
                unsigned char buffer[16384];
                while((read = fread(buffer, 1, 16384, dll)) > 0)
                {
                    fwrite(buffer, 1, read, localDll);   
                }
                fclose(localDll);
                gotDll = true;
            }
            fclose(dll);
        }
    }
    
    if(!gotDll)
    {
        cout << "Unable to find iTunesMobileDevice.dll.  Please install iTunes" << endl;
    }
    else
    {
        launch( argc, argv );
    }
    
     exit(0);
}
