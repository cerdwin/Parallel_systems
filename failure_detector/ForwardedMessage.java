package cz.cvut.fel.agents.pdv.swim;
 
import cz.cvut.fel.agents.pdv.dsand.Message;
 
public class ForwardedPings extends Message {
    private final String forwardprocess;
    public ForwardedPings(String forwardprocess){
        this.forwardprocess = forwardprocess;
    }
    public String getForwardprocess(){
        return forwardprocess;
    }
 
 
}
