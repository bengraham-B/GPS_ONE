using Serilog;
using Serilog.Sinks.Grafana.Loki;
using Server.Services; // This is the UDP Service

var builder = WebApplication.CreateBuilder(args);

// Using Loki for logging
builder.Host.UseSerilog((context, services, configuration) =>
{
    configuration
        .ReadFrom.Configuration(context.Configuration)
        .Enrich.FromLogContext()
        .Enrich.WithProperty("Application", "GPS_ONE.Server")
        .WriteTo.Console()
        .WriteTo.GrafanaLoki(
            uri: context.Configuration["Loki:Url"] ?? "http://localhost:3100",
            labels: new[]
            {
                new LokiLabel { Key = "app", Value = "GPS_ONE" },
                new LokiLabel { Key = "env", Value = context.HostingEnvironment.EnvironmentName}
            }
        );
});

builder.Services.AddControllers();
builder.Services.AddSignalR();
builder.Services.AddHostedService<UDPReceiveService>(); // <-- When ASP.Net runs, teh UDP Receives Service will also run


var app = builder.Build();
app.MapControllers();
app.Run();