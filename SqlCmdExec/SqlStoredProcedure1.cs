using Microsoft.SqlServer.Server;
using System;
using System.Data;
using System.Diagnostics;
using System.Text;

public partial class StoredProcedures
{
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static void SqlCmdExec(String cmd)
    {
        //可自定义执行命令程序
        //public static void SqlCmdExec(string filename, string cmd)
        {
            // 在此处放置代码
            //if (!string.IsNullOrEmpty(cmd)&&!string.IsNullOrEmpty(filename))
            if (!string.IsNullOrEmpty(cmd))
            {
                //不使用pipe，也能获取程序标准输出，但是ADO取不到
                //SqlContext.Pipe.Send(RunCommand("cmd.exe", "/c " + cmd));
                //RunCommand("cmd.exe","/c "+cmd);

                //需要输出标准结果集才能在ADO中获取到
                string cmdres = RunCommand("sqlps.exe", cmd);
                SqlDataRecord rec = new SqlDataRecord(new SqlMetaData[] {
                new SqlMetaData("output",SqlDbType.Text,-1)
            });
                SqlContext.Pipe.SendResultsStart(rec);
                rec.SetSqlString(0, cmdres);
                SqlContext.Pipe.SendResultsRow(rec);
                SqlContext.Pipe.SendResultsEnd();


            }
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
            //SqlContext.Pipe.Send(stdOutput.ToString());
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
