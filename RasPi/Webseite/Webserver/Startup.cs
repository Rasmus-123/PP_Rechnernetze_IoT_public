using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.HttpsPolicy;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using Webserver.Lib;
using Webserver.Data;

namespace Webserver
{
    public class Startup
    {
        public IConfiguration Configuration { get; }
        public IWebHostEnvironment WebHostEnvironment { get; }
        private readonly ILogger<Startup> Logger;

        public Startup(IConfiguration configuration, IWebHostEnvironment environment)
        {
            Configuration = configuration;
            WebHostEnvironment = environment;

            ApplicationLogging.LoggerFactory = LoggerFactory.Create(builder => {
                builder
                    .AddFilter("Microsoft", LogLevel.Warning)
                    .AddFilter("System", LogLevel.Warning)
                    .AddFilter("SBTest.Startup", LogLevel.Information)
                    .AddConsole();
            });

            Logger = ApplicationLogging.LoggerFactory.CreateLogger<Startup>();
        }


        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddControllersWithViews()
                .AddRazorRuntimeCompilation();
            services.Add(
                new ServiceDescriptor(
                    typeof(SensorDBContext),
                    new SensorDBContext(
                    Configuration.GetConnectionString("SensorDB")
                    )));

            // Authentication
            if (Configuration.GetSection("Options")["Authentication"] == "yes")
            {
                services.AddMvc(options =>
                {
                    var userName = Configuration.GetSection("Options")["UserName"];
                    var userPw = Configuration.GetSection("Options")["UserPassword"];
                    options.Filters.Add(new Attributes.BasicAuthenticationAttribute(userName, userPw));
                });
            }

            if (WebHostEnvironment.IsDevelopment())
            {
                services.AddWebOptimizer(minifyJavaScript: false, minifyCss: false);
            }
            else
            {
                services.AddWebOptimizer();
            }
        }

        private void LogStartupInfo()
        {
            string buildDetails = "" +
                "Application: " + Assembly.GetExecutingAssembly().GetName().Name + "\n" +
          "      Version: " + Assembly.GetExecutingAssembly().GetName().Version + "\n" +
          "      Build-Mode: " + ThisAssembly.BuildMode + "\n" +
          "      Build-Time: " + ThisAssembly.GetBuildTimestamp().ToLocalTime() + "\n" +
          "      Branch: " + ThisAssembly.Git.Branch + "\n" +
          "      Last Commit: " + ThisAssembly.Git.Commit + "\n" +
          "      Uncommited Changes: " + ThisAssembly.Git.IsDirty;

            string runDetails = "" +
                "Current Timestamp: " + DateTime.Now.ToString() + "\n" +
          "      Environment: " + WebHostEnvironment.EnvironmentName + "\n" +
          "      Authentication: " + Configuration.GetSection("Options")["Authentication"] + "\n" +
          "      Runtime: " + Environment.Version.ToString() + "\n" +
          "      OS: " + Environment.OSVersion.ToString();

            Logger.LogInformation(buildDetails);
            Logger.LogInformation(runDetails);
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            LogStartupInfo();

            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }
            else
            {
                app.UseExceptionHandler("/Home/Error");
                // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
                app.UseHsts();

                app.UseWebOptimizer();
            }
            app.UseHttpsRedirection();
            app.UseStaticFiles();

            app.UseRouting();

            app.UseAuthorization();

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllerRoute(
                    name: "default",
                    pattern: "{controller=Home}/{action=Index}/{id?}");
            });
        }
    }
}
