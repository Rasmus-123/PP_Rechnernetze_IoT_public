using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Webserver.Data.Types
{
    public class Triangulation
    {
        public int ID { get; set; }
        public DateTime Timestamp { get; set; }
        public int rssi1 { get; set; }
        public int rssi2 { get; set; }
        public int rssi3 { get; set; }

        public string ValueString 
        { 
            get
            {
                return rssi1.ToString() + ", " + rssi2.ToString() + ", " + rssi3.ToString();
            }
        }
    }
}
