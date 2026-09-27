import { Component, OnInit, signal } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { FormsModule } from "@angular/forms";
import { AsyncPipe } from "@angular/common";
import { Observable } from "rxjs";

import { SignalRService } from "./services/signal-r-service";
import { IMessage } from "./Models/IMessage.interface";

@Component({
    imports: [RouterOutlet, FormsModule, AsyncPipe],
    selector: 'app-root',
    styleUrl: './app.css',
    templateUrl: './app.html',
})
export class App implements OnInit {
    protected readonly title = signal('Angular');
    user: string = '';
    draft = '';
    
    // Declared here without a value — assigned in the constructor body,
    // AFTER signalRService has actually been set by the parameter property.
    messages$: Observable<IMessage[]>;
    
    constructor(public signalRService: SignalRService) {
        this.messages$ = this.signalRService.message$;
    }
    
    ngOnInit(): void {
        this.signalRService.startConnection();
    }
    
    sendMessage(): void {
        if (!this.draft.trim()) return;
        this.signalRService.sendMessage(this.user || "Sam", this.draft);
        this.draft = '';
    }
}