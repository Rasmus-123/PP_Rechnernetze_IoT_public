using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace Webserver.Models.Sensor
{
    public class IndexViewModel
    {
        public IEnumerable<SensorInfo> Sensors { get; set; }
    }
}
