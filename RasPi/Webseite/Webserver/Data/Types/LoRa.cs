﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Webserver.Data.Types
{
    public class LoRa
    {
        public int ID { get; set; }
        public int Sensor { get; set; }
        public DateTime Timestamp { get; set; }
        public int Value { get; set; }
    }
}
