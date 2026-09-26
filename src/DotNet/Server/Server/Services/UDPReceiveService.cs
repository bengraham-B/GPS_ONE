using System.Diagnostics.CodeAnalysis;
using System.Net.Sockets;
using System.Text;

/*
 * Background Services
 * These run for the entire lifetime of the App.
 * Will listen on a socket alongside the API
 */

namespace Server.Services;

[SuppressMessage("ReSharper", "InconsistentNaming")]
public class UDPReceiveService : BackgroundService
{
    // Injecting the Logger
    private readonly ILogger<UDPReceiveService> _logger;
    
    // Port the UPD Receiver will listen on.
    private const int port = 5005;
    
    // Constructor - DI Supplies the logger automatically because this class is registered via AddHostedService<UdpReceiverService>() in program.cs
    public UDPReceiveService(ILogger<UDPReceiveService> logger)
    {
        _logger = logger;
    }
    
    /*
     * This is required by background service to be implemented
     * The host calls this once, automatically, right after the app finishes starting up.
     * "override is required"
     * 'Stoping Token' is a cancellation taken the host controls, it stays "not canceled" for the duration the app is running. It then gets canceled automatically when the app shuts down. (Ctrl + c)
     */
    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        
        // UDPClient wraps a raw UDP Socket, passing a port number means we bind to this port and begin listening
        // The "using" ensures that the socket is properly closed/released when method exists. From shutdown or exception
        using var UDPClient = new UdpClient(port);
        
        _logger.LogInformation("UPD Receiver listening on Port: {port}", port); // This logs once on startup

        /*
         * Main UDP Receive Loop
         * This runs for as long as the Application is alive
         * "stoppingToken.IsCancellationRequested" -> Becomes true when the app begins shutting down, thus ending the loop.
         */
        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                /*
                 * ReceiveAsync suspends this loop (without blocking a thread) until a UDP packet arrives on the port, OR stopping token is canceled. | Whatever happens first.
                 * This thread does not burn CPU sitting idle as the thread is freed back to the pool whilewaiting.
                 *
                 * Passing the shutdown token here is what allows shutdown to interupt a receceive which is currently waiting for data.
                 */
                UdpReceiveResult result = await UDPClient.ReceiveAsync(stoppingToken);

                // Result.Buffer is the raw bytes received in this one packet .
                // UDP is message based, so each ReceiveAsync call gives you exactly one datagram as it was sent - no manual framing needed.
                string message = Encoding.UTF8.GetString(result.Buffer);

                /* Result.RemoteEdnPoint - tells us who sent the packet (IP + PORT)
                 * The {Placeholder} syntax is structured logging, each value becomes its own labeld field in the log output.
                 * Allowing for filtering on filter/query on Sender or bytes
                 */

                _logger.LogInformation("Received {Bytes} bytes from {Sender}: {Message}", result.Buffer.Length,
                    result.RemoteEndPoint, message);


            }
            catch (OperationCanceledException)
            {
                // ReceiveAsync throws this specific exception when
                // stoppingToken is cancelled WHILE it was waiting for a
                // packet. This is the expected, clean shutdown path —
                // not an error — so we just exit the loop quietly.
                break;

            }
            catch (Exception e)
            {
                // Any other exception (e.g. a malformed packet causing
                // a decoding error) is logged but does NOT rethrow.
                // Without this catch, one bad packet would throw out of
                // ExecuteAsync entirely, silently killing the receiver
                // for the rest of the app's lifetime — the loop would
                // just stop, with no obvious symptom besides "no more
                // GPS data ever arrives."
                _logger.LogError(e, "Error receiving UDP packet");

            }
        }

        // Logged once, on the way out, so it's visible in the logs that
        // the receiver shut down deliberately (as opposed to just
        // vanishing) when the app stops.
        _logger.LogInformation("UDP receiver stopped");
    
       
    }
}