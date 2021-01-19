package cz.cvut.fel.agents.pdv.swim;
 
import cz.cvut.fel.agents.pdv.dsand.Message;
 
public class DeadPings extends Message {
    private final String deadprocess;
    public DeadPings(String deadprocess){
        this.deadprocess = deadprocess;
    }
    public String getDeadprocess(){
        return deadprocess;
    }
 
}
