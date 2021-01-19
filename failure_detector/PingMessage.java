package cz.cvut.fel.agents.pdv.swim;
 
import cz.cvut.fel.agents.pdv.dsand.Message;
 
public class PingMessage extends Message {
    private final String pingprocess;
    public PingMessage(String pingprocess){
        this.pingprocess = pingprocess;
    }
 
 
 
    public String getPingprocess(){
        return pingprocess;
    }
 
}
