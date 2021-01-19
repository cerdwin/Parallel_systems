package cz.cvut.fel.agents.pdv.exclusion;
 
import cz.cvut.fel.agents.pdv.clocked.ClockedMessage;
import cz.cvut.fel.agents.pdv.clocked.ClockedProcess;
import cz.cvut.fel.agents.pdv.dsand.Message;
 
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Map;
 
 
public class ExclusionPrimitive {
 
    /**
     * Stavy, ve kterych se zamek muze nachazet.
     */
    enum AcquisitionState {
        RELEASED,    // Uvolneny   - Proces, ktery vlastni aktualni instanci ExclusionPrimitive o pristup do kriticke
                     //              sekce nezada
 
        WANTED,      // Chteny     - Proces, ktery vlastni aktualni instanci ExclusionPrimitive zada o pristup do
                     //              kriticke sekce. Ten mu ale zatim nebyl odsouhlasen ostatnimi procesy.
 
        HELD         // Vlastneny  - Procesu bylo prideleno pravo pristupovat ke sdilenemu zdroji.
    }
 
    private ClockedProcess owner;            // Proces, ktery vlastni aktualni instanci ExclusionPrimitive
 
    private String criticalSectionName;      // Nazev kriticke sekce. POZOR! V aplikaci se muze nachazet vice kritickych
                                             // sekci s ruznymi nazvy!
 
    private String[] allAccessingProcesses;  // Seznam vsech procesu, ktere mohou chtit pristupovat ke kriticke sekci
                                             // s nazvem 'criticalSectionName' (a tak spolurozhoduji o udelovani pristupu)
 
    private AcquisitionState state;          // Aktualni stav zamku (vzhledem k procesu 'owner').
                                             // state==HELD znamena, ze proces 'owner' muze vstoupit do kriticke sekce
    private ArrayList<MessageRequest> AwaitingReply;
    private int time;
    private HashMap<String, Boolean> Allowed;
 
    // Doplnte pripadne vlastni datove struktury potrebne pro implementaci
    // Ricart-Agrawalova algoritmu pro vzajemne vylouceni
 
    public ExclusionPrimitive(ClockedProcess owner, String criticalSectionName, String[] allProcesses) {
        this.owner = owner;
        this.criticalSectionName = criticalSectionName;
        this.allAccessingProcesses = allProcesses;
 
        // Na zacatku je zamek uvolneny
        this.state = AcquisitionState.RELEASED;
        this.AwaitingReply = new ArrayList<>();
        this.time = 0;
        this.Allowed = new HashMap<>();
 
    }
 
    /**
     * Metoda pro zpracovani nove prichozi zpravy
     *
     * @param m    prichozi zprava
     * @return 'true', pokud je zprava 'm' relevantni pro aktualni kritickou sekci.
     */
    public boolean accept(ClockedMessage m) {
 
        // Cas poslani prijate zpravy muzete zjistit dotazem na hodnotu
        // m.sentOn. Aktualni logicky cas muzete zjistit metodou owner.getTime().
        // Zpravy se posilaji pomoci owner.send(prijemce, zprava) a je jim auto-
        // maticky pridelen logicky cas odeslani. Retezec identifikujici proces
        // 'owner' je ulozeny v owner.id.
 
        if (m instanceof MessageRequest) {
 
            if(!((MessageRequest) m).criticalSectionName.equals(criticalSectionName)){
                return false;
            }
            if(((state != AcquisitionState.HELD && state != AcquisitionState.RELEASED ) && (( ( 0 > owner.id.compareTo(m.sender) )&& (time == m.sentOn)) || (time < m.sentOn))) || (state == AcquisitionState.HELD)){
                AwaitingReply.add((MessageRequest) m);
                return true;
            }
            owner.increaseTime();
            owner.send(m.sender, new AcceptableMessage(criticalSectionName));
            return true;
 
        }
 
        else if (m instanceof AcceptableMessage) {
 
            if(!((AcceptableMessage) m).criticalSectionName.equals(criticalSectionName) || !m.recipient.equals(owner.id)){
                return false;
                //
            }
            Allowed.put(m.sender, true);
            if(state != AcquisitionState.RELEASED && state != AcquisitionState.HELD){
 
                for(Map.Entry<String, Boolean> something:Allowed.entrySet()){
                    if(something.getValue() == false){
                        return true;
                    }
                }
                owner.increaseTime();
                state = AcquisitionState.HELD;
            }
            return true;
        }
        return false;
    }
 
    public void requestEnter() {
 
        // Implementujte zadost procesu 'owner' o pristup
        // ke sdilenemu zdroji 'criticalSectionName'
        state = AcquisitionState.WANTED;
        //owner.increaseTime();//////////del on sight
        for(String process : allAccessingProcesses){
            if(!process.equals(owner.id)){
                Allowed.put(process, false);
                owner.send(process, new MessageRequest(criticalSectionName));
            }
        }
        time = owner.getTime();
 
 
    }
 
    public void exit() {
 
        // Implementuje uvolneni zdroje, aby k nemu meli pristup i
        // ostatni procesy z 'allAccessingProcesses', ktere ke zdroji
        // mohou chtit pristupovat
 
        owner.increaseTime();
        state = AcquisitionState.RELEASED;
        for(MessageRequest last_message: AwaitingReply){
            owner.send(last_message.sender, new AcceptableMessage(criticalSectionName));
        }
        AwaitingReply.clear();
    }
 
    public String getName() {
        return criticalSectionName;
    }
 
    public boolean isHeld() {
        return state == AcquisitionState.HELD;
    }
 
}
