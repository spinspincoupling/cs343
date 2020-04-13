#include "cardoffice.h"
#include "printer.h"
#include "watcard.h"
#include "bank.h"
#include "MPRNG.h"
#include <uFuture.h>

extern MPRNG mprng;	

WATCardOffice::Courier::Courier(Printer &prt, Bank &bank, WATCardOffice *office, unsigned int id)
    :prt{prt}, bank{bank}, office{office}, id{id} {
        prt.print(Printer::Kind::WATCardOfficeCourier, id, 'S');
}

WATCardOffice::Courier::~Courier(){
    prt.print(Printer::Kind::WATCardOfficeCourier, id, 'F');
}

void WATCardOffice::Courier::main(){
    for(;;){
            Job *w = office->requestWork();
            if(w == nullptr) break;
            prt.print(Printer::Kind::WATCardOfficeCourier, id, 't', w->sid, w->amount);
            bank.withdraw(w->sid, w->amount);
            WATCard *watcard = w->card == nullptr? new WATCard():w->card;
            watcard->deposit(w->amount);
            if(mprng(5) == 0){
                prt.print(Printer::Kind::WATCardOfficeCourier, id, 'L', w->sid);
                w->result.exception(new WATCardOffice::Lost());
                delete watcard;
            } else {
                prt.print(Printer::Kind::WATCardOfficeCourier, id, 'T', w->sid, w->amount);
                w->result.delivery(watcard);
            }
            delete w;
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
    if(workQueue.empty()) return nullptr;
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
            workQueue.clear();
            for (unsigned int i=0; i<numCouriers; ++i){ //wait for courier to terminate
                _Accept(requestWork); 
            }
            for (unsigned int i=0; i<numCouriers; ++i){
                delete couriers[i];
            }
            break;
        }
        or _When(!workQueue.empty()) _Accept(requestWork){
        }
        or _Accept(create, transfer){
        }
    }
}