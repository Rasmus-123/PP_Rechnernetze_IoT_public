using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using Webserver.Lib;

using SqlKata.Execution;
using MySql.Data.MySqlClient;

namespace Webserver.Data
{
    public class SensorDBContext
    {
        public string ConnectionString { get; set; }
        private readonly ILogger Logger;

        public SensorDBContext(string connectionstring)
        {
            this.ConnectionString = connectionstring;
            this.Logger = ApplicationLogging.LoggerFactory.CreateLogger<SensorDBContext>();
        }

        public Types.Sensor GetSensor(int id)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            var query = sensorDB
                .Query("sensoren")
                .Select("*")
                .Where("ID", id);

            return query.FirstOrDefault<Types.Sensor>();
        }

        public IEnumerable<Types.Sensor> GetSensors(Object? filter = null, Action<SqlKata.Query> lambda = null)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            var query = sensorDB
                .Query("sensoren")
                .Select("*");

            if (lambda is not null)
                lambda(query);

            return query.Get<Types.Sensor>();
        }

        public Types.Ping GetPing(int id)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            var query = sensorDB
                .Query("pings")
                .Select("*")
                .Where("ID", id);

            return query.FirstOrDefault<Types.Ping>();
        }

        public Types.Ping GetLastPing(Types.Sensor sensor)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            var query = sensorDB
                .Query("pings")
                .Select("*")
                .Where("Sensor", sensor.ID)
                .OrderByDesc("ID");

            return query.FirstOrDefault<Types.Ping>();
        }

        public Types.BarrierInterrupt GetLastBarrierInterrupt(Types.Sensor sensor)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            var query = sensorDB
                .Query("schranken-unterbrechungen")
                .Select("*")
                .Where("Sensor", sensor.ID)
                .OrderByDesc("ID");

            return query.FirstOrDefault<Types.BarrierInterrupt>();
        }

        public Types.LoRa GetLastLoRa(Types.Sensor sensor)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            var query = sensorDB
                .Query("lora")
                .Select("*")
                .Where("Sensor", sensor.ID)
                .OrderByDesc("ID");

            return query.FirstOrDefault<Types.LoRa>();
        }

        public IEnumerable<Models.Sensor.SensorInfo> GetSensorInfos(IEnumerable<Types.Sensor> sensoren)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            List<Models.Sensor.SensorInfo> results = new();

            foreach (var sensor in sensoren)
            {
                results.Add(new Models.Sensor.SensorInfo()
                {
                    Sensor = sensor,
                    LastPing = GetLastPing(sensor),
                    LastBarrierInterrupt = GetLastBarrierInterrupt(sensor),
                    LastLoRa = GetLastLoRa(sensor)
                });
            }

            return results;
        }
        
        public void UpdateSensor(Types.Sensor sensor)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("sensoren").Where("ID", sensor.ID).Update(sensor);

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }

        public void DeleteSensor(int id)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("sensoren").Where("ID", id).Delete();

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }

        public IEnumerable<Types.EMailRecipient> GetEMailRecipients()
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            return sensorDB.Query("email-recipients")
                            .Get<Types.EMailRecipient>();
        }

        public Types.EMailRecipient GetEMailRecipient(int id)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            return sensorDB.Query("email-recipients")
                            .Where("ID", id)
                            .FirstOrDefault<Types.EMailRecipient>();
        }


        public void DeleteEMailRecipient(int id)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("email-recipients")
                                   .Where("ID", id)
                                   .Delete();

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }

        public void UpdateEMailRecipient(Types.EMailRecipient model)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("email-recipients")
                                   .Where("ID", model.ID)
                                   .Update(model);

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }

        public void AddEMailRecipient(string address)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("email-recipients")
                                   .Insert(new { Address = address });

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }

        public IEnumerable<Types.TelegramRecipient> GetTelegramRecipients()
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            return sensorDB.Query("telegram")
                            .Get<Types.TelegramRecipient>();
        }

        public Types.TelegramRecipient GetTelegramRecipient(int id)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            return sensorDB.Query("telegram")
                            .Where("ID", id)
                            .FirstOrDefault<Types.TelegramRecipient>();
        }


        public void DeleteTelegramRecipient(int id)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("telegram")
                                   .Where("ID", id)
                                   .Delete();

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }

        public void UpdateTelegramRecipient(Types.TelegramRecipient model)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("telegram")
                                   .Where("ID", model.ID)
                                   .Update(model);

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }

        public void AddTelegramRecipient(string address)
        {
            var connection = new MySqlConnection(ConnectionString);
            var compiler = new SqlKata.Compilers.MySqlCompiler();
            var sensorDB = new QueryFactory(connection, compiler);

            sensorDB.Statement("START TRANSACTION");

            int affected = sensorDB.Query("telegram")
                                   .Insert(new { Address = address });

            if (affected != 1)
            {
                sensorDB.Statement("ROLLBACK");
                throw new ArgumentException("Number of Rows affected: " + affected);
            }
            else
            {
                sensorDB.Statement("COMMIT");
                return;
            }
        }
    }
}
