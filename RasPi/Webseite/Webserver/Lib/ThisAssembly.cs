using System;
using System.Reflection;

/**
* ThisAssembly is a Partial Class
* This is to make it public, as to allow access in .cshtml
* 
* Also Extend with Additional Information
*/
public partial class ThisAssembly
{
    public partial class Git { }

    public const string BuildMode =
                    #if DEBUG
                        "Debug";
                    #else
                        "Release";
                    #endif

    public static DateTime GetBuildTimestamp()
    {
        const int peHeaderOffset = 60;
        const int timestampOffset = 8;
        var bytes = new byte[2048];

        using (var file = new System.IO.FileStream(Assembly.GetExecutingAssembly().Location, System.IO.FileMode.Open, System.IO.FileAccess.Read, System.IO.FileShare.ReadWrite))
        {
            file.Read(bytes, 0, bytes.Length);
        }

        var headerPos = BitConverter.ToInt32(bytes, peHeaderOffset);
        var unixTime = BitConverter.ToInt32(bytes, headerPos + timestampOffset);
        DateTime epoch = new(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
        return epoch.AddSeconds(unixTime);
    }
}