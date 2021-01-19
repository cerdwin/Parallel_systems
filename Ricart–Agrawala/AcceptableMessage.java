package cz.cvut.fel.agents.pdv.exclusion;
 
import cz.cvut.fel.agents.pdv.clocked.ClockedMessage;
 
public class AcceptableMessage extends ClockedMessage {
 
    protected String criticalSectionName;
 
    public AcceptableMessage(String criticalSectionName) {
        this.criticalSectionName = criticalSectionName;
    }
}
