using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Threading.Tasks;

namespace Webserver.Data.Types
{
    public class EMailRecipient
    {
        public int? ID { get; set; }

        [Display(Name = "Adresse")]
        public string Address { get; set; }
    }
}
