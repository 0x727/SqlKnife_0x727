using Microsoft.SqlServer.Server;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Text;

namespace EfsPotato
{
    class RowUtils
    {
        static public void SendAllRow(ArrayList alltext)
        {

            string[] temparry = (string[])alltext.ToArray(typeof(string));
            SqlDataRecord rec = new SqlDataRecord(new SqlMetaData[] {
                new SqlMetaData("output",SqlDbType.Text,-1)
            });
            SqlContext.Pipe.SendResultsStart(rec);
            for (int i = 0; i < temparry.Length; i++)
            {
                rec.SetSqlString(0, temparry[i]);
                SqlContext.Pipe.SendResultsRow(rec);
            }

            SqlContext.Pipe.SendResultsEnd();

        }
        static public void SendOneRow(string text)
        {

            SqlDataRecord rec = new SqlDataRecord(new SqlMetaData[] {
                new SqlMetaData("output",SqlDbType.Text,-1)
            });
            SqlContext.Pipe.SendResultsStart(rec);
            rec.SetSqlString(0, text);
            SqlContext.Pipe.SendResultsRow(rec);
            SqlContext.Pipe.SendResultsEnd();

        }

    }
}
