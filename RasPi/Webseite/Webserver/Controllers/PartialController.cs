using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Webserver.Data;

namespace Webserver.Controllers
{
    public class PartialController : Controller
    {
        private readonly ILogger<PartialController> Logger;

        public PartialController(ILogger<PartialController> logger)
        {
            Logger = logger;
        }

        public IActionResult Index()
        {
            return PartialView();
        }

        [HttpPost]
        public IActionResult EditSensorName(int? sensorID)
        {
            if (sensorID is null) throw new Exception("ID is null!");

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;

            Data.Types.Sensor sensor = context.GetSensor((int)sensorID);

            return PartialView(sensor);
        }

        [HttpPost]
        public IActionResult EditSensorCard(int? sensorID)
        {
            if (sensorID is null) throw new Exception("ID is null!");

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;

            Data.Types.Sensor sensor = context.GetSensor((int)sensorID);

            Models.Sensor.SensorInfo sensorInfo = context.GetSensorInfos(new[] { sensor }).Single();

            return PartialView(sensorInfo);
        }

        [HttpPost]
        [ValidateAntiForgeryToken]
        public IActionResult UpdateSensor(Data.Types.Sensor sensor)
        {
            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            if (ModelState.IsValid)
            {
                context.UpdateSensor(sensor);
            }

            return RedirectToAction("Index", "Sensor");
        }

        [HttpPost]
        public IActionResult EditEMailRecipient(int? id)
        {
            if (id is null) throw new Exception("ID is null!");

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;

            var model = context.GetEMailRecipient((int)id);

            return PartialView(model);
        }

        [HttpPost]
        [ValidateAntiForgeryToken]
        public IActionResult UpdateEMailRecipient(Data.Types.EMailRecipient model)
        {
            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            if (ModelState.IsValid)
            {
                context.UpdateEMailRecipient(model);
            }

            return RedirectToAction("MailingList", "Settings");
        }

        [HttpPost]
        public IActionResult EditTelegramRecipient(int? id)
        {
            if (id is null) throw new Exception("ID is null!");

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;

            var model = context.GetTelegramRecipient((int)id);

            return PartialView(model);
        }

        [HttpPost]
        [ValidateAntiForgeryToken]
        public IActionResult UpdateTelegramRecipient(Data.Types.TelegramRecipient model)
        {
            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            if (ModelState.IsValid)
            {
                context.UpdateTelegramRecipient(model);
            }

            return RedirectToAction("TelegramList", "Settings");
        }

        [HttpGet]
        public IActionResult LastTriangulationJson()
        {
            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;

            var model = context.GetLastTriangulation();

            return Json(System.Text.Json.JsonSerializer.Serialize(model));
        }
    }
}
