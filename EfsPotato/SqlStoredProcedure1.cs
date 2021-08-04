using System;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using Microsoft.SqlServer.Server;

public partial class StoredProcedures
{
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static void EfsPotatoCmd(string cmd)
    {
        // 在此处放置代码
        EfsPotato.EfsPotato.orgMain(cmd);

    }
}
