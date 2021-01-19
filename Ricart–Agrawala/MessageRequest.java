package cz.cvut.fel.agents.pdv.exclusion;
 
import cz.cvut.fel.agents.pdv.clocked.ClockedMessage;
 
public class MessageRequest extends ClockedMessage {
    protected String criticalSectionName;
    public MessageRequest(String criticalSectionName){this.criticalSectionName = criticalSectionName;}
 
}
