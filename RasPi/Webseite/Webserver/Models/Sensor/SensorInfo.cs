using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Webserver.Models.Sensor
{
    public class SensorInfo
    {
        public Data.Types.Sensor Sensor { get; set; }

        public Data.Types.Ping LastPing { get; set; }

        public Data.Types.BarrierInterrupt LastBarrierInterrupt { get; set; }

        public Data.Types.LoRa LastLoRa { get; set; }

        public string LastActivityTimestamp
        {
            get
            {
                if (LastBarrierInterrupt is not null) 
                    return LastBarrierInterrupt.Timestamp.ToString();

                if (LastLoRa is not null)
                    return LastLoRa.Timestamp.ToString();


                return "";
            }
        }

        public string LastActivityValue
        {
            get
            {
                if (LastBarrierInterrupt is not null)
                    return "Schrankenunterbrechung: " + LastBarrierInterrupt.Value.ToString();

                if (LastLoRa is not null)
                    return "LoRa: " + LastLoRa.Value.ToString();


                return "";
            }
        }


        public bool Active 
        {
            get
            {
                if (LastPing is null) return false;

                if (LastPing.Timestamp < DateTime.Now.AddMinutes(-5.0d)) return false;

                if (LastPing.Status != "ok") return false;

                return true;
            }
        }
    }
}
