using Microsoft.AspNetCore.SignalR;

namespace Server.Hubs;

public class WebAppHub: Hub
{
    public async Task SendMessage(string user, string message)
    {
        /*
         * Clients.All.SendAsync => This broadcasts the message to all connected clients
         */
        await Clients.All.SendAsync("ReceiveMessage", user, message);
    }
    
}