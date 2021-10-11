using Microsoft.Extensions.Logging;

namespace Webserver.Lib
{
    /**
     * Singleton containing a LoggerFactory that can be used to Create ILoggers
     * Value set in Startup Constructor
     */
    public class ApplicationLogging
    {
        private static ILoggerFactory _Factory = null;

        public static ILoggerFactory LoggerFactory
        {
            get 
            {
                if (_Factory == null)
                {
                    _Factory = new LoggerFactory();
                }

                return _Factory;
            }

            set
            {
                _Factory = value;
            }
        }
    }
}
