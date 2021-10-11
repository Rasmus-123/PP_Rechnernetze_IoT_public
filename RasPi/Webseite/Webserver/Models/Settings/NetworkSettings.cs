using Microsoft.AspNetCore.Mvc;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Threading.Tasks;

namespace Webserver.Models.Settings
{
    public class NetworkSettings
    {
        public string SSID { get; set; }
        [Display(Name = "Passwort")]
        public string Password { get; set; }
    }
}
