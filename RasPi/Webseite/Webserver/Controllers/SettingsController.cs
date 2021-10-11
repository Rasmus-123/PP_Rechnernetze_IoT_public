using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Webserver.Data;

/*
 * Some scripts require sudo privileges
 * 
 * sudo visudo
 *      ALL ALL=NOPASSWD: /path/to/script.sh
 *
 * Then it can be sudo executed by everyone
 */



namespace Webserver.Controllers
{
    public class SettingsController : Controller
    {
        private readonly ILogger<SettingsController> Logger;

        public SettingsController(ILogger<SettingsController> logger)
        {
            Logger = logger;
        }

        private string GetCurrentSSID()
        {
            System.Diagnostics.Process process = new()
            {
                StartInfo = new()
                {
                    FileName = "/bin/bash",
                    RedirectStandardInput = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    Arguments = "-c \"sudo ~/scripts/getSSID.sh\"",
                    CreateNoWindow = true
                }
            };

            process.Start();

            process.WaitForExit();

            string ssid = process.StandardOutput.ReadToEnd();

            return ssid;
        }

        private string GetCurrentPSK()
        {
            System.Diagnostics.Process process = new()
            {
                StartInfo = new()
                {
                    FileName = "/bin/bash",
                    RedirectStandardInput = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    Arguments = "-c \"sudo ~/scripts/getPSK.sh\"",
                    CreateNoWindow = true
                }
            };

            process.Start();

            process.WaitForExit();
            string psk = process.StandardOutput.ReadToEnd();

            return psk;
        }

        public IActionResult Index()
        {
            Models.Settings.Settings model;
            if (Environment.OSVersion.Platform == PlatformID.Unix)
            {
                model = new()
                {
                    NetworkSettings = new()
                    {
                        SSID = GetCurrentSSID(),
                        Password = GetCurrentPSK()
                    }
                };
            
            } 
            else 
            {
                model = new()
                {
                    NetworkSettings = new()
                    {
                        SSID = "Unsupported",
                        Password = "On non-Unix Systems!"
                    }
                };
            }
            return View(model);
        }

        [HttpPost]
        [ValidateAntiForgeryToken]
        public IActionResult ChangeWiFiNetwork(Models.Settings.NetworkSettings model)
        {
            // Ich versteh nicht, warum das Model Binding hier nicht funktioniert.
            // Daher erstmal Manuell die Daten hier rausziehen
            Logger.LogInformation("SSID:", model.SSID);
            Logger.LogInformation(model.SSID);
            Logger.LogInformation("PASS:", model.Password);
            Logger.LogInformation(model.Password);

            if (!(Request.Form.ContainsKey("NetworkSettings.SSID") && Request.Form.ContainsKey("NetworkSettings.Password")))
            {
                throw new Exception("SSID or Password missing!");
            }

            Models.Settings.NetworkSettings settings = new()
            {
                SSID = Request.Form["NetworkSettings.SSID"],
                Password = Request.Form["NetworkSettings.Password"]
            };

            Logger.LogInformation("SSID: " + settings.SSID);
            Logger.LogInformation("PASS: " + settings.Password);

            // No " or \n should be enough to not break the wpa_supplicant.conf file
            if (System.Text.RegularExpressions.Regex.IsMatch(settings.SSID, "[\"\n]") || System.Text.RegularExpressions.Regex.IsMatch(settings.Password, "[\"\n]"))
            {
                throw new ArgumentException("No \" or \\n allowed in SSID or Password!");
            }

            if (Environment.OSVersion.Platform == PlatformID.Unix && ModelState.IsValid)
            {
                System.Diagnostics.Process process = new()
                {
                    StartInfo = new()
                    {
                        FileName = "bash",
                        RedirectStandardInput = true,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        UseShellExecute = false
                    }
                };

                process.Start();
                process.StandardInput.WriteLine($"sudo ~/scripts/change_wifi_network.sh -s \"{settings.SSID}\" -p \"{settings.Password}\"");
                process.StandardInput.WriteLine("exit");
                process.WaitForExit();
            }
            else
            {
                throw new Exception("Unsupported on Non-Unix Systems!");
            }

            return RedirectToAction("Index");
        }

        public IActionResult Reboot()
        {
            Logger.LogInformation("[" + DateTime.Now.ToString() +  "] Rebooting!");
            if (Environment.OSVersion.Platform == PlatformID.Unix)
            {
                System.Diagnostics.Process process = new()
                {
                    StartInfo = new()
                    {
                        FileName = "/bin/bash",
                        RedirectStandardInput = true,
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        UseShellExecute = false,
                        Arguments = "-c \"sudo ~/scripts/reboot.sh\"",
                        CreateNoWindow = true
                    }
                };

                process.Start();
                process.WaitForExit();
            }

            return NoContent();
        }


        public IActionResult MailingList()
        {
            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            IEnumerable<Data.Types.EMailRecipient> model = context.GetEMailRecipients();
            return View(model);
        }

        public IActionResult DeleteEMailRecipient(int? id)
        {
            if (id is null) throw new ArgumentNullException();

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            context.DeleteEMailRecipient((int)id);

            return RedirectToAction("MailingList");
        }
        
        public IActionResult AddEMailRecipient([FromQuery]string address)
        {
            if (address is null) throw new ArgumentNullException();

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            context.AddEMailRecipient(address);

            return RedirectToAction("MailingList");
        }


        public IActionResult TelegramList()
        {
            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            IEnumerable<Data.Types.TelegramRecipient> model = context.GetTelegramRecipients();
            return View(model);
        }

        public IActionResult DeleteTelegramRecipient(int? id)
        {
            if (id is null) throw new ArgumentNullException();

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            context.DeleteTelegramRecipient((int)id);

            return RedirectToAction("TelegramList");
        }

        public IActionResult AddTelegramRecipient([FromQuery] string address)
        {
            if (address is null) throw new ArgumentNullException();

            SensorDBContext context = HttpContext.RequestServices.GetService(typeof(SensorDBContext)) as SensorDBContext;
            context.AddTelegramRecipient(address);

            return RedirectToAction("TelegramList");
        }
    }
}
