using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;

namespace wiNstallerGUI
{
    public partial class Form1 : Form
    {
        delegate void SetTextCallback(string text);
        delegate void ApplicationExitedCallback();

        private Process winstallerProc = null;

        public Form1()
        {
            InitializeComponent();
        }

        private void installButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            fbd.Description = "Choose an iPhone application (the .app directory)";
            fbd.ShowNewFolderButton = false;

            DialogResult result = fbd.ShowDialog();
            if (result == DialogResult.OK)
            {
                launchWinstaller( "install \"" + fbd.SelectedPath + "\"");
            }

        }

        private void reactivateButton_Click(object sender, EventArgs e)
        {
            launchWinstaller( "reactivate");
        }


        private void jailbreakButton_Click(object sender, EventArgs e)
        {
            launchWinstaller("jailbreak");
        }

        private void activateButton_Click(object sender, EventArgs e)
        {
            launchWinstaller("activate");
        }

        private void deactivateButton_Click(object sender, EventArgs e)
        {
            launchWinstaller("deactivate");
        }

        private void returntojailButton_Click(object sender, EventArgs e)
        {
            launchWinstaller("returntojail");
        }

        private void setRunningUI()
        {
            installButton.Enabled = false;
            activateButton.Enabled = false;
            deactivateButton.Enabled = false;
            reactivateButton.Enabled = false;
            jailbreakButton.Enabled = false;
            returntojailButton.Enabled = false;

            cancelButton.Enabled = true;
        }

        private void setIdleUI()
        {
            installButton.Enabled = true;
            activateButton.Enabled = true;
            deactivateButton.Enabled = true;
            reactivateButton.Enabled = true;
            jailbreakButton.Enabled = true;
            returntojailButton.Enabled = true;

            cancelButton.Enabled = false;
        }

        static bool FindiTunesDll()
        {
            string cwd = Directory.GetCurrentDirectory().ToString();

            FileInfo dll = new FileInfo(cwd + "\\iTunesMobileDevice.dll");
            if (dll.Exists)
            {
                return true;
            }

            dll = new FileInfo("C:\\Program Files\\Common Files\\Apple\\Mobile Device Support\\bin\\iTunesMobileDevice.dll");
            dll = dll.CopyTo(cwd + "\\iTunesMobileDevice.dll");
            if (dll.Exists)
            {
                return true;
            }

            return false;
        }

        private void launchWinstaller( string args )
        {
            if (!FindiTunesDll())
            {
                MessageBox.Show("Cannot find iTunesMobileDevice.dll.  Is iTunes installed?");
                return;
            }

            consoleOutput.Text = "";
            string path = Directory.GetCurrentDirectory().ToString() + "\\wiNstallerapp.exe";

            setRunningUI();

            winstallerProc = new Process();
            winstallerProc.EnableRaisingEvents = true;
            winstallerProc.StartInfo.FileName = path;
            winstallerProc.StartInfo.Arguments = args;
            winstallerProc.StartInfo.ErrorDialog = false;
            winstallerProc.StartInfo.RedirectStandardOutput = true;
            winstallerProc.StartInfo.RedirectStandardError = true;
            winstallerProc.StartInfo.RedirectStandardInput = true;
            winstallerProc.StartInfo.UseShellExecute = false;
            winstallerProc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            winstallerProc.StartInfo.CreateNoWindow = true;
            winstallerProc.OutputDataReceived += new DataReceivedEventHandler(consoleDataReceived);
            winstallerProc.ErrorDataReceived += new DataReceivedEventHandler(consoleDataReceived);
            winstallerProc.Exited += new EventHandler(winstallerProc_Exited);
            if (!winstallerProc.Start())
            {
                setIdleUI();
            }
            else
            {
                winstallerProc.BeginErrorReadLine();
                winstallerProc.BeginOutputReadLine();
            }
        }

        void winstallerProc_Exited(object sender, EventArgs e)
        {
            ApplicationExited();
        }

        private void ApplicationExited()
        {
            if (this.consoleOutput.InvokeRequired)
            {
                ApplicationExitedCallback d = new ApplicationExitedCallback(ApplicationExited);
                this.Invoke(d, null);
            }
            else
            {
                winstallerProc = null;
                setIdleUI();
            }
        }

        private void AppendConsoleText( string text )
        {
            if (this.consoleOutput.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(AppendConsoleText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                consoleOutput.AppendText(text);

            }
        }


        void consoleDataReceived(object sender, DataReceivedEventArgs e)
        {
            if (e.Data != null)
            {
                if (e.Data.Contains("Press any key"))
                {
                    stopProcess();
                }
                else
                {
                    AppendConsoleText(e.Data + "\r\n");
                }
            }
        }

        private void stopProcess()
        {
            if (winstallerProc != null)
            {
                winstallerProc.CancelErrorRead();
                winstallerProc.CancelOutputRead();
                winstallerProc.Kill();
                winstallerProc = null;
            }
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            stopProcess();
        }

    }
}