using Serilog;
using Serilog.Sinks.Grafana.Loki;
using Server.Hubs;
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

builder.Services.AddCors(options =>
{
    options.AddPolicy("AngularClient", policy =>
    {
        policy.WithOrigins("http://localhost:3300")
            .AllowAnyHeader()
            .AllowAnyMethod()
            .AllowCredentials(); // Required for SignalR's websocket
    });
});

builder.Services.AddControllers();
builder.Services.AddSignalR();
builder.Services.AddHostedService<UDPReceiveService>(); // <-- When ASP.Net runs, teh UDP Receives Service will also run


var app = builder.Build();
app.UseRouting();
app.UseCors("AngularClient");
app.MapHub<WebAppHub>("/webapphub").RequireCors("AngularClient");
app.MapControllers();
app.Run();