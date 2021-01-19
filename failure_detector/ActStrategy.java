package cz.cvut.fel.agents.pdv.swim;
import cz.cvut.fel.agents.pdv.dsand.Message;
import cz.cvut.fel.agents.pdv.dsand.Pair;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Random;
import java.util.*;
 
public class ActStrategy {
 
    // maximalni zpozdeni zprav
    private final int maxDelayForMessages;
    private final List<String> otherProcesses;
 
    // Definujte vsechny sve promenne a datove struktury, ktere budete potrebovat
    private int generation_counter;
    private final Random die;
    private final LinkedHashMap<String, Integer> deletebox;
    private int mylimitcounter;
    private final List<String> deadset;
    private final List<Pair<String, Message>> outbox;
    private final HashMap<String, LinkedHashSet<String>> Awaited;
 
 
    public ActStrategy(int maxDelayForMessages, List<String> otherProcesses,
                       int timeToDetectKilledProcess, int upperBoundOnMessages) {
        deletebox = new LinkedHashMap<>();
 
        this.maxDelayForMessages = maxDelayForMessages;
        this.otherProcesses = otherProcesses;
        Awaited = new HashMap<>();
        generation_counter = -1;
        mylimitcounter = upperBoundOnMessages;
        die = new Random();
        outbox = new ArrayList<>();
        deadset = new ArrayList<>();
        // Doplnte inicializaci
    }
 
    public List<Pair<String, Message>> act(Queue<Message> inbox, String disseminationProcess) {
 
        outbox.clear();
        generation_counter++;
        for(;;){
            Message msg = inbox.poll();
            if(msg == null)break;
            if(msg instanceof PingMessage && mylimitcounter > 0 && !deadset.contains(msg.sender)){ // sb sent me a ping and I can still reply
                PingMessage temp = (PingMessage) msg;
                String from = temp.sender;
                if(mylimitcounter > 0){
                    outbox.add(new Pair<>(from, new AckMessage(temp.getPingprocess())));
                    mylimitcounter--;
                }
            }
            if(msg instanceof PFDMessage){
                PFDMessage temppfd = (PFDMessage) msg;
            }
            if(msg instanceof DeadPings){
                DeadPings tempdead = (DeadPings) msg;
                // we will no longer be working with this dead case
                otherProcesses.remove(tempdead.getDeadprocess());
                deadset.add(tempdead.getDeadprocess());
                // maybe think about creating a list of dead processes for safekeeping
            } //////////
            if(msg instanceof ForwardedPings && !deadset.contains(msg.recipient)){ // someone has either replied to me, or to somebody else and I should pass it on
                ForwardedPings temprepl = (ForwardedPings) msg;
                String sender = temprepl.sender;
                Awaited.put(temprepl.getForwardprocess(), new LinkedHashSet<>());
                Awaited.get(temprepl.getForwardprocess()).add(sender);
                if(mylimitcounter > 0){
                    outbox.add(new Pair<>(temprepl.getForwardprocess(), new PingMessage(temprepl.getForwardprocess())));
                    mylimitcounter--;
                }
            }
            if(msg instanceof AckMessage && !deadset.contains(((AckMessage) msg).sender) && !deadset.contains(((AckMessage) msg).getAckid())) {
                AckMessage tempack = (AckMessage) msg;
                if(Awaited.containsKey(tempack.getAckid())){
                    for(String destination: Awaited.get(tempack.getAckid())){
                       if(mylimitcounter >0){
                           outbox.add(new Pair<>(destination, tempack));
                           mylimitcounter--;
                       }
                       else{break;}
                    }
                   Awaited.remove(tempack.getAckid());
                }
                deletebox.remove(tempack.getAckid());
            }
        }
        inbox.clear();
        // when I want to pick whom to send other message to
        int randchoice = die.nextInt(500);
        if(randchoice > 250){
            int randomIndex = die.nextInt(otherProcesses.size());
            String randomRecipient = otherProcesses.get(randomIndex); //https://www.baeldung.com/java-random-list-element
            if(Awaited.containsKey(randomRecipient)){
                while(Awaited.containsKey(randomRecipient)){
                    randomIndex = die.nextInt(otherProcesses.size());
                    randomRecipient = otherProcesses.get(randomIndex);
                }
            }
            if(mylimitcounter>0){
                outbox.add(new Pair<>(randomRecipient,new PingMessage(randomRecipient)));
                mylimitcounter--;
            }
 
            deletebox.put(randomRecipient, generation_counter); /////////////////
 
        }
        // when i want to check dead processes
        if(true){
            for(Map.Entry<String, Integer> entry : deletebox.entrySet()){
                String key = entry.getKey();
                Integer timesnap = entry.getValue();
                if(timesnap + 9*maxDelayForMessages-maxDelayForMessages%16 < generation_counter && !deadset.contains(key) && otherProcesses.contains(key)){
                    otherProcesses.remove(key); // we no longer will be pinging these
                    deadset.add(key);
 
                   if(mylimitcounter > 0){
                       outbox.add(new Pair<>(disseminationProcess, new PFDMessage(key)));
                       mylimitcounter--;
                   }
                }else if(timesnap + 3*maxDelayForMessages - maxDelayForMessages%3 == generation_counter && mylimitcounter >0/*&& !deletebox.contains(key) */){
                    ///////////// not sure
                    if(!deletebox.containsKey(key)){
                        deletebox.put(key,generation_counter);
                    }
                    /////////// not sure
                    for(int i = 0; i < 10; i++){
                        int randommidpoint = die.nextInt(otherProcesses.size());
                        String randomRecipient = otherProcesses.get(randommidpoint);
                        if(mylimitcounter>0){
                            outbox.add(new Pair<>(randomRecipient, new ForwardedPings(entry.getKey())));
                            mylimitcounter--;
                        }else{break;}
                    }
 
                }
            }
 
       }
 
        return outbox;
    }
 
}
