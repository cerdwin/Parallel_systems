package cz.cvut.fel.agents.pdv.swim;
 
import cz.cvut.fel.agents.pdv.dsand.Message;
 
public class AckMessage extends Message {
    private final String ackid;
    public AckMessage (String ackid){
        this.ackid = ackid;
    }
    public String getAckid(){
        return ackid;
    }
}
