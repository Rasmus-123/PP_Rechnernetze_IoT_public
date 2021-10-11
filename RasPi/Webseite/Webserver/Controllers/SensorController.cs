using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Webserver.Data;
using Webserver.Models.Sensor;

namespace Webserver.Controllers
{
    public class SensorController : Controller
    {
        private readonly ILogger<SensorController> Logger;

        public SensorController(ILogger<SensorController> logger)
        {
            Logger = logger;
        }

        public IActionResult Index()
        {
            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;

            IndexViewModel model = new()
            {
                Sensors = context.GetSensorInfos(context.GetSensors())
            };
            
            return View(model);
        }

        public IActionResult DeleteSensor(int? id)
        {
            if (id is null) throw new Exception("ID is null!");

            Logger.LogInformation("Deleting sensor: " + id);

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;

            context.DeleteSensor((int)id);

            return RedirectToAction("Index");
        }

        public IActionResult Triangulation()
        {
            return View();
        }
    }
}
