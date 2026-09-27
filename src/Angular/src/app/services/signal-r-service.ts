import { Injectable } from '@angular/core';
import * as signalR from "@microsoft/signalr"
import {BehaviorSubject} from "rxjs";

// Models
import {IMessage} from "../Models/IMessage.interface";


@Injectable({
    providedIn: 'root'
})
export class SignalRService {
    private hubConnection!: signalR.HubConnection;
    
    // A BehaviourSubject lets any component Subscribe
    private messageSubject = new BehaviorSubject<IMessage[]>([]);
    public message$ = this.messageSubject.asObservable();
    
    public startConnection(){
        this.hubConnection = new signalR.HubConnectionBuilder()
            .withUrl("http://localhost:5111/webapphub")
            .withAutomaticReconnect()
            .build();
        
        this.registerServerEvents(); // Setup Listeners before starting the Connection;
        
        this.hubConnection
            .start()
            .then(() => console.log("Connection Started"))
            .catch((err) => console.log("Error: ", err));
    }
    
    private registerServerEvents(){
        this.hubConnection.on('ReceiveMessage', (user: string, message: string) => {
            const current = this.messageSubject.value;
            this.messageSubject.next([...current, {user, message}]);
        });
    }
    
   public stopConnection(): void{
        this.hubConnection?.stop();
        
   }
    
    public sendMessage(user: string, message: string){
        this.hubConnection.invoke('SendMessage', user, message)
            .catch((err) => console.log(err));
    }

}
