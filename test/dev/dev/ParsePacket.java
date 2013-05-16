

import java.util.StringTokenizer;
import java.util.Vector;

public class ParsePacket {
	private int numseq; 
	private String idSrc; 
	private String idDest;
	private long ttl;
	private String data;
	
	public ParsePacket (){
	    super();
	    this.numseq = 0;
	    this.idSrc = "";
	    this.idDest = "";
	    this.ttl = 0;
	    this.data = "";
	}
    
	public ParsePacket(int numseq, String idSrc, String idDest, long ttl, String data) {
	    super();
		this.numseq = numseq;
		this.idSrc = idSrc;
		this.idDest = idDest;
		this.ttl = ttl;
		this.data = data;
	}


	public ParsePacket parsing(String message){
		
	    StringTokenizer tokens = new StringTokenizer(message);
	    tokens.nextToken();
	    tokens.nextToken();
	    int seqNum =Integer.parseInt(tokens.nextToken());
	    tokens.nextToken();
	    String src = tokens.nextToken();
	    tokens.nextToken();
	    String dest = tokens.nextToken();
	    
	    long ttl = 0;
	    String lastparm = tokens.nextToken();
		 
		 if( lastparm.equals("ok*")){
			 lastparm  = "ok*";  
		 }
		 else if( lastparm.equals("toofar*")){
			 lastparm = "toofar*";
		 }
		 else {
			
			 ttl = Long.parseLong(tokens.nextToken());
			 tokens.nextToken();
			 int size = tokens.countTokens();
			 int i  = 0; 
			 lastparm ="";
			 while (i < size-1){
				
				 lastparm = lastparm + " " +tokens.nextToken(); 
				 i++;
			 }
			 String  lastwork = tokens.nextToken();
			 lastparm = lastparm + " " + lastwork.substring(0, lastwork.length()-1);
		 } 
		 ParsePacket packet = new ParsePacket(seqNum, src, dest, ttl, lastparm); 
		 return packet; 
	}
    
    @Override
	public String toString() {
	return "packet seqnum " + numseq + " src " + idSrc + " dst "
	    + idDest + " ttl " + ttl + " data " + data;
    }
    public String constractPacketData(int seqNum, String idSrc, String idDst,long ttl, String data ){
    	return "packet seqnum "+seqNum+" src "+idSrc+ " dst "+idDst+ " ttl "+ttl+" data "+data+"*";
    }
    public String constractPacketOk(int seqNum, String idSrc, String idDst){
    	return "packet seqnum "+seqNum+" src "+idSrc+ " dst "+idDst+ " ok*";
    }
    public String constractPacketTooFar(int seqNum, String idSrc, String idDst){
    	return "packet seqnum "+seqNum+" src "+idSrc+ " dst "+idDst+ " toofar*";
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
    
    public String getData() {
	return data;
    }
    
    public void setData(String data) {
	this.data = data;
    }
	
}