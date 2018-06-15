using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.IO;
using System.Linq;
using System.Printing;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace GraderTools
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        App()
        {
            LocalPrintServer localPrintServer = new LocalPrintServer();
            PrintQueue printQueue = localPrintServer.DefaultPrintQueue;

            PrintSystemJobInfo info = printQueue.AddJob("SW Grading");

            StreamReader reader = new StreamReader("C:\\repo\\GraderTools\\App.xaml.cs");
            Byte[] contents = UnicodeEncoding.Unicode.GetBytes(reader.ReadToEnd());
            reader.Close();

            Stream jobStream = info.JobStream;
            jobStream.Write(contents, 0, contents.Length);
            jobStream.Close();
        }
    }
}
