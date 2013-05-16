

import java.util.StringTokenizer;
import java.util.Vector;

public class ParsePing {
    private int numseq; 
    private String idSrc; 
    private String idDest;
    private long ttl;
    private boolean ttlzero;
    
    public ParsePing (){
	this.numseq = 0;
	this.idSrc = "";
	this.idDest = "";
	this.ttl = 0;
	this.ttlzero = false;
    }
    
    public ParsePing(int numseq, String idSrc, String idDest, long ttl,boolean ttlezro) {
	this.numseq = numseq;
	this.idSrc = idSrc;
	this.idDest = idDest;
	this.ttl = ttl;
	this.ttlzero = ttlzero;
    }
    
    
    public ParsePing parsing(String message){
	
	StringTokenizer tokens = new StringTokenizer(message);
	tokens.nextToken();
	tokens.nextToken();
	int seqNum =Integer.parseInt(tokens.nextToken());
	tokens.nextToken();
	String src = tokens.nextToken();
	tokens.nextToken();
	String dest = tokens.nextToken();
	
	long ttl = 0;
	boolean ttlzero = false;
	String lastparm = tokens.nextToken();
	if(lastparm.equals("ttl")){
	    String ttlValue = tokens.nextToken();
	    ttl = Long.parseLong(ttlValue.substring(0,ttlValue.length() - 1));
	}
	else
	    ttlzero = true;
	
	ParsePing ping = new ParsePing(seqNum, src, dest, ttl,ttlzero); 
	return ping; 
	}
    
    @Override
	public String toString() {
	return "packet seqnum " + numseq + " src " + idSrc + " dst "
	    + idDest + " ttl " + ttl;
    }
    public String constractPing(int seqNum, String idSrc, String idDst,long ttl){
    	return "ping seqnum "+seqNum+" src "+idSrc+ " dst "+idDst+ " ttl "+ttl+"*";
    }
    public String constractPongTrue(int seqNum, String idSrc, String idDst,long ttl){
    	return "pong seqnum "+seqNum+" src "+idSrc+ " dst "+idDst+" ttl "+ttl+"*";
    }
    public String constractPongFalse(int seqNum, String idSrc, String idDst){
    	return "pong seqnum "+seqNum+" src "+idSrc+ " dst "+idDst+ " ttlzero*";
    }
	public int getNumseq() {
	    return numseq;
	}
    
    public void setNumseq(int numseq) {
	this.numseq = numseq;
    }
    
    public String getIdSrc() {
	return idSrc;
    }
    
    public void setIdSrc(String idSrc) {
	this.idSrc = idSrc;
    }
    
    public String getIdDest() {
	return idDest;
    }
    
    public void setIdDest(String idDest) {
	this.idDest = idDest;
    }
    
    public long getTtl() {
	return ttl;
    }
    
    public void setTtl(long ttl) {
	this.ttl = ttl;
	}
    
   	
}