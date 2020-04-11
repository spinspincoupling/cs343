#include "cardoffice.h"
#include "printer.h"
#include "watcard.h"
#include "bank.h"
#include "global.h"
#include "MPRNG.h"
#include <uFuture.h>
#include <iostream>

WATCardOffice::Courier::Courier(Printer &prt, Bank &bank, WATCardOffice *office, unsigned int id)
    :prt{prt}, bank{bank}, office{office}, id{id} {}

WATCardOffice::Courier::~Courier(){
    prt.print(Printer::Kind::WATCardOfficeCourier, id, 'F');
}

void WATCardOffice::Courier::main(){
    prt.print(Printer::Kind::WATCardOfficeCourier, id, 'S');
    for(;;){
        _Accept(~Courier){
            break;
        } _Else{
            Job *w = office->requestWork();
            if(w == nullptr) break;
            prt.print(Printer::Kind::WATCardOfficeCourier, id, 't', w->sid, w->amount);
            bank.withdraw(w->sid, w->amount);
            WATCard *watcard = w->card == nullptr? new WATCard() : w->card; //create or transfer
            watcard->deposit(w->amount);
            if(mprng(5) == 0){
                prt.print(Printer::Kind::WATCardOfficeCourier, id, 'L', w->sid);
                try{
                    WATCardOffice::Lost* e = new WATCardOffice::Lost();
                    _Resume *e;
                } _CatchResume(WATCardOffice::Lost &e) {
                    w->result.exception(&e);
                    if(w->card == nullptr) delete watcard;
                }
            } else {
                prt.print(Printer::Kind::WATCardOfficeCourier, id, 'T', w->sid, w->amount);
                w->result.delivery(watcard);
            }
            delete w;
        }
        
    }
}

WATCardOffice::WATCardOffice( Printer & prt, Bank & bank, unsigned int numCouriers )
    :prt{prt}, bank{bank},numCouriers{numCouriers}{}

WATCardOffice::~WATCardOffice(){
    prt.print(Printer::Kind::WATCardOffice, 'F');
}

WATCard::FWATCard WATCardOffice::create( unsigned int sid, unsigned int amount ){
    Job *w = new Job(sid, amount);
    workQueue.push_back(w);
    prt.print(Printer::Kind::WATCardOffice, 'C', sid, amount);
    return w->result;
}

WATCard::FWATCard WATCardOffice::transfer( unsigned int sid, unsigned int amount, WATCard * card ){
    Job *w = new Job(sid, amount, card);
    workQueue.push_back(w);
    prt.print(Printer::Kind::WATCardOffice, 'T', sid, amount);
    return w->result;
}

WATCardOffice::Job * WATCardOffice::requestWork(){
    if(workQueue.empty()){
        waiting.wait();
        if(workQueue.empty()) return nullptr;
    }
    Job *w = workQueue.front();
    workQueue.pop_front();
    prt.print(Printer::Kind::WATCardOffice, 'W');
    return w;
}

void WATCardOffice::main(){
    prt.print(Printer::Kind::WATCardOffice, 'S');
    Courier* couriers[numCouriers];
    for(unsigned int i=0; i<numCouriers; ++i){
        couriers[i] = new Courier(prt, bank, this, i);
    }
    for(;;){
        _Accept(~WATCardOffice){
            //for (auto &p:couriers){
            //    p->stop();
            //}
            while(!waiting.empty()){
                waiting.signalBlock();
            }
            for (auto &p:couriers){
                delete p;
            }
            break;
        }
        or _Accept(requestWork){
        }
        or _Accept(create, transfer){
            if(!waiting.empty()){
                waiting.signalBlock();
            }
        }
    }
}